# C Binary Packet Serializer

A simple, efficient system for packing multiple C structures into a single contiguous memory buffer. This pattern is commonly used in **network programming**, **binary file formats**, and **embedded systems** to minimize overhead and ensure data integrity during transmission.

## 🚀 Features

* **Magic Number Validation:** Uses a unique identifier (`0xCAFEBABE`) to verify packet integrity.
* **Version Control:** Includes a version field to ensure compatibility between sender and receiver.
* **Dynamic Payload:** Supports a variable number of `Order` entries in a single packet.
* **Zero-Copy Logic:** Uses pointer arithmetic and casting to navigate memory efficiently without unnecessary data duplication.

---

## 📦 Packet Structure

The data is organized in memory as a single contiguous block. The header tells the parser how to interpret the bytes that follow.

| Offset | Field | Type | Description |
| --- | --- | --- | --- |
| 0 | `magic` | `uint32_t` | Identifies the protocol (0xCAFEBABE). |
| 4 | `version` | `uint16_t` | Protocol version number. |
| 6 | `count` | `uint16_t` | Number of Order entries in the payload. |
| 8+ | `Payload` | `Order[]` | Array of `Order` structs (ID, Price, Quantity). |

---

## 🛠️ How It Works

### 1. Serialization (`serialize_orders`)

This function acts as the "Encoder." It calculates the total memory required for the header and the orders. It then performs a **pointer cast** on the raw buffer:

* It writes the `PacketHeader` at the very beginning.
* It offsets the pointer by `sizeof(PacketHeader)` to find the starting address for the data.
* It loops through the input array and copies each `Order` into the buffer.

### 2. Parsing (`parse_packet`)

This function acts as the "Decoder." It treats the incoming `char*` buffer as a structured object:

* It first reads the header to validate the **Magic Number**. If the magic number doesn't match, it rejects the packet as corrupted or invalid.
* It reads the `count` field to determine how many times it needs to iterate over the remaining memory to extract the `Order` data.

---

## 💻 Getting Started

### Prerequisites

* A C compiler (GCC, Clang, or MSVC).
* Standard C libraries.

### Compilation

Use the following command to compile the source code:

```bash
gcc -o order_entry order_entry.c

```

### Running the Demo

```bash
./order_entry

```

The program will:

1. Generate a random number of orders.
2. Serialize them into a byte buffer.
3. Print the total bytes written.
4. Parse the buffer and display the extracted trade information to the console.

---

## ⚠️ Important Considerations

* **Memory Alignment:** This code assumes that the compiler's padding for `PacketHeader` and `Order` is consistent. In a production environment, you should use `#pragma pack(push, 1)` to ensure there is no hidden padding between fields.
* **Endianness:** This implementation assumes both the sender and receiver use the same byte order (Little Endian or Big Endian). For cross-platform networking, you should use functions like `htonl()` and `ntohl()`.
* **Buffer Safety:** The `serialize_orders` function includes a basic size check, but always ensure the destination buffer is correctly allocated before calling.

