# Mini Redis (C++)

A simplified Redis-like in-memory key-value store built from scratch using C++ to understand low-level systems, networking, and concurrency.

---

# 🚀 Features Implemented

## ✅ Networking

* TCP server using sockets (`socket`, `bind`, `listen`, `accept`)
* Multiple client connections supported

## ✅ Command Handling

* Basic commands:

  * `SET key value`
  * `GET key`
  * `DEL key`
  * `PING`

## ✅ Connection Handling

* Stream-based TCP handling
* Buffering to reconstruct full commands
* Handles partial reads and multiple commands

## ✅ Threading (Evolution)

### Phase 1 — Thread per client ❌

* One thread per connection
* Not scalable

### Phase 2 — Thread Pool ✅

* Fixed number of worker threads
* Task queue for incoming clients
* Efficient resource usage

## ✅ Thread-Safe Store

* Shared in-memory key-value store
* Protected using `std::mutex`
* Avoids race conditions

---

# 🧠 Architecture

```
Client
   ↓
Server (accept)
   ↓
ThreadPool (enqueue task)
   ↓
Worker Thread
   ↓
ClientSession
   ↓
CommandParser
   ↓
Store (thread-safe)
```

---

# ⚙️ Thread Pool Design

* Fixed worker threads (e.g., 4)
* Tasks stored as `std::function<void()>`
* `std::queue` used as task queue
* `std::condition_variable` used for efficient waiting
* No busy waiting

---

# 🔥 Key Learning Concepts

## Operating System Concepts

* Process vs Thread
* Blocking I/O
* File descriptors
* Kernel socket buffers

## Concurrency

* Race conditions
* Mutex and locking
* Thread safety
* Deadlocks (conceptual)
* Producer-consumer pattern

## Networking

* TCP as stream protocol
* Partial reads
* Connection buffering

## System Design

* Thread-per-client vs thread pool
* Task scheduling
* Resource management

---

# ⚠️ Current Limitation (Important)

⚠️ **Thread Pool + Long-Lived Connections Issue**

* Each worker thread handles a client in an infinite loop
* Threads never become free
* Additional clients get stuck in queue

👉 This highlights an important design lesson:

```
Thread pool works best with short-lived tasks
```

---

# 🛠 How to Run

```bash
mkdir build
cd build
cmake ..
make
./mini_redis
```

---

# 🔌 Connect to Server

```bash
nc localhost 6379
```

---

# 🧪 Example Commands

```
PING
SET name Akshay
GET name
DEL name
```

---

# 📌 Current Status

✔ Thread-safe store
✔ Thread pool implemented
✔ Multi-client support
⚠ Needs redesign of client handling for thread reuse

---

# 🚀 Upcoming Improvements

* Fix client handling (short-lived tasks)
* Implement request-based processing
* Add TTL (key expiration)
* Add persistence (RDB/AOF)
* Move to event-driven model (epoll)
* Performance benchmarking

---

# 💡 Why This Project Matters

This project demonstrates:

* Deep understanding of systems programming
* Hands-on experience with concurrency
* Real-world backend architecture evolution
* Ability to debug and improve designs

---

# 📚 Learning Outcome

This project is not just about building Redis-like functionality, but understanding:

> "How real backend systems are designed, scaled, and optimized"

---
