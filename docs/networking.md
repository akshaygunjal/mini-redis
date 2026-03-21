# Networking Basics

## What is a Socket?

A socket is an endpoint for communication between two machines.

In TCP:
Client  <---->  Server

The server:
- Opens a port
- Listens for connections
- Accepts client connections

We will implement this in server.cpp.

# Networking Basics

## What is TCP?

TCP (Transmission Control Protocol) is:
- Connection oriented
- Reliable
- Ordered
- Error-checked

It guarantees:
- No packet loss (retransmits)
- Order is preserved
- No duplication

We will use TCP for our server.

---

## What is a Socket?

A socket is a file descriptor that represents a network connection.

In Linux:
Everything is a file descriptor:
- Files
- Pipes
- Sockets

## TCP Server Lifecycle

1. socket()        → create server_fd
2. bind()          → attach to port
3. listen()        → start listening
4. accept()        → returns client_fd
5. read/write()    → communicate
6. close()         → close connection

accept() is blocking.

Server socket continues listening.
Each client gets a new socket.

## Observations From Minimal Server

- socket() returns a file descriptor
- bind() attaches socket to port
- listen() changes state to LISTEN
- accept() blocks until connection
- accept() returns NEW socket for client
- server_fd continues listening

## TCP Is a Stream Protocol

TCP does not preserve message boundaries.

A single command may arrive in multiple reads:

Example:

SET name Akshay

Could arrive as:

SE
T name A
kshay

Servers must buffer incoming data and reconstruct full commands.

We will use newline-delimited commands:

SET name Akshay\n
GET name\n

Server reads until newline.

## read() Return Values

read() returns the number of bytes read.

> 0  → client closed connection (EOF)
> >0 → data received
> -1 → error

When read() returns 0, the server must close the client socket.


## Why Servers Use Buffers

Servers read data using buffers like:

char buffer[1024];

Reasons:

1. System calls are expensive
2. Network data arrives in chunks
3. Parsing multiple commands becomes easier

TCP data is first stored in the kernel socket buffer.

read() copies data from kernel space into user space.

## Partial Reads

read() may return fewer bytes than requested.

Example:

Client sends 2000 bytes
Buffer size = 1024

First read → 1024 bytes  
Second read → 976 bytes

Remaining data stays in the kernel socket buffer.

## Message Framing

TCP is a stream protocol and does not preserve message boundaries.

Example:

Client sends:
SET name Akshay\nGET name\n

Server may receive both commands in one read().

Servers maintain a connection buffer and parse commands using delimiters.

Our protocol uses newline '\n' to separate commands.

Experiment: Observing TCP stream

The server reads raw bytes using read().

Multiple commands may arrive in a single read().
Commands may also arrive in fragments.

This confirms that TCP is a stream protocol.

write()

write(fd, buffer, size)

Used to send data through a socket.

Parameters:
fd     → socket file descriptor
buffer → data to send
size   → number of bytes

write() copies data into the kernel send buffer, and the TCP stack transmits it to the client.