# IronTrack API Documentation

## Authentication Routes
| Method | Endpoint        | Description |
|--------|----------------|-------------|
| GET    | `/`            | Landing Page |
| GET    | `/user/login`  | Passenger Login Form |
| POST   | `/login`       | Authenticates User/Admin |
| POST   | `/register`    | Creates new account (Requires Secret Key for Admin) |

## User Operations
| Method | Endpoint | Description |
|--------|----------|-------------|
| POST   | `/book`  | Books a ticket. Fails if seats >= 50. |

## Admin Operations
| Method | Endpoint        | Description |
|--------|----------------|-------------|
| POST   | `/add_train`   | Adds a new train to the schedule. |
| POST   | `/admin_search`| Returns list of passengers for a specific Train ID. |