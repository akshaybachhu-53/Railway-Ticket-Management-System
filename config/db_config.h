#pragma once

// Database Connection Configuration
// Ideally, in a real production app, we would load these from an environment variable.
const char* CONN_STR = "DRIVER={SQL Server};SERVER=.\\SQLEXPRESS;DATABASE=RailwayDB;Trusted_Connection=yes;";

// App Settings
const int SERVER_PORT = 8080;
const int MAX_SEATS_PER_TRAIN = 50;