#define _WIN32_WINNT 0x0A00 

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <sql.h>
#include "httplib.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>

// 2. Load Configuration (Connection String, Port, Limits)
#include "../config/db_config.h"

// COMPILATION: g++ main.cpp -o app.exe -lws2_32 -lodbc32

// --- HELPERS ---
std::string loadFile(std::string filename) {
    std::ifstream file("../frontend/" + filename);
    if (!file.is_open()) return "<h1>Error: 404 Not Found (" + filename + ")</h1>";
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void replaceAll(std::string& str, const std::string& from, const std::string& to) {
    if(from.empty()) return;
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}

// --- DATABASE FUNCTIONS ---

int db_check_login(std::string user, std::string pass, bool &isAdmin) {
    SQLHENV hEnv; SQLHDBC hDbc; SQLHSTMT hStmt;
    SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
    SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
    SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);
    // Use CONN_STR from config
    SQLDriverConnect(hDbc, NULL, (SQLCHAR*)CONN_STR, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT);
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

    std::string query = "SELECT UserID, IsAdmin FROM Users WHERE Username='" + user + "' AND PasswordHash='" + pass + "'";
    SQLExecDirect(hStmt, (SQLCHAR*)query.c_str(), SQL_NTS);

    int id = -1;
    long adminBit = 0;
    SQLLEN indicator;

    if (SQLFetch(hStmt) == SQL_SUCCESS) {
        SQLGetData(hStmt, 1, SQL_C_LONG, &id, 0, &indicator);
        SQLGetData(hStmt, 2, SQL_C_LONG, &adminBit, 0, &indicator);
        isAdmin = (adminBit == 1);
    }

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    SQLDisconnect(hDbc);
    SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
    SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
    return id;
}

bool db_register_user(std::string user, std::string email, std::string pass, std::string role, std::string secretKey) {
    SQLHENV hEnv; SQLHDBC hDbc; SQLHSTMT hStmt;
    SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
    SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
    SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);
    SQLDriverConnect(hDbc, NULL, (SQLCHAR*)CONN_STR, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT);
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

    std::string isAdmin = "0";
    if (role == "admin" && secretKey == "RAILWAY_MASTER") isAdmin = "1";
    else if (role == "admin") return false;

    std::string query = "INSERT INTO Users (Username, Email, PasswordHash, IsAdmin) VALUES ('" + user + "', '" + email + "', '" + pass + "', " + isAdmin + ")";
    SQLRETURN ret = SQLExecDirect(hStmt, (SQLCHAR*)query.c_str(), SQL_NTS);
    bool success = SQL_SUCCEEDED(ret);

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    SQLDisconnect(hDbc);
    SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
    SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
    return success;
}

std::string db_get_all_trains() {
    SQLHENV hEnv; SQLHDBC hDbc; SQLHSTMT hStmt;
    SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
    SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
    SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);
    SQLDriverConnect(hDbc, NULL, (SQLCHAR*)CONN_STR, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT);
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

    std::string query = "SELECT T.TrainID, T.TrainName, S1.StationName, S2.StationName, T.TravelDate, T.Price, T.TotalSeats "
                        "FROM Trains T "
                        "JOIN Stations S1 ON T.SourceStationID = S1.StationID "
                        "JOIN Stations S2 ON T.DestStationID = S2.StationID";

    SQLExecDirect(hStmt, (SQLCHAR*)query.c_str(), SQL_NTS);

    std::string rows = "";
    long tId, seats;
    double price;
    char tName[100], src[50], dest[50], date[50];
    SQLLEN ind;

    while (SQLFetch(hStmt) == SQL_SUCCESS) {
        SQLGetData(hStmt, 1, SQL_C_LONG, &tId, 0, &ind);
        SQLGetData(hStmt, 2, SQL_C_CHAR, tName, sizeof(tName), &ind);
        SQLGetData(hStmt, 3, SQL_C_CHAR, src, sizeof(src), &ind);
        SQLGetData(hStmt, 4, SQL_C_CHAR, dest, sizeof(dest), &ind);
        SQLGetData(hStmt, 5, SQL_C_CHAR, date, sizeof(date), &ind);
        SQLGetData(hStmt, 6, SQL_C_DOUBLE, &price, 0, &ind);
        SQLGetData(hStmt, 7, SQL_C_LONG, &seats, 0, &ind);

        std::string dateStr = std::string(date).substr(0, 10);
        std::string route = std::string(src) + " &rarr; " + std::string(dest);
        std::stringstream priceStream;
        priceStream << "$" << std::fixed << std::setprecision(2) << price;

        rows += "<tr><td><b>" + std::to_string(tId) + "</b></td><td>" + std::string(tName) + "</td><td>" + route + "</td><td>" + dateStr + "</td><td>" + priceStream.str() + "</td><td>" + std::to_string(seats) + "</td></tr>";
    }

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    SQLDisconnect(hDbc);
    SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
    SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
    return rows;
}

