#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#define LATEST_VERSION 1

// Structure representing the packet header containing metadata
typedef struct{
    uint32_t magic;   // Magic number to identify the packet format (0xCAFEBABE)
    uint16_t version; // Protocol version for compatibility checks
    uint16_t count;   // Number of Order entries following this header
} PacketHeader;

// Structure representing a single trade order (Payload)
typedef struct {
    uint32_t id;       // Unique identifier for the order
    uint32_t Price;    // Price of the item
    uint32_t Quantity; // Quantity of items
} Order;


// Function to serialize (pack) an array of Order structs into a raw byte buffer
int serialize_orders(char *buffer, size_t buffer_size, Order *orders, int count, int version){
    // Calculate the total size needed: Header size + (Size of one Order * Number of Orders)
    size_t required_size = sizeof(PacketHeader) + (count * sizeof(Order));

    // Safety Check: Ensure the provided buffer is large enough to hold the data
    if (required_size > buffer_size){
        printf("[MEMORY ERROR] ORDER SIZE IS TOO LARGE\n");
        return -1;
    }

    // Cast the start of the raw char buffer to a PacketHeader pointer
    // This allows us to write struct members directly into the buffer memory
    PacketHeader *header = (PacketHeader*)buffer;

    header -> magic = 0xCAFEBABE;
    header -> count = count;
    header -> version = version;

    // Calculate the memory address where the payload (orders) should start
    // We skip 'sizeof(PacketHeader)' bytes from the beginning of the buffer
    Order *payload = (Order*)(buffer + sizeof(PacketHeader));

    // Loop through the source orders and copy them into the buffer
    for (int i = 0; i < count; i++){
        payload -> id = orders[i].id;
        payload -> Price = orders[i].Price;
        payload -> Quantity = orders[i].Quantity;

        payload++; // Move the pointer to the next Order slot in the buffer
    }

    return sizeof(PacketHeader) + (count * sizeof(Order));
}


// Function to parse (unpack) a raw buffer back into structured data
int parse_packet(char *buffer){
    printf("--- Packet Received ---\n\n");

    
    // Cast the buffer to a PacketHeader pointer to read metadata
    PacketHeader *header = (PacketHeader*)buffer;

    // Validation: Check if the Magic Number matches and Version is supported
    if (header -> magic != 0xCAFEBABE || header -> version != LATEST_VERSION){
        printf("[NETWORK ERROR] Invalid Packet Received. \n");
        return -1;
    }

    printf("Magic: 0x%X\n", header -> magic);
    printf("Version:  %u\n", header -> version);
    printf("Count: %u\n\n", header -> count);



  
    Order *trades = (Order*)(buffer + sizeof(PacketHeader));

    // Iterate through the number of trades specified in the header
    for (int i = 0; i < header -> count; i++){
        printf("Trade ID =  %u Price = %u  Quantity = %u\n", trades -> id, trades -> Price, trades -> Quantity);
        // Move the pointer to the next Trade struct in memory
        trades++;
    }

    printf("Packet Processing complete. \n");
    return 0;
}



int main() {

    // Seed the random number generator
    srand(time(NULL));

    // Generate a random number of orders between 1 and 20
    int num_of_orders = (rand() % 20) + 1;
    
    // Allocate a buffer large enough to hold the header and the orders
    // Note: This uses a Variable Length Array (VLA)
    size_t size = sizeof(PacketHeader) + (num_of_orders * sizeof(Order)) + 10;
    Order my_orders[num_of_orders];

    char buffer[size]; // The raw byte buffer simulating a network packet

    // Populate the order array with random dummy data
    for (int i = 0; i < num_of_orders; i++){
        my_orders[i].id = i + 1;
        my_orders[i].Price = (rand() % 1000);
        my_orders[i].Quantity = (rand() % 50);
    }
    
    // Serialize the structured data into the raw buffer
    int total_bytes = serialize_orders(buffer, sizeof(buffer), my_orders, num_of_orders, 1);


    printf("Total Bytes Written: %d (Expected: %d)\n", total_bytes, (sizeof(PacketHeader) + (num_of_orders * sizeof(Order))));

    parse_packet(buffer);
    return 0; 

}
