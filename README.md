# Chatter

A simple terminal-based chat server and client written in C, built from scratch to explore key systems programming concepts like sockets and I/O multiplexing. As well as some more generic concepts like creating a hashtable from scratch and handling the terminal in raw mode for the client UI.

---

## Table of Contents

1. [Overview](#overview)
2. [Features](#features)
3. [What I Learned](#what-i-learned)
4. [How It Works](#how-it-works)
5. [Usage](#usage)
6. [Building](#building)
7. [Known Limitations](#known-limitations)
8. [Next Steps](#next-steps)
9. [License](#license)

---

## Overview

**Chatter** is a toy project for learning and experimentation. It consists of a basic chat server and a terminal client that can connect, send messages, and receive broadcasts. It's written in C with no external libraries.

This project was created to explore low-level networking and terminal interaction from first principles.

---

## Features

- Simple chat server handling multiple clients
- Terminal client with raw mode input
- Uses `kqueue` for efficient I/O multiplexing (BSD/macOS)
- Custom-built hashtable for managing clients
- Handles new connections and message broadcasting
- Minimal dependencies — pure C

---

## What I Learned

Built primarily for educational purposes. Here’s what I explored:

- **Sockets API:**
  Built a TCP server/client from scratch using `socket()`, `bind()`, `accept()`, `connect()`, etc.

- **I/O Multiplexing:**
  Used `kqueue` (macOS/BSD) for scalable, event-driven handling of multiple clients.

- **Hashtable:**
  Implemented a basic hashmap structure from scratch to store and manage connected clients.

- **Terminal Raw Mode:**
  Manipulated terminal settings to capture keystrokes without line buffering or echoing.

---

## How It Works

### Server

- Listens on a given port
- Registers file descriptors using `kqueue`
- On client connect: adds to hashtable + event list
- On message: reads from client, broadcasts to all others

### Client

- Connects to server
- Switches terminal to raw mode
- Captures keystrokes, sends to server
- Displays messages from others in real time

---

## Usage

### 1. Start the server:

```bash
./build/bin/server <PORT NO>
```
### 2. Run the client in another terminal (repeat)

```bash
./build/bin/client <SERVER_HOSTNAME> <SERVER PORT NO>
```

### 3. Exchange messages

---

## Building

### Requirements:
- GCC or Clang
- macOS or BSD (for kqueue)
- Make

### Build:
```bash
make
```

This will compile both server and client binaries.

---

### Known Limitations

- No message framing — line-based messages only
- No authentication or usernames
- No input history or advanced editing
- No Windows/Linux support (uses kqueue)
- Client input and output are basic (no UI lib)

---

### Next Steps / Ideas

- Add epoll/poll support for Linux
- Implement message framing with size headers
- Add usernames and command handling
- Better UI with ncurses or tui lib
- Chat rooms / channels
- Write tests for the hashtable implementation

---

### License

Free to use as you please.