std::string db_get_history(std::string userId) {
    SQLHENV hEnv; SQLHDBC hDbc; SQLHSTMT hStmt;
    SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
    SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
    SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);
    SQLDriverConnect(hDbc, NULL, (SQLCHAR*)CONN_STR, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT);
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

    std::string query = "SELECT B.BookingID, T.TrainName, S1.StationName, S2.StationName, B.Status "
                        "FROM Bookings B "
                        "JOIN Trains T ON B.TrainID = T.TrainID "
                        "JOIN Stations S1 ON T.SourceStationID = S1.StationID "
                        "JOIN Stations S2 ON T.DestStationID = S2.StationID "
                        "WHERE B.UserID=" + userId;

    SQLExecDirect(hStmt, (SQLCHAR*)query.c_str(), SQL_NTS);

    std::string rows = "";
    char tName[100], src[50], dest[50], status[50];
    int bId;
    SQLLEN ind;

    while (SQLFetch(hStmt) == SQL_SUCCESS) {
        SQLGetData(hStmt, 1, SQL_C_LONG, &bId, 0, &ind);
        SQLGetData(hStmt, 2, SQL_C_CHAR, tName, sizeof(tName), &ind);
        SQLGetData(hStmt, 3, SQL_C_CHAR, src, sizeof(src), &ind);
        SQLGetData(hStmt, 4, SQL_C_CHAR, dest, sizeof(dest), &ind);
        SQLGetData(hStmt, 5, SQL_C_CHAR, status, sizeof(status), &ind);
        std::string display = std::string(tName) + " (" + std::string(src) + " -> " + std::string(dest) + ")";
        rows += "<tr><td>" + std::to_string(bId) + "</td><td>" + display + "</td><td>" + std::string(status) + "</td></tr>";
    }
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    SQLDisconnect(hDbc);
    SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
    SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
    return rows;
}

std::string db_get_passengers_by_train(std::string trainId) {
    SQLHENV hEnv; SQLHDBC hDbc; SQLHSTMT hStmt;
    SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
    SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
    SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);
    SQLDriverConnect(hDbc, NULL, (SQLCHAR*)CONN_STR, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT);
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

    std::string query = "SELECT U.Username, B.Status FROM Bookings B JOIN Users U ON B.UserID = U.UserID WHERE B.TrainID = " + trainId;
    SQLExecDirect(hStmt, (SQLCHAR*)query.c_str(), SQL_NTS);

    std::string rows = "";
    char uName[100], status[50];
    SQLLEN ind;
    bool found = false;

    while (SQLFetch(hStmt) == SQL_SUCCESS) {
        found = true;
        SQLGetData(hStmt, 1, SQL_C_CHAR, uName, sizeof(uName), &ind);
        SQLGetData(hStmt, 2, SQL_C_CHAR, status, sizeof(status), &ind);
        rows += "<tr><td>" + std::string(uName) + "</td><td>" + std::string(status) + "</td></tr>";
    }
    if (!found) rows = "<tr><td colspan='2'>No passengers found for Train ID " + trainId + "</td></tr>";
    
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    SQLDisconnect(hDbc);
    SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
    SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
    return rows;
}

// *** NEW ATOMIC BOOKING FUNCTION (Thread-Safe) ***
bool db_book_ticket(std::string userId, std::string trainId) {
    SQLHENV hEnv; SQLHDBC hDbc; SQLHSTMT hStmt;
    SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
    SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
    SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);
    SQLDriverConnect(hDbc, NULL, (SQLCHAR*)CONN_STR, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT);
    
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

    // ATOMIC SQL: "Insert ONLY if Count < MAX_SEATS"
    // This prevents race conditions where two threads read '49' at the same time.
    std::string query = 
        "INSERT INTO Bookings (UserID, TrainID, Status) "
        "SELECT " + userId + ", " + trainId + ", 'CONFIRMED' "
        "WHERE (SELECT COUNT(*) FROM Bookings WHERE TrainID = " + trainId + ") < " + std::to_string(MAX_SEATS_PER_TRAIN);

    SQLExecDirect(hStmt, (SQLCHAR*)query.c_str(), SQL_NTS);

    SQLLEN rowsAffected = 0;
    SQLRowCount(hStmt, &rowsAffected);

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    SQLDisconnect(hDbc);
    SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
    SQLFreeHandle(SQL_HANDLE_ENV, hEnv);

    return (rowsAffected > 0);
}

void db_add_train(std::string name, std::string srcId, std::string destId, std::string date, std::string price, std::string seats) {
    SQLHENV hEnv; SQLHDBC hDbc; SQLHSTMT hStmt;
    SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
    SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
    SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);
    SQLDriverConnect(hDbc, NULL, (SQLCHAR*)CONN_STR, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT);
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

    std::string query = "INSERT INTO Trains (TrainName, SourceStationID, DestStationID, TravelDate, Price, TotalSeats) VALUES ('" 
                        + name + "', " + srcId + ", " + destId + ", '" + date + "', " + price + ", " + seats + ")";
    
    SQLExecDirect(hStmt, (SQLCHAR*)query.c_str(), SQL_NTS);

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    SQLDisconnect(hDbc);
    SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
    SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
}

// --- MAIN SERVER ---
int main() {
    httplib::Server svr;

    svr.Get("/", [](const httplib::Request& req, httplib::Response& res) { res.set_content(loadFile("index.html"), "text/html; charset=utf-8"); });
    svr.Get("/user/login", [](const httplib::Request& req, httplib::Response& res) { res.set_content(loadFile("user_login.html"), "text/html; charset=utf-8"); });
    svr.Get("/signup", [](const httplib::Request& req, httplib::Response& res) { res.set_content(loadFile("user_signup.html"), "text/html; charset=utf-8"); });
    svr.Get("/admin/login", [](const httplib::Request& req, httplib::Response& res) { res.set_content(loadFile("admin_login.html"), "text/html; charset=utf-8"); });
    svr.Get("/admin/signup", [](const httplib::Request& req, httplib::Response& res) { res.set_content(loadFile("admin_signup.html"), "text/html; charset=utf-8"); });

    svr.Post("/login", [](const httplib::Request& req, httplib::Response& res) {
        std::string user = req.get_param_value("username");
        std::string pass = req.get_param_value("password");
        std::string expectedRole = req.get_param_value("role");

        bool isAdmin = false;
        int userId = db_check_login(user, pass, isAdmin);

        if (userId == -1) {
            res.set_content("<h1>Login Failed</h1><p>Incorrect Username/Password.</p><a href='/'>Go Back</a>", "text/html; charset=utf-8");
            return;
        }
        if (expectedRole == "admin" && !isAdmin) {
             res.set_content("<h1>Access Denied</h1><p>You are not an Administrator.</p><a href='/'>Go Back</a>", "text/html; charset=utf-8");
             return;
        }

        if (isAdmin) {
            std::string html = loadFile("admin_dashboard.html");
            replaceAll(html, "{{PASSENGER_LIST}}", "<tr><td colspan='2'>Search for a Train ID above</td></tr>");
            res.set_content(html, "text/html; charset=utf-8");
        } else {
            std::string html = loadFile("user_dashboard.html");
            replaceAll(html, "{{USERNAME}}", user);
            replaceAll(html, "{{USER_ID}}", std::to_string(userId));
            replaceAll(html, "{{HISTORY_TABLE}}", db_get_history(std::to_string(userId)));
            replaceAll(html, "{{TRAIN_LIST}}", db_get_all_trains());
            res.set_content(html, "text/html; charset=utf-8");
        }
    });

    svr.Post("/register", [](const httplib::Request& req, httplib::Response& res) {
        std::string user = req.get_param_value("username");
        std::string email = req.get_param_value("email");
        std::string pass = req.get_param_value("password");
        std::string role = req.get_param_value("role");
        std::string secret = req.get_param_value("secret_key");

        if (db_register_user(user, email, pass, role, secret)) {
            if (role == "admin") res.set_content("<h1>Admin Created!</h1><a href='/admin/login'>Login to Admin Portal</a>", "text/html; charset=utf-8");
            else res.set_content("<h1>User Created!</h1><a href='/user/login'>Login to User Portal</a>", "text/html; charset=utf-8");
        } else {
            res.set_content("<h1>Registration Failed!</h1><p>Username exists OR Invalid Secret Key.</p><a href='/'>Go Back</a>", "text/html; charset=utf-8");
        }
    });

    svr.Post("/book", [](const httplib::Request& req, httplib::Response& res) {
        if (db_book_ticket(req.get_param_value("user_id"), req.get_param_value("train_id"))) {
            res.set_content("<h1>Ticket Booked!</h1><p>Your seat is confirmed.</p><a href='/'>Logout</a>", "text/html; charset=utf-8");
        } else {
            res.set_content("<h1>Booking Failed</h1><h3 style='color:red'>Train is Full</h3><a href='/'>Go Back</a>", "text/html; charset=utf-8");
        }
    });

    svr.Post("/add_train", [](const httplib::Request& req, httplib::Response& res) {
        db_add_train(
            req.get_param_value("train_name"),
            req.get_param_value("source_id"),
            req.get_param_value("dest_id"),
            req.get_param_value("travel_date"),
            req.get_param_value("price"),
            req.get_param_value("seats")
        );
        std::string html = loadFile("admin_dashboard.html");
        replaceAll(html, "{{PASSENGER_LIST}}", "<tr><td colspan='2' style='color:green'><b>Success: Train Added!</b></td></tr>");
        res.set_content(html, "text/html; charset=utf-8");
    });

    svr.Post("/admin_search", [](const httplib::Request& req, httplib::Response& res) {
        std::string rows = db_get_passengers_by_train(req.get_param_value("train_id"));
        std::string html = loadFile("admin_dashboard.html");
        replaceAll(html, "{{PASSENGER_LIST}}", rows);
        res.set_content(html, "text/html; charset=utf-8");
    });

    // 4. Use Config Port
    std::cout << "Server started at http://localhost:" << SERVER_PORT << "\n";
    svr.listen("0.0.0.0", SERVER_PORT);
}