# Heap-Memory-Management

The goal is to implement the malloc and free C functions, on a heap, using the first-fit allocation strategy.
This project was done as part of the "Concepts in Programming Languages" course at the Computer Science and Engineering Department of VNIT, Nagpur, India.

# Background

The corresponding function signatures in C are:
* void* malloc(size_t size)
* void free (void* ptr)

void*: a generic pointer to any data type (it's represented in memory as a char pointer)

size_t: memory size in bytes

ptr: was allocated by malloc

# Implementation

We allocate a character array as the contigous heap memory.

To keep track of the memory 'fragments' in the heap, we keep a doubly linked list that contains the fragment's information ('mi' struct in the code).
This gives us a overhead memory cost that we must account for within the heap (sizeof(mi), which is 32 bytes in the code). Hence, sizeof(mi) are always used up within 
the heap, even prior to allocating any memory.

We use pointer arithmetic and typecasting to switch between the actual memory pointer and list node (mi).

Additionally, the main() code also checks whether the heap is freed completely or not, before the program exits.

## Allocation (malloc)

First-fit allocation strategy allocates the requested memory into the first free memory fragment that fits the needed size ('alloc_size' in the code) (done by the mem_alloc function, which is the emulated malloc function).
If the free fragment is larger than needed, it is broken into the requested fragment and the remaining as the reduced free fragment (done by the frag function). Remember that this also incurs the 
memory cost for each fragment created.

## Freeing (free)

The fragment is set free (done by the free_mem function, akin to the C free function).
Then, heap is checked for all adjacent free fragments to merge (in the merge_free_mem function), where we are refunded the cost of the struct size for each fragment merged.

### Example shown in the code
Heap: 1 KB (1000 bytes)

So available memory: 968B, as sizeof(mi) = 32 here is always present for initial MI

When allocating new memory, sizeof(mi) + alloc_size is used up; 
while the same size is freed up when requested so.

As of now, the code doesn't take user input, but a full illustration is present in the main driver code. The 'free' and 'size' variables for each fragment are printed after each step (by the print_heap_info function).


