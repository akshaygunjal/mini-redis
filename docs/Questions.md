## 1️⃣ What happens if two clients connect at the same time?

**Your answer:**

> whoever is first in the queue will get connected

✔ Almost correct.

### What actually happens

When multiple clients connect:

- TCP handshake completes with the kernel
- The kernel puts those connections in the accept queue

```text
Client1 ──┐
Client2 ──┼──> Kernel Accept Queue
Client3 ──┘
```

When your program calls:

```cpp
accept(server_fd, ...)
```

The kernel:

- Pops one connection from the queue
- Returns a `client_fd`

Your server then handles it.

### Important detail

Your server is single-threaded right now, so it handles connections one at a time.

If 3 clients connect:

```text
Queue:
Client1
Client2
Client3
```

Your server will accept them sequentially.

Later we will improve this using:

- threads
- event loops

### 👶 Analogy

Imagine a ticket counter.

People line up:

```text
Person1
Person2
Person3
```

The clerk calls:

> "Next!"

That is `accept()`.

---

## 2️⃣ What happens if we remove `listen()`?

**Your answer:**

> Server will never be able to connect with the client

✔ Correct idea, but let's be precise.

If you call:

```cpp
socket()
bind()
accept()
```

Without `listen()`:

`accept()` will fail.

The kernel will return an error like:

```text
EINVAL
```

Because the socket was never put into `LISTEN` state.

So the correct lifecycle is mandatory:

```text
socket → bind → listen → accept
```

### 👶 Analogy

Imagine opening a shop.

You:

- Rent building (`socket`)
- Put address board (`bind`)
- Unlock door and say "Open!" (`listen`)

If you skip step 3, customers cannot enter.

---

## 3️⃣ What happens if port `6379` is already used?

**Your answer:**

> It will return an error

✔ 100% correct.

When you run:

```cpp
bind(server_fd, ...)
```

If another process already owns the port:

The kernel returns:

```text
EADDRINUSE
```

Your program will print:

```text
Bind failed!
```

You can verify this with:

```bash
ss -ltn
```

This command shows all listening ports.

### 👶 Analogy

Imagine house numbers.

Only one house can be number `6379` on a street.

If someone already lives there:

You cannot claim it.

---

## 4️⃣ Why do we use `htons()`?

**Your answer:**

> it converts port number into byte format

✔ Correct direction.

Let’s make it precise.

### Problem

Different CPUs store bytes differently:

Two major formats exist:

| Format | Used by |
| --- | --- |
| Little-endian | x86 (your PC) |
| Big-endian | Network standard |

Example port:

```text
6379 = 0x18EB
```

Memory representation differs.

Network protocols require big-endian.

So we convert:

```text
Host byte order → Network byte order
```

Using:

```cpp
htons()
```

Meaning:

```text
Host TO Network Short
```

### Example

```cpp
address.sin_port = htons(6379);
```

Without this conversion:

The port number may be interpreted incorrectly.

### 👶 Analogy

Imagine sending numbers to another country.

You write:

```text
18 | 79
```

But they read:

```text
79 | 18
```

So before sending, you rearrange it properly.

That’s what `htons()` does.

### 🧠 Extra Knowledge (Good for Interviews)

There are 4 similar functions:

| Function | Meaning |
| --- | --- |
| `htons()` | Host → Network Short |
| `htonl()` | Host → Network Long |
| `ntohs()` | Network → Host Short |
| `ntohl()` | Network → Host Long |

---

## 5️⃣ How should the server handle this?

```text
A) Assume full command arrives in one read
B) Buffer and reconstruct message
C) Read until newline
```

the actual strategy most servers use is:

Buffer the stream and parse messages using a delimiter or protocol format.

Let’s break this down carefully.

### 🧠 The Core Problem: TCP Is a Stream

TCP does not preserve message boundaries.

If a client sends:

```text
SET name Akshay
```

TCP may deliver it like:

```text
SE
T name A
kshay
```

or

```text
SET name AkshaySET age 25
```

or even:

```text
SET name Akshay
```

All three are possible.

This is because TCP sends a continuous byte stream, not messages.

### 👶 5-Year-Old Analogy

Imagine a water pipe.

You pour words into the pipe:

```text
SET name Akshay
```

But the other side receives water chunks, not words.

Maybe they receive:

```text
SE
T name A
kshay
```

So the receiver must collect water until a full sentence appears.

### 🧠 How Real Servers Solve This

Servers define message boundaries.

Common approaches:

#### 1️⃣ Newline delimiter

```text
SET name Akshay\n
GET name\n
```

Server reads until `\n`.

Very simple.

#### 2️⃣ Length-prefixed protocol

Example:

```text
16:SET name Akshay
```

Server reads:

```text
first 16 bytes
```

then processes command.

#### 3️⃣ Structured protocol

Real Redis uses something called:

`RESP (Redis Serialization Protocol)`

Example:

```text
*3
$3
SET
$4
name
$6
Akshay
```

But we will not implement that yet.

### 🧠 What We Will Implement (For Learning)

We will use the newline protocol.

Client sends:

```text
SET name Akshay\n
GET name\n
DEL name\n
```

Server logic:

```text
read()
append to buffer
if newline found:
    process command
```

### 🧠 Visual Flow

Client sends

```text
SET name Akshay\nGET name\n
```

Server buffer becomes:

```text
SET name Akshay\nGET name\n
```

Parser extracts:

```text
Command1 → SET name Akshay
Command2 → GET name
```

### 🧠 Why This Matters

Without buffering:

Your server might read:

```text
SET name A
```

And treat it as a command.

Which is wrong.

So every real server maintains:

```text
connection_buffer
```

### 🧠 Architecture We'll Use

Each client connection will have:

```text
client_fd
read_buffer
command_parser
```

Flow:

```text
Client
   ↓
read()
   ↓
append to buffer
   ↓
extract full command
   ↓
execute
```

Before we code, one last thinking question:

If the client disconnects,

read() returns:

```text
0
```

Why do you think that happens?

## 6️⃣ What Does `read()` Return?

When you call:

```cpp
read(client_fd, buffer, size);
```

read() returns the number of bytes read.

| Return Value | Meaning |
| --- | --- |
| `> 0` | Bytes were successfully read |
| `0` | Connection closed by peer |
| `-1` | Error occurred |

### ❗ So Why Does `read()` Return `0` When Client Disconnects?

Because in TCP, when a client closes the connection, it sends a FIN packet.

The kernel interprets that as:

> “The client will send no more data.”

So when your program calls read() again:

The kernel says:

> There is no more data ever coming.

And it returns:

```text
0
```

This is called EOF (End of File).

Even though it's a socket, Linux treats it like a file stream.

### 👶 5-Year-Old Analogy

Imagine you are listening to a friend on a walkie-talkie.

You keep saying:

> "Say something!"

If your friend turns off their walkie-talkie, you hear silence forever.

That silence is like `read() == 0`.

It means:

> The other side has closed the connection.

### 🧠 Important Rule in Servers

When you see:

```text
bytes_read == 0
```

You must:

```cpp
close(client_fd)
```

Because the connection is finished.

Otherwise your server keeps a dead connection open.

### 🧠 Real Server Pattern

Almost every TCP server loop looks like this:

```cpp
while (true) {
    int bytes = read(client_fd, buffer, sizeof(buffer));

    if (bytes > 0) {
        // process data
    }

    else if (bytes == 0) {
        // client disconnected
        close(client_fd);
        break;
    }

    else {
        // error
    }
}
```

### 🧠 Visual Timeline

Client connects:

```cpp
accept()
```

Client sends data:

```text
read() → returns bytes
```

Client closes connection:

```text
read() → returns 0
```

Server closes socket:

```cpp
close(client_fd)
```

---

## 7️⃣ Why do we usually use a buffer like this?

```cpp
char buffer[1024];
```

Why not read 1 byte at a time?

### 🧠 Why Do We Use a Buffer (like `char buffer[1024]`)?

Example:

```cpp
char buffer[1024];
read(client_fd, buffer, sizeof(buffer));
```

Instead of:

```cpp
read(client_fd, &c, 1);
```

There are three major reasons.

#### 1️⃣ System Calls Are Expensive

`read()` is a system call.

System calls switch from:

```text
User space → Kernel space → User space
```

That context switch is slow compared to normal CPU operations.

If you read 1 byte at a time, you would do:

```text
read()
read()
read()
read()
read()
```

Maybe 1000+ system calls for a small message.

Instead we read a chunk:

```text
read(1024 bytes)
```

Much faster.

### 👶 5-Year-Old Analogy

Imagine carrying sand.

You could move sand:

```text
1 grain at a time 😅
```

or

```text
1 bucket at a time 🪣
```

A bucket is faster.

The buffer is the bucket.

#### 2️⃣ Network Data Arrives in Chunks

TCP sends data in packets.

Typical packet sizes:

```text
~1500 bytes (Ethernet MTU)
```

So the kernel already has chunks of data ready.

If you read 1 byte at a time, you're wasting that efficiency.

Reading with a buffer allows you to grab all available data.

#### 3️⃣ Simpler Parsing

Suppose client sends:

```text
SET name Akshay\n
```

Buffer may receive:

```text
SET name Akshay\nGET age\n
```

Now your server can parse multiple commands from one read.

This is very common in real servers.

### 🧠 What Actually Happens Inside the Kernel

When TCP packets arrive:

```text
Network card
      ↓
Kernel TCP stack
      ↓
Kernel socket buffer
      ↓
read()
```

Your program reads from the kernel socket buffer.

```text
[Kernel Socket Buffer]

SET name Akshay\nGET age\n
```

Your read() just copies bytes from that buffer.

### 🧠 Why 1024?

It's just a convenient size.

Common buffer sizes:

```text
512
1024
4096
8192
```

Later we'll probably use:

```text
4096
```

because it matches memory page size on Linux.

### 📘 Add This to `docs/networking.md`

```md
## Why Servers Use Buffers

Servers read data using buffers like:

char buffer[1024];

Reasons:

1. System calls are expensive
2. Network data arrives in chunks
3. Parsing multiple commands becomes easier

TCP data is first stored in the kernel socket buffer.

read() copies data from kernel space into user space.
```

### 🧠 Important Mental Model

Right now the data path is:

```text
Client
   ↓
Network
   ↓
Kernel TCP buffer
   ↓
read()
   ↓
User buffer (char buffer[1024])
```

---

## 8️⃣ What do you think happens?

```text
A) Server crashes
B) read() returns only 1024 bytes
C) Kernel truncates the message
D) Remaining data is lost
```

`read()` returns only the amount that fits in your buffer, and the remaining data stays in the kernel socket buffer.

Nothing is lost.

Let’s break this down carefully.

### 🧠 Scenario

Client sends:

```text
2000 bytes
```

Your server buffer:

```cpp
char buffer[1024];
```

Your call:

```cpp
read(client_fd, buffer, 1024);
```

### 🧠 What the Kernel Does

Remember the pipeline:

```text
Client
   ↓
Network
   ↓
Kernel Socket Buffer
   ↓
read()
   ↓
User Buffer
```

When the 2000 bytes arrive:

```text
Kernel Socket Buffer

[2000 bytes]
```

Your program asks for 1024 bytes.

So the kernel copies:

```text
1024 bytes → your buffer
```

Now the kernel buffer still contains:

```text
Remaining 976 bytes
```

Those bytes stay there until you call `read()` again.

### 🧠 Timeline Example

Client sends

```text
2000 bytes
```

First read

```cpp
read(client_fd, buffer, 1024);
```

returns:

```text
1024
```

Kernel buffer now:

```text
976 bytes remaining
```

Second read

```cpp
read(client_fd, buffer, 1024);
```

returns:

```text
976
```

Kernel buffer now:

```text
empty
```

### 👶 5-Year-Old Analogy

Imagine a mailbox.

Your friend sends 2000 letters.

But your bag can only carry 1024 letters.

