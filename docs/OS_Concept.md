# 🧠 OS & Systems Concepts — Mini Redis Project

This document captures all Operating System and Systems Programming concepts learned while building a multi-client TCP server in C++.

---

# 1. Process vs Thread

## Process

A process is a running program with:

* Its own memory space
* File descriptors
* OS-managed resources

Example:

```
./mini_redis → creates a process
```

## Thread

A thread is a unit of execution inside a process.

Threads:

* Share memory of the process
* Have independent execution flow (stack, registers)

Example:

```
Process (mini_redis)
   ├── Main Thread (server loop)
   ├── Thread1 (client1)
   ├── Thread2 (client2)
```

---

# 2. TCP Server Lifecycle

A TCP server follows:

```
socket()
bind()
listen()
accept()
read()
write()
close()
```

## Explanation

* `socket()` → Create endpoint
* `bind()` → Attach to IP + port
* `listen()` → Start listening
* `accept()` → Accept client connection
* `read()` → Receive data
* `write()` → Send response
* `close()` → Close connection

---

# 3. File Descriptors

In Linux:

* Everything is treated as a file
* Sockets are also file descriptors

Standard FDs:

```
0 → stdin
1 → stdout
2 → stderr
3+ → sockets/files
```

---

# 4. Blocking I/O

Functions like:

```
accept()
read()
```

are **blocking calls**.

Meaning:

* They pause execution until something happens

Example:

```
read() → waits until data arrives
```

---

# 5. TCP is a Stream Protocol

TCP does NOT preserve message boundaries.

Example:

Client sends:

```
SET name Akshay
```

Server might receive:

```
SET na
me Ak
shay
```

So we must:

* Buffer incoming data
* Reconstruct full messages

---

# 6. Connection Buffer

Each client maintains:

```
connection_buffer
```

Flow:

```
read() → append → parse → process → erase
```

Used to:

* Handle partial reads
* Handle multiple commands in one read

---

# 7. Partial Reads

`read()` may return less data than requested.

Example:

```
Client sends 2000 bytes
Buffer size = 1024

First read → 1024 bytes
Second read → 976 bytes
```

Remaining data stays in kernel buffer.

---

# 8. Kernel Socket Buffers

Data flow:

```
Client
   ↓
Network
   ↓
Kernel socket buffer
   ↓
read()
   ↓
User buffer
```

For sending:

```
write()
   ↓
Kernel send buffer
   ↓
Network
   ↓
Client
```

---

# 9. Multi-Client Handling (Threads)

Initial design:

```
accept → handle → accept
```

Problem:

* Only one client at a time

Improved design:

```
accept → create thread → handle client
```

Now:

```
Client1 → Thread1
Client2 → Thread2
```

---

# 10. std::thread

Creating a thread:

```cpp
std::thread t(function, args);
```

## detach()

```cpp
t.detach();
```

* Thread runs independently
* Main thread continues execution
* No need to join

Without `detach()` or `join()` → program crashes (`std::terminate`)

---

# 11. listen() Backlog

```cpp
listen(fd, 5);
```

Meaning:

* Up to ~5 pending connections in queue

Important:

* Does NOT limit total clients
* Only controls waiting queue

---

# 12. Race Condition

Occurs when:

```
multiple threads
access shared data
at least one modifies it
```

Example:

```
Thread1 → set("name", "Akshay")
Thread2 → set("name", "Rahul")
```

May lead to:

* Data corruption
* Crashes

---

# 13. Shared Memory Problem

All threads share:

```
static Store store;
```

So:

```
Thread1 → modify store
Thread2 → modify store
```

Needs synchronization.

---

# 14. Mutex (Mutual Exclusion)

Ensures:

```
Only ONE thread accesses critical section at a time
```

Usage:

```cpp
std::lock_guard<std::mutex> lock(mtx);
```

---

# 15. Critical Section

Code accessing shared data:

```cpp
data[key] = value;
```

Must be protected.

---

# 16. lock_guard (RAII)

```cpp
std::lock_guard<std::mutex> lock(mtx);
```

Automatically:

* Locks on creation
* Unlocks when scope ends

Prevents:

* Forgetting unlock
* Bugs

---

# 17. Deadlock (Basic Case)

If mutex is not unlocked:

```
Thread1 → lock → never unlock
Thread2 → waits forever
Thread3 → waits forever
```

System appears frozen.

---

# 18. Thread Safety

A system is thread-safe if:

```
multiple threads
can safely access shared data
without corruption
```

Achieved using:

* Mutex
* Locks

---

# 19. Encapsulation Principle

The component owning data should protect it.

Good design:

```
Store
 ├── data
 └── mutex
```

Bad design:

```
Mutex outside store → unsafe usage
```

---

# 20. Limitation of Current Design

Current model:

```
thread per client
```

Problems:

* Too many threads
* High memory usage
* Context switching overhead

Better designs:

* Thread pool
* Event-driven (epoll)

---

# 21. Key Takeaways

* Threads share memory → powerful but dangerous
* Mutex ensures safety → but reduces parallelism
* TCP is stream-based → requires buffering
* Server design evolves step by step
* Clean architecture improves scalability

---

# 🚀 Future Concepts

* Thread pools
* Reader-writer locks
* epoll (non-blocking I/O)
* Event-driven servers
* Performance tuning

---

# 📌 Final Note

This project demonstrates:

* Networking fundamentals
* OS-level concepts
* Concurrency handling
* Real-world backend design

This is already a strong **systems-level project for your resume**.
