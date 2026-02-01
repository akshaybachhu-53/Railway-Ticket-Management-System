CREATE DATABASE RailwayDB;
GO
USE RailwayDB;
GO

CREATE TABLE City (
	city_id INT IDENTITY(1, 1) PRIMARY KEY,
	city_name VARCHAR(50) NOT NULL UNIQUE
);


CREATE TABLE Station (
    station_id INT IDENTITY(5001,1) PRIMARY KEY,
    station_name VARCHAR(50) NOT NULL,
    city_id INT NOT NULL,
    station_code VARCHAR(10) UNIQUE,

    CONSTRAINT fk_station_city
        FOREIGN KEY (city_id)
        REFERENCES City(city_id)
);


CREATE TABLE Train (
	train_id INT IDENTITY(1,1) PRIMARY KEY,
	train_name VARCHAR(100) NOT NULL,
	total_seats INT NOT NULL CHECK (total_seats > 0)
);

CREATE TABLE TrainSchedule (
	schedule_id INT IDENTITY(1,1) PRIMARY KEY,
	train_id INT NOT NULL,
	source_station_id INT NOT NULL,
	destination_station_id INT NOT NULL,
	journey_date DATE NOT NULL,
	available_seats INT NOT NULL,

	CONSTRAINT fk_schedule_train
		FOREIGN KEY (train_id)
		REFERENCES Train(train_id),

	CONSTRAINT fk_schedule_source
        FOREIGN KEY (source_station_id)
        REFERENCES Station(station_id),

    CONSTRAINT fk_schedule_destination
        FOREIGN KEY (destination_station_id)
        REFERENCES Station(station_id),

    CONSTRAINT chk_different_stations
        CHECK (source_station_id <> destination_station_id)
);

CREATE TABLE Users (
	user_id INT IDENTITY(1,1) PRIMARY KEY,
	name VARCHAR(100) NOT NULL,
	phone VARCHAR(15) UNIQUE
);

CREATE TABLE Ticket (
	ticket_id INT identity(1,1) PRIMARY KEY,
	user_id INT NOT NULL,
	schedule_id INT NOT NULL,
	seats_booked INT NOT NULL CHECK (seats_booked > 0),
	booking_status VARCHAR(20) DEFAULT 'CONFIRMED',

	CONSTRAINT fk_ticket_user
        FOREIGN KEY (user_id)
        REFERENCES Users(user_id),

    CONSTRAINT fk_ticket_schedule
        FOREIGN KEY (schedule_id)
        REFERENCES TrainSchedule(schedule_id)
);
SELECT TABLE_NAME
FROM INFORMATION_SCHEMA.TABLES
WHERE TABLE_TYPE = 'BASE TABLE';

SELECT name
FROM sys.tables;