/*
Name: Fauzan Nayeem Farooqui
Program: malloc and free C function implementations (uses the first-fit allocation strategy)
This project was done as part of the "Concepts in Programming Languages" course at the CSE department of VNIT, Nagpur, India.
Date: 01/04/2021

Important notes:
Below are the normal malloc and free function signatures in C:
 void* malloc(size_t size), returns a pointer to the allocated memory, which is then typecasted before assigning
 void free(void* ptr), where ptr is a pointer returned by malloc
 We emulate the same signatures here.
We also use the size_t type, which is used to store memory size in bytes, defined in stdlib.
void* is a generic pointer that can point to any datatype, using proper typecasting. It has similar representations as a char pointer.
'fragment' here means the different partitions that memory is fragmented into during the allocation/freeing processes
*/

#include<stdio.h>
#include<stdlib.h>
#define HEAP_SIZE 1000 //1 KB Heap

char full_heap[HEAP_SIZE];  //Heap storage. 1 byte each (char) allows normal memory allocation and also void* is represented as a char pointer, hence heap is made as char type
typedef struct mem_info //we need to store some info about the memories that we are allocating and freeing. So memory overhead is sizeof(mi), which is 32 here
{
    int free; //if memory is_free, then free is True i.e free = 1, otherwise 0 (when allocated)
    size_t size;
    struct mem_info *prev;
    struct mem_info *next;
}mi; //short for mem_info
mi* heap_info = (void*)full_heap; //A doubly linked list that contains info about the heap storage's memory fragment(s). It has info of all memories, as the heap is just 1 KB

