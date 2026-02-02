#include <windows.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <sql.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

// 1. INCLUDE CONFIGURATION (Load Connection String)
#include "../config/db_config.h"

// COMPILATION: g++ setup.cpp -o setup.exe -lodbc32

// --- Helper: Read File (With BOM Fix) ---
std::string readFile(std::string filename) {
    std::ifstream file(filename, std::ios::binary); 
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    // Remove BOM if present
    if (content.size() >= 3 && 
        (unsigned char)content[0] == 0xEF && 
        (unsigned char)content[1] == 0xBB && 
        (unsigned char)content[2] == 0xBF) {
        return content.substr(3);
    }
    return content;
}

// --- Helper: Split SQL by "GO" ---
std::vector<std::string> splitSqlByGo(std::string sql) {
    std::vector<std::string> commands;
    std::string delimiter = "GO";
    size_t pos = 0;
    while ((pos = sql.find(delimiter)) != std::string::npos) {
        std::string token = sql.substr(0, pos);
        if (token.length() > 2) commands.push_back(token);
        sql.erase(0, pos + delimiter.length());
    }
    if (sql.length() > 2) commands.push_back(sql);
    return commands;
}

// --- Helper: Execute SQL Block ---
void runSQL(std::string sql, SQLHDBC hDbc) {
    if (sql.empty()) return;
    size_t first = sql.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) return; 

    SQLHSTMT hStmt;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
    SQLRETURN ret = SQLExecDirect(hStmt, (SQLCHAR*)sql.c_str(), SQL_NTS);

    if (!SQL_SUCCEEDED(ret)) {
        SQLCHAR sqlState[6], msg[SQL_MAX_MESSAGE_LENGTH];
        SQLINTEGER nativeError;
        SQLSMALLINT textLen;
        SQLGetDiagRec(SQL_HANDLE_STMT, hStmt, 1, sqlState, &nativeError, msg, sizeof(msg), &textLen);
        if (std::string((char*)sqlState) != "01000") { 
            std::cerr << " [!] SQL Error: " << msg << std::endl;
        }
    }
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
}

int main() {
    std::cout << "--- Railway DB Setup Tool (Configured) ---\n";

    SQLHENV hEnv; SQLHDBC hDbc;
    SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
    SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
    SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);

    // USE CONN_STR FROM db_config.h
    SQLRETURN ret = SQLDriverConnect(hDbc, NULL, (SQLCHAR*)CONN_STR, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT);
    if (!SQL_SUCCEEDED(ret)) {
        std::cerr << "CRITICAL: Database Connection Failed!\nCheck config/db_config.h" << std::endl;
        return 1;
    }

    // 1. Run Schema
    std::cout << "1. Processing Schema...\n";
    std::string schemaSQL = readFile("../database/schema.sql");
    std::vector<std::string> schemaCmds = splitSqlByGo(schemaSQL);
    for (const auto& cmd : schemaCmds) runSQL(cmd, hDbc);
    std::cout << "   Schema Applied.\n";

    // 2. Run Sample Data
    std::cout << "2. Processing Sample Data...\n";
    std::string dataSQL = readFile("../database/sample_data.sql");
    std::vector<std::string> dataCmds = splitSqlByGo(dataSQL);
    for (const auto& cmd : dataCmds) runSQL(cmd, hDbc);
    std::cout << "   Data Inserted.\n";

    std::cout << "\nSUCCESS: Database Reset Complete!\n";
    
    SQLDisconnect(hDbc);
    SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
    SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
    
    std::cin.get(); 
    return 0;
}