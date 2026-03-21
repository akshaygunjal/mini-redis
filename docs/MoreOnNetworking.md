# 1️⃣ What Is TCP?

## 🧠 Technical Explanation

TCP (Transmission Control Protocol) is a communication protocol that:

- Is connection-oriented
- Guarantees delivery
- Preserves order
- Handles retransmission
- Detects errors

Before sending data, TCP performs a:
Three-way handshake (SYN → SYN-ACK → ACK)

This establishes a reliable connection.

## 👶 Explain Like I’m 5

Imagine you want to send your friend toy blocks.
You don’t just throw them.

You:

- Knock on the door
- Friend says “I’m ready!”
- You say “Okay, I’m sending!”

Now you pass blocks one by one.
If one falls, your friend says:
“That one fell. Send again.”

That’s TCP.

Reliable.
Ordered.
Safe.

---

# 2️⃣ What Is a Socket?

## 🧠 Technical Explanation

A socket is:
An endpoint for communication between two processes.

In Linux, a socket is just a file descriptor.

Everything in Linux is treated like a file:

- Files
- Pipes
- Devices
- Sockets

When we create a socket:

```cpp
int fd = socket(AF_INET, SOCK_STREAM, 0);
```

We get back an integer (file descriptor).
That integer represents a network connection.

## 👶 Explain Like I’m 5

Imagine two houses.
Each house has a special talking pipe.

That pipe is a socket.

You talk through the pipe.
Your friend hears through their pipe.

The pipe is the socket.

---

# 3️⃣ What Does `socket()` Do?

## 🧠 Technical Explanation

```cpp
socket(AF_INET, SOCK_STREAM, 0);
```

Creates a communication endpoint.

Parameters:

- AF_INET → IPv4
- SOCK_STREAM → TCP
- 0 → Default protocol

Returns:

- A file descriptor
- Or -1 if error

## 👶 Explain Like I’m 5

You go to a toy shop and say:
“Give me a talking pipe.”

The shop gives you one.

Now you have the pipe.
But it’s not connected yet.

---

# 4️⃣ What Does `bind()` Do?

## 🧠 Technical Explanation

Bind attaches the socket to a:

- Specific IP address
- Specific port number

Example:

```cpp
bind(server_fd, ...);
```

This means:
“I want to own port 6379.”

Without bind:

- Clients won’t know where to connect
- The OS may assign a random port

## 👶 Explain Like I’m 5

Imagine your house has no house number.
How will your friend find you?

Bind is like putting a house number outside:
House #6379

Now people know where to knock.

---

# 5️⃣ What Does `listen()` Do?

## 🧠 Technical Explanation

```cpp
listen(server_fd, 10);
```

Tells the OS:
“I am ready to accept connections.”

Backlog (10) means:
How many connection requests can wait in queue.

The socket now enters LISTEN state.

## 👶 Explain Like I’m 5

You sit near your door and say:
“I am ready for visitors.”

If 10 friends come, they wait in line.

That waiting line is the backlog.

---

# 6️⃣ What Does `accept()` Do?

## 🧠 Technical Explanation

```cpp
int client_fd = accept(server_fd, ...);
```

- Blocks until a client connects
- Returns a NEW socket for that client

Important:
You now have:

- server_fd → still listening
- client_fd → talking to that specific client

This allows multiple clients.

## 👶 Explain Like I’m 5

You are sitting at your door.

When a friend knocks:
You open a private talking pipe just for that friend.

But you keep your door open for others.

Door = server_fd  
Private pipe = client_fd

---

# 7️⃣ What Is Blocking?

## 🧠 Technical Explanation

A blocking call pauses execution until something happens.

Example:

```cpp
accept()
```

If no client connects:
Program waits.

It does nothing.

Later we will learn:

- Non-blocking
- epoll
- async models

## 👶 Explain Like I’m 5

You are waiting at the door.

If nobody knocks,
you just stand there.

You don’t do anything else.

That’s blocking.

---

# 8️⃣ Full TCP Server Lifecycle

## 🧠 Technical Flow

```text
socket()
bind()
listen()
accept()
read()
write()
close()
```

## 👶 Story Version

- Buy a talking pipe
- Put house number outside
- Say “I’m ready!”
- Wait for knock
- Talk
- Say goodbye

That’s a server.

---

# 9️⃣ Why Does `accept()` Return a New Socket?

Because:

The main server must keep listening.

If it reused the same socket:
It couldn’t accept more clients.

So:

```text
server_fd  → always listening
client_fd1 → client 1
client_fd2 → client 2
client_fd3 → client 3
```

This is how multi-client servers work.

---

# 🔥 What You Now Understand

You now understand:

- TCP basics
- Sockets
- Ports
- Blocking
- Server lifecycle
- File descriptors
- Multi-client architecture foundation
