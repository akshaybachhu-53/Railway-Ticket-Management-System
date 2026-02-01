INSERT INTO City (city_name)
VALUES
('Delhi'),
('Mumbai'),
('Chennai'),
('Bengaluru');

INSERT INTO Station (station_name, city_id)
VALUES
('NDLS', 1),     -- Delhi
('CSTM', 2),     -- Mumbai
('MAS', 3),      -- Chennai
('SBC', 4);      -- Bengaluru


INSERT INTO Train (train_name, total_seats)
VALUES
('Rajdhani Express', 500),
('Shatabdi Express', 300);

INSERT INTO TrainSchedule
(train_id, source_station_id, destination_station_id, journey_date, available_seats)
VALUES
(1, 1, 2, '2026-02-05', 500),  -- Delhi → Mumbai
(2, 2, 3, '2026-02-06', 300);  -- Mumbai → Chennai

INSERT INTO Users (name, phone)
VALUES
('Akshay', '9999999999'),
('Rahul', '8888888888');

SELECT * FROM City;
SELECT * FROM Station;
SELECT * FROM Train;
SELECT * FROM TrainSchedule;
SELECT * FROM Users;