So you:

1️⃣ Take 1024 letters
2️⃣ Go back again
3️⃣ Take the remaining 976

The mailbox keeps the rest safely.

Nothing disappears.

### 🧠 Important Concept

This is why servers read in a loop.

Typical pattern:

```cpp
while (true) {
    int bytes = read(client_fd, buffer, sizeof(buffer));

    if (bytes > 0) {
        process(buffer, bytes);
    }
}
```

You keep reading until the command is complete.

`read()` can also return less data even if more is available.

Example:

You request:

```text
1024 bytes
```

But the kernel may return:

```text
512 bytes
```

because that's what currently arrived.

So network code must always handle partial reads.

---

## 9️⃣ Suppose client sends:

```text
SET name Akshay\nGET name\n
```

Both commands in one packet.

Your `read()` receives:

```text
SET name Akshay\nGET name\n
```

How should the server process this?

```text
A) Execute only first command
B) Execute both commands
C) Treat it as invalid command
D) Wait for next read()
```

The server should:

Execute both commands.

But only after the parser extracts them from the buffer.

### 🧠 Why This Happens

TCP is a stream protocol, not a message protocol.

So the network might deliver:

```text
SET name Akshay\nGET name\n
```

in one `read()`.

Or:

```text
SET name Akshay\n
```

then

```text
GET name\n
```

Both are valid.

Your server must handle both cases.

### 🧠 Correct Architecture

The server does three stages:

1. Read bytes
2. Append to connection buffer
3. Parser extracts complete commands

Flow:

```text
TCP Stream
     ↓
read()
     ↓
connection buffer
     ↓
command parser
     ↓
execute command
```

### 🧠 Example

Client sends:

```text
SET name Akshay\nGET name\n
```

Your `read()` receives:

```text
SET name Akshay\nGET name\n
```

Your connection buffer becomes:

```text
SET name Akshay\nGET name\n
```

Now the parser extracts:

```text
Command 1 → SET name Akshay
Command 2 → GET name
```

Both get executed.

### 👶 5-Year-Old Analogy

Imagine someone hands you a piece of paper that says:

```text
Buy milk
Buy eggs
```

Two instructions on the same paper.

You don’t ignore the second line.

You read each line separately and do both.

Your parser is the person reading the lines.

### 🧠 What Happens If Message Is Incomplete?

Example TCP delivery:

```text
SET name Ak
```

Parser sees:

```text
No newline yet
```

So it waits.

Next read:

```text
shay\n
```

Buffer becomes:

```text
SET name Akshay\n
```

Now parser executes command.

### 🧠 This Is Called

```text
Message Framing
```

Because TCP doesn’t give us message boundaries.

We create them using:

```text
delimiter (\n)
```

### 📘 Add This to `docs/networking.md`

```md
## Message Framing

TCP is a stream protocol and does not preserve message boundaries.

Example:

Client sends:
SET name Akshay\nGET name\n

Server may receive both commands in one read().

Servers maintain a connection buffer and parse commands using delimiters.

Our protocol uses newline '\n' to separate commands.
```

### 🧠 Final Mental Model

Every client connection will eventually have:

```text
client_fd
read_buffer
command_parser
```

Pipeline:

```text
Client
   ↓
TCP stream
   ↓
read()
   ↓
connection buffer
   ↓
parser
   ↓
command execution
```

This design is used by:

```text
Redis
Nginx
PostgreSQL
```

All servers follow this same pattern.

When the client sends:

SET name Akshay\nGET name\n

Should our server respond once or twice?

If the client sends:

SET name Akshay\nGET name\n

The server should respond twice, because there are two complete commands separated by \n.

Example interaction:

Client sends:

SET name Akshay
GET name

Server responds:

OK
Akshay

Two commands → two responses.

This is how many text-based protocols behave (including early versions of Redis-like protocols).

Command Parsing Design

TCP is a stream protocol, so servers must buffer incoming data.

A connection buffer accumulates data from read() calls.

Commands are extracted using newline '\n' as a delimiter.

Each complete command is parsed and executed.
