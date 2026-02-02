# Database Schema

## Tables

### 1. Users
- **UserID** (PK, Int)
- **Username** (String, Unique)
- **PasswordHash** (String)
- **IsAdmin** (Boolean)

### 2. Stations
- **StationID** (PK, Int)
- **StationName** (String) - e.g., "New York Penn"

### 3. Trains
- **TrainID** (PK, Int)
- **TrainName** (String)
- **SourceStationID** (FK -> Stations)
- **DestStationID** (FK -> Stations)
- **TotalSeats** (Int, Default: 50)

### 4. Bookings
- **BookingID** (PK, Int)
- **UserID** (FK -> Users)
- **TrainID** (FK -> Trains)
- **Status** (String) - e.g., "CONFIRMED"