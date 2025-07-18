# DKVS — Distributed Key-Value Store

## Project Overview

This project implements a **Distributed Key-Value Store (DKVS)** in C, inspired by **Amazon Dynamo**. It is a simplified Distributed Hash Table (DHT) where data is distributed across multiple servers, offering **redundancy, fault tolerance, and scalability**.

The project centers around building a peer-to-peer data store, allowing clients to perform `put` and `get` operations over a network of interconnected servers.

---

## Minimal API

```c
put(key, value, N, W);
value = get(key, N, R);
```

* **key** / **value**: strings to associate and retrieve.
* **N**: number of servers to contact.
* **W**: number of successful writes required (write quorum).
* **R**: number of consistent reads required (read quorum).

---

## Features

* **Redundancy**: A value is stored on multiple servers.
* **Fault Tolerance**: System can tolerate server/network failures using configurable quorum values.
* **Consistent Hashing**: Keys are mapped across a ring structure using SHA-1.
* **Virtual Nodes**: A server may appear multiple times in the ring to improve balance and fault tolerance.
* **Parallelism**: The client sends all read/write requests. Responses from servers are multi-threaded.

---

## Key Concepts

* **Ring-based Distribution**: The key space is arranged in a ring using SHA-1 hashes. Servers (via virtual nodes) are placed in this ring to manage key ranges.
* **Nodes vs Servers**:

  * **Server**: A physical process/machine.
  * **Node**: An abstraction for a server’s position in the ring.
* **Quorum-based Consistency**:

  * `put()` is successful if at least **W** servers confirm the write.
  * `get()` is successful if at least **R** servers agree on the value.

---

## Example Workflow

Assume:

* `S = 5` servers
* `M = 9` nodes (ring positions)
* `N = 4`, `W = 3`, `R = 2`
* Server B is down
* `Hring("my_key") = 32`
* `Hlocal("my_key") = 10`

### Write Example

```c
put("my_key", "my_value", N=4, W=3);
```

* Contacts servers: B, C, A, D (B is down).
* C, A, and D succeed — quorum of W=3 reached → **write success**.

### Read Example

```c
get("my_key", N=4, R=2);
```

* Contacts same servers.
* C, A, and D respond with "my\_value".
* 3 matching values → **read success**.

---

### Instructions

1) cd ./done 
2) Compile into the **/done** directory with make
3) Create servers with "./dkvs-server <IPv4-Address> <Port> <Number-of-Nodes> {key value}..." (You may add key and value pair in the last field)
4) Launch client requests with "./dkvs-client <operation> [W] [R] [N] -- arguments"

### Examples
You could make use of a servers.txt with several servers and several nodes per server, for instance:

```
127.0.0.1 1234 1
127.0.0.1 1235 2
127.0.0.1 1236 3
```

and launch the corresponding servers, either by hand on several terminal, or using this command:

```
cut -d' ' -f 1-2 servers.txt | sort -u | while read line; do i=$(($i + 1)); ./dkvs-server $line >LOG$i.txt 2>&1 & done
```

Then populate a few, for instance:

```
./dkvs-client put -- a b
./dkvs-client put -- cc dd
./dkvs-client put -- cc dd
```




