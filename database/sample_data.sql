USE RailwayDB;
GO

-- 1. Insert Stations
-- ID 1 will be 'New York', ID 2 'Boston', ID 3 'Washington DC'
INSERT INTO Stations (StationName) VALUES 
('New York Penn'), 
('Boston South Station'), 
('Washington Union Station'),
('Philadelphia 30th St');
GO

-- 2. Insert Users
-- One Admin, Two Passengers
INSERT INTO Users (Username, Email, PasswordHash, IsAdmin) VALUES 
('admin_user', 'admin@railway.com', 'admin123', 1),   -- UserID 1
('john_doe', 'john@gmail.com', 'pass123', 0),        -- UserID 2
('alice_smith', 'alice@yahoo.com', 'secure456', 0);  -- UserID 3
GO

-- 3. Insert Trains
-- NOTICE: We use the IDs 1, 2, 3 from the Stations table above.
INSERT INTO Trains (TrainName, SourceStationID, DestStationID, TravelDate, Price, TotalSeats) VALUES 
('Acela Express 2150', 1, 2, '2023-12-01', 120.50, 50),  -- NY (1) to Boston (2)
('Northeast Regional', 2, 3, '2023-12-05', 85.00, 100),  -- Boston (2) to DC (3)
('Keystone Service', 4, 1, '2023-12-06', 45.00, 60),     -- Philly (4) to NY (1)
('Night Owl', 3, 2, '2023-12-07', 90.00, 40);            -- DC (3) to Boston (2)
GO

-- 4. Insert Bookings
-- John (User 2) books Acela (Train 1)
-- Alice (User 3) books Northeast Regional (Train 2)
INSERT INTO Bookings (UserID, TrainID, Status) VALUES 
(2, 1, 'CONFIRMED'),
(3, 2, 'WAITLIST');
GO

SELECT 
    T.TrainName, 
    S1.StationName AS FromStation, 
    S2.StationName AS ToStation, 
    T.Price
FROM Trains T
JOIN Stations S1 ON T.SourceStationID = S1.StationID
JOIN Stations S2 ON T.DestStationID = S2.StationID;

SELECT * FROM Trains;
SELECT * FROM Stations;
SELECT * FROM Users;
SELECT * FROM Bookings;
