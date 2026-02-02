# 🚆 IronTrack Railway Management System

> A high-performance, full-stack web application engineered in **C++** without frameworks, demonstrating low-level systems programming, raw socket handling, and direct ODBC database interaction.

![C++](https://img.shields.io/badge/Language-C++-00599C?style=for-the-badge&logo=c%2B%2B)
![SQL Server](https://img.shields.io/badge/Database-SQL_Server-CC2927?style=for-the-badge&logo=microsoft-sql-server)
![HTML5](https://img.shields.io/badge/Frontend-HTML5_&_CSS3-E34F26?style=for-the-badge&logo=html5)

## 📖 Overview

IronTrack is a railway reservation system that bridges the gap between low-level backend logic and modern web interfaces. Unlike typical web apps built with high-level frameworks (like Django or Node.js), this project manages **HTTP requests, threading, and memory manually**.

**Key Technical Highlights:**
* **Custom Server-Side Rendering (SSR):** Implemented a custom C++ engine to parse HTML templates and inject dynamic data.
* **Raw Database Interaction:** Uses the **ODBC API** (`sqlext.h`) for direct communication with MS SQL Server, bypassing heavy ORMs for maximum control.
* **Concurrency Control:** Enforces strict ACID properties to prevent overbooking (Seat Locking Mechanism).
* **Role-Based Access Control (RBAC):** distinct, secure authentication flows for Passengers and Administrators.

---

## 🚀 Features

### 👤 User Portal
* **Real-time Availability:** View a dynamic table of all available trains with routes and pricing.
* **Instant Booking:** Book tickets by Train ID (auto-locks seat).
* **Travel History:** View past and upcoming trips with status updates.

### 👮 Admin Portal
* **Secure Login:** Requires a high-privilege Secret Key (`RAILWAY_MASTER`) for account creation.
* **Fleet Management:** Add new trains, set routes, prices, and schedules.
* **Passenger Manifests:** Search any Train ID to retrieve a list of all booked passengers.

---

## 🛠️ Tech Stack

* **Backend:** C++ (MinGW-w64), `httplib` (HTTP Server), `ws2_32` (Windows Sockets)
* **Database:** Microsoft SQL Server (Express Edition), ODBC Driver
* **Frontend:** HTML5, CSS3 (Flexbox/Grid), Server-Side Injection
* **Build Tools:** Custom Batch Scripts, G++ Compiler

---

## ⚙️ Project Structure

```text
IronTrack/
├── backend/            # Core C++ Logic
│   ├── main.cpp        # Server entry point, Routes, & Business Logic
│   └── setup.cpp       # Database Initialization Tool
├── config/
│   └── db_config.h     # Database Connection Strings & Constants
├── database/
│   ├── schema.sql      # Table definitions (Users, Trains, Bookings)
│   └── sample_data.sql # Initial dummy data
├── frontend/           # HTML Templates
│   ├── index.html      # Landing Page
│   ├── user_*.html     # User Login/Dashboard
│   └── admin_*.html    # Admin Login/Dashboard
├── build/
│   └── make_win.bat    # One-click build script
└── docs/               # Architecture Documentation