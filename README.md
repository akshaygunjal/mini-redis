# Mini Redis (C++)

A simplified Redis-like in-memory key-value store built from scratch using C++.

## Features

- TCP server using sockets
- Multi-client support using threads
- Command parsing (SET, GET, DEL, PING)
- Thread-safe in-memory store using mutex
- Connection buffering and stream handling

## Architecture

Client → Server → ClientSession → CommandParser → Store

## Concepts Covered

- TCP networking (socket, bind, listen, accept)
- Blocking I/O
- Threading (`std::thread`)
- Race conditions & mutex
- System-level design

## How to Run

```bash
mkdir build
cd build
cmake ..
make
./mini_redis

Connect using:
nc localhost 6379

Example Commands
SET name Akshay
GET name
DEL name
PING

Future Improvements
- Thread pool
- Non-blocking I/O (epoll)
- Persistence (RDB/AOF)
- TTL support