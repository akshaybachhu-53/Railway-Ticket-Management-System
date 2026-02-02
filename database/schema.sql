IF NOT EXISTS (SELECT * FROM sys.databases WHERE name = 'RailwayDB')
BEGIN
    CREATE DATABASE RailwayDB;
END
GO

USE RailwayDB;
GO

DROP TABLE IF EXISTS Bookings; 
DROP TABLE IF EXISTS Trains; 
DROP TABLE IF EXISTS Stations;
DROP TABLE IF EXISTS Users; 
GO

-- Create Users Table
CREATE TABLE Users (
    UserID INT IDENTITY(1,1) PRIMARY KEY,
    Username VARCHAR(50) NOT NULL UNIQUE,
    Email VARCHAR(100) NOT NULL UNIQUE,
    PasswordHash VARCHAR(255) NOT NULL,
    IsAdmin BIT NOT NULL DEFAULT 0,
    CreatedAt DATETIME NOT NULL DEFAULT GETDATE()
);

-- Create Stations Table
CREATE TABLE Stations (
	StationID INT IDENTITY(1,1) PRIMARY KEY,
	StationName VARCHAR(50) NOT NULL UNIQUE
);

-- Create Trains Table
CREATE TABLE Trains (
	TRAINID INT PRIMARY KEY IDENTITY(1, 1),
	TrainName VARCHAR(100) NOT NULL,
	SourceStationID INT NOT NULL,
	DestStationID INT NOT NULL,
	TravelDate DATE NOT NULL,
	TotalSeats INT DEFAULT 50,
	Price DECIMAL(10, 2) NOT NULL,

	CONSTRAINT fk_trains_source
		FOREIGN KEY (SourceStationID) REFERENCES Stations(StationID),
	CONSTRAINT fk_trains_destination
		FOREIGN KEY (DestStationID) REFERENCES Stations(StationID),
	CONSTRAINT chk_diff_stations
		CHECK (SourceStationID <> DestStationID)
);


-- Create Bookings Table
CREATE TABLE Bookings (
	BookingID INT PRIMARY KEY IDENTITY(1, 1),
	UserID INT NOT NULL,
	TrainID INT NOT NULL,
	Booking DATETIME DEFAULT GETDATE(),
	Status VARCHAR(20) DEFAULT 'CONFIRMED',

	FOREIGN KEY (UserID) REFERENCES Users(UserID),
	FOREIGN KEY (TrainID) REFERENCES Trains(TrainID)
);
GO