void frag(mi* free_mem, size_t alloc_size) //fragments the larger free_memory into required allocated_memory_fragment and leftover free_fragment. Used in mem_alloc. Would lead to internal fragmentation
{
    mi* free_frag = (void*)((void*)free_mem + alloc_size + sizeof(mi)); //starting from the beginning of free_mem, the free_frag starts after the allocated size and then overhead memory (mi for free_mem) is considered
    free_frag->free = 1;
    free_frag->size = free_mem->size - alloc_size - sizeof(mi);
    free_frag->prev = free_mem;
    free_frag->next = free_mem->next;
    //now free_mem has a free fragment at its end. now free_mem is ready to allocate alloc_size memory at its start
    free_mem->free = 0; 
    free_mem->size = alloc_size;
    free_mem->next = free_frag;
}
void* mem_alloc(size_t alloc_size) //allocation of memory. signature explained in starting node
{
    mi *mi_ptr; //henceforth, mi_ptr usually means a temporary pointer to one of the nodes in the mi heap_info list. This gives access to each memory fragment
    void* mem_alloc_ptr; //final pointer to the actual memory allocated  
    mi_ptr = heap_info; //as we should check from the start of the heap for free memory
    int alloc=0; //a flag variable
    while(mi_ptr!=NULL && !alloc) //keep finding a suitable fragment until heap ends or memory is allocated
    {
        if(mi_ptr->free && (mi_ptr->size > (alloc_size + sizeof(mi)) || mi_ptr->size==alloc_size)) //first-fit method: choose the first free fragment that can fit the alloc_size
        {
            if(mi_ptr->size > (alloc_size + sizeof(mi))) //when fragmenting, we need to reserve memory for the smaller free fragment's MI
            { 
                frag(mi_ptr, alloc_size);
                mi_ptr+=1; //mi_ptr had pointed to the mi. Incrementing it by 1 makes it point to the actual memory allocated, which is then typecasted in the next line
                mem_alloc_ptr = (void*)mi_ptr;
                alloc = 1;
            }
            else
                if(mi_ptr->size==alloc_size) //if condition just makes it more explicit. no fragmentation, so we need not have fresh memory for MI as we simply allocate the existing free memory
                { 
                    mi_ptr->free = 0; //simply allocated
                    mi_ptr+=1;
                    mem_alloc_ptr = (void*)mi_ptr;
                    alloc = 1;
                }
        }
        else
            mi_ptr = mi_ptr->next; //if the fragment is not suitable to allocate, move on to the next one
    }

    if(!alloc) //if still unable to find a suitable fragment
    {
        mem_alloc_ptr = NULL;
        printf("\nNo suitable memory fragment available. Try freeing memory.");
    }
    
    return mem_alloc_ptr; //pointer to actual memory is finally returned
}
void merge_free_mem() //"defrag"s to avoid external fragmentation
{    
    mi *mi_ptr, *adj_mem;
    mi_ptr = heap_info; //There could be free memory before or after the just-freed memory. Hence check from beginning. 
    adj_mem = mi_ptr->next;
    while(adj_mem!=NULL) 
    {
        if(mi_ptr->free && adj_mem->free) //as long as the adjacent memory is mergable 
        {
            mi_ptr->size += sizeof(mi) + adj_mem->size; //the size update accounts for the overhead memory too
            mi_ptr->next = adj_mem->next;
           
            adj_mem = mi_ptr->next;
            //adj_mem->prev = mi_ptr; //for some reason, this would give a problem if 3 consecutive memories are found free: the first 2 are merged, but while merging 3rd one, program stops at this very line. Reason not found yet, but obviously has something to do with incorrect prev pointer. Commenting out this line makes the program work just as fine.
        }
        if(!mi_ptr->free || (adj_mem!=NULL && !adj_mem->free)) //once the current surrounding fragments are merged, move on to check for more such fragments in the remaining heap
        {
            mi_ptr = adj_mem;
            adj_mem = mi_ptr->next;
        }
    }
}
void free_mem(void* mem_alloc_ptr) //freeing up the memory. signature explained in starting note
{
    if((mem_alloc_ptr>=(void*)full_heap && mem_alloc_ptr<=(void*)(full_heap+HEAP_SIZE)) && mem_alloc_ptr!=NULL) //makes sense to only free up the memory that was allocated by mem_alloc, which should lie within full_heap
    {
        mi* mi_ptr = mem_alloc_ptr;
        mi_ptr -= 1; //now points to the MI for the memory being freed, similarly seen in mem_alloc
        if(!mi_ptr->free) //if already free, saves some time from running the merge_free_mem() unnecessarily
        {
            mi_ptr->free=1;
            merge_free_mem(); //need to combine adjacent free memory fragments, as we allocate based on free size (also asked in assignment problem statement)
        }
        else
            printf("\nMemory is already free.");
    }
    else
        printf("The memory is either NULL or was not allocated by mem_alloc function.");
}
void print_heap_info()
{
    mi* mi_ptr = heap_info; //prints from start
    printf("\nTotal Heap info: ");
    while(mi_ptr!=NULL)
    {
        printf("\nFree: %d,", mi_ptr->free); //whether free or not
        printf(" Size: %llu", mi_ptr->size); //size is of type size_t, which is typedef of unsigned long long
        mi_ptr = mi_ptr->next;
    }
    printf("\n");
}
int main()
{
    //Initializing the heap
    heap_info->free = 1;
    heap_info->size = HEAP_SIZE - sizeof(mi);
    heap_info->prev = NULL;
    heap_info->next = NULL;
    printf("\nInitialized heap.");
    print_heap_info();

    //Pattern for below examples:
    //memory is allocated in the same format as normal malloc()
    //the allocated data and then updated heap MI is printed
    char* char_ptr1 = (char*)mem_alloc(sizeof(char));
    *char_ptr1 = 'A';
    printf("\nAllocated Char: %c", *char_ptr1);
    print_heap_info();

    int* int_ptr = (int*)mem_alloc(sizeof(int));
    *int_ptr = 1;
    printf("\nAllocated Int: %d", *int_ptr);
    print_heap_info();

    float* flt_ptr = (float*)mem_alloc(sizeof(float));
    *flt_ptr = 1.1;
    printf("\nAllocated Float: %f", *flt_ptr);
    print_heap_info();

    //memory is freed in the same format as normal free()
    //A message along with the updated heap MI is printed
    free_mem(int_ptr);
    printf("\nInt memory freed");
    print_heap_info();
    
    free_mem(char_ptr1);
    printf("\nChar memory freed");
    print_heap_info();

    free_mem(int_ptr); //Integer memory was already freed. So now no action, simply displays a message.
    print_heap_info();
    
    char* char_ptr2 = (char*)mem_alloc(2*sizeof(char)); //char array of size 2
    *(char_ptr2+0) = 'B';
    *(char_ptr2+1) = 'C';
    printf("\nAllocated array of characters of size 2: %c, %c", *(char_ptr2+0), *(char_ptr2+1));
    print_heap_info();
    
    free_mem(flt_ptr);
    printf("\nFloat memory freed");
    print_heap_info();

    free_mem(char_ptr2);
    printf("\nMemory of Char array of size 2 freed");
    print_heap_info();

    if(heap_info->size == HEAP_SIZE - sizeof(mi) && heap_info->free) //same conditions as after initializing the heap
        printf("\nAny memory allocated is successfully freed.\nExiting...");
    else
        printf("\nPlease make sure to free the memory you allocated, as user/programmer is solely responsible for this.\nExiting...");

    return 0;
}