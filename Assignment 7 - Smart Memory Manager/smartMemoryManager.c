#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#define FREE 0
#define ALLOCATED 1

struct Block{
    int start;
    int size;
    int id;
    int status;
};
char *memory;
struct Block *blocks;

int poolSize;
int count = 1;
int capacity = 4;
int nextId = 1;
int idAvailable = 1;
int strategy = 1;

int initialize(){
    printf("Enter memory pool size: ");

    if(scanf("%d", &poolSize)!=1){
        printf("Invalid input.\n");
        return 0;
    }

    if (poolSize < 1 || poolSize > 1000000){
        printf("Invalid pool size.\n");
        return 0;
    }

    memory=malloc(poolSize);
    blocks=malloc(capacity * sizeof(struct Block));

    if(memory == NULL || blocks == NULL){
        printf("Memory allocation failed.\n");
        free(memory);
        free(blocks);
        memory=NULL;
        blocks=NULL;
        return 0;
    }
    blocks[0].start=0;
    blocks[0].size=poolSize;
    blocks[0].id=0;
    blocks[0].status=FREE;

    printf("\n1. First Fit\n");
    printf("2. Best Fit\n");
    printf("3. Worst Fit\n");
    printf("Select strategy: ");
    if(scanf("%d", &strategy) != 1){
        printf("Invalid input.\n");
        free(memory);
        free(blocks);
        memory=NULL;
        blocks=NULL;
        return 0;
    }

    if (strategy<1 || strategy>3){
        printf("Invalid strategy.\n");
        free(memory);
        free(blocks);
        memory=NULL;
        blocks=NULL;
        return 0;
    }
    printf("Strategy set successfully.\n");
    return 1;
}

int ensureCapacity(){
    if(count<capacity)
        return 1;

    if(capacity>=poolSize)
        return 0;

    int newCapacity=capacity*2;

    if(newCapacity > poolSize)
        newCapacity=poolSize;

    struct Block *temp=realloc(blocks, newCapacity * sizeof(struct Block));

    if(temp==NULL)
        return 0;

    blocks=temp;
    capacity=newCapacity;
    return 1;
}

int firstFit(int size){
    for(int i=0;i<count;i++){
        if (blocks[i].status==FREE && blocks[i].size >= size){
            return i;
        }
    }

    return -1;
}
int bestFit(int size){
    int index=-1;

    for (int i=0;i<count;i++){
        if (blocks[i].status==FREE && blocks[i].size >= size){
            if (index==-1 || blocks[i].size < blocks[index].size){
                index=i;
            }
        }
    }
    return index;
}

int worstFit(int size){
    int index = -1;

    for(int i=0;i<count;i++){
        if (blocks[i].status == FREE && blocks[i].size >= size){
            if (index == -1 || blocks[i].size > blocks[index].size){
                index = i;
            }
        }
    }
    return index;
}
int findFreeBlock(int size){
    if(strategy == 1)
        return firstFit(size);

    if(strategy == 2)
        return bestFit(size);

    return worstFit(size);
}
int allocateAt(int index, int size, int id){
    int oldSize = blocks[index].size;
    int oldStart = blocks[index].start;

    if (oldSize > size){
        if (!ensureCapacity())
            return 0;

        for (int i = count; i > index; i--){
            blocks[i] = blocks[i - 1];
        }

        blocks[index].start = oldStart;
        blocks[index].size = size;
        blocks[index].id = id;
        blocks[index].status = ALLOCATED;

        blocks[index + 1].start = oldStart+size;
        blocks[index + 1].size = oldSize-size;
        blocks[index + 1].id = 0;
        blocks[index + 1].status = FREE;

        count++;
    }
    else{
        blocks[index].id = id;
        blocks[index].status = ALLOCATED;
    }

    return 1;
}

int getTotalFree(){
    int total = 0;

    for (int i=0; i<count; i++){
        if (blocks[i].status == FREE)
            total += blocks[i].size;
    }
    return total;
}

int allocateMemory(){
    int size;

    printf("Enter requested size: ");

    if (scanf("%d", &size) != 1){
        printf("Invalid input.\n");
        return 0;
    }

    if (size <= 0 || size > poolSize){
        printf("Invalid size.\n");
        return 0;
    }

    if (!idAvailable){
        printf("Allocation ID limit reached.\n");
        return 0;
    }

    int index=findFreeBlock(size);

    if (index == -1){
        int totalFree = getTotalFree();

        printf("Allocation failed.\n");
        printf("Individual free blocks: ");

        for (int i = 0; i < count; i++){
            if (blocks[i].status == FREE)
                printf("%d ", blocks[i].size);
        }
        printf("\nTotal free memory: %d\n", totalFree);
        if (totalFree >= size){
            printf("External fragmentation detected.\n");
            printf("Total free memory is sufficient, ");
            printf("but no single free block is large enough.\n");
        }
        else{
            printf("Insufficient memory.\n");
        }

        return 0;
    }
    int id = nextId;
    int start = blocks[index].start;

    if(!allocateAt(index, size, id)){
        printf("Metadata allocation failed.\n");
        return 0;
    }
    if(nextId == INT_MAX)
        idAvailable = 0;
    else
        nextId++;

    printf("Allocation successful!\n");
    printf("ID: %d Start: %d Size: %d\n",id, start, size);

    return 1;
}

int findAllocation(int id){
    for(int i = 0; i < count; i++){
        if(blocks[i].id == id && blocks[i].status == ALLOCATED){
            return i;
        }
    }
    return -1;
}

void mergeFreeBlocks(){
    int i = 0;
    while(i<count-1){
        if(blocks[i].status == FREE && blocks[i + 1].status == FREE){
            blocks[i].size += blocks[i+1].size;

            for(int j=i+1;j<count-1;j++){
                blocks[j] = blocks[j+1];
            }
            count--;
        }
        else{
            i++;
        }
    }
}

int releaseMemory(){
    int id;
    printf("Enter allocation ID: ");
    if(scanf("%d", &id) != 1){
        printf("Invalid input.\n");
        return 0;
    }
    int index = findAllocation(id);
    if(index==-1){
        printf("Invalid allocation ID.\n");
        return 0;
    }
    blocks[index].status = FREE;
    blocks[index].id = 0;
    mergeFreeBlocks();
    printf("Allocation released successfully.\n");
    return 1;
}

int resizeMemory(){
    int id, newSize;
    printf("Enter allocation ID: ");
    if (scanf("%d", &id) != 1){
        printf("Invalid input.\n");
        return 0;
    }
    int index = findAllocation(id);
    if (index == -1){
        printf("Invalid allocation ID.\n");
        return 0;
    }
    printf("Enter new size: ");
    if (scanf("%d", &newSize) != 1){
        printf("Invalid input.\n");
        return 0;
    }
    if (newSize <= 0 || newSize > poolSize){
        printf("Invalid size.\n");
        return 0;
    }
    int oldStart = blocks[index].start;
    int oldSize = blocks[index].size;
    if (newSize == oldSize){
        printf("Size unchanged.\n");
        return 1;
    }
    if (newSize < oldSize){
        int freeSize = oldSize - newSize;
        if (index + 1 < count && blocks[index + 1].status == FREE){
            blocks[index].size = newSize;
            blocks[index + 1].start = oldStart + newSize;
            blocks[index + 1].size += freeSize;
            mergeFreeBlocks();
            printf("Allocation shrunk successfull.\n");
            return 1;
        }
        if (!ensureCapacity()){
            printf("Metadata allocation failed.\n");
            return 0;
        }
        for(int i = count; i>index+1; i--){
            blocks[i] = blocks[i - 1];
        }
        blocks[index].size = newSize;
        blocks[index + 1].start = oldStart + newSize;
        blocks[index + 1].size = freeSize;
        blocks[index + 1].id = 0;
        blocks[index + 1].status = FREE;
        count++;
        mergeFreeBlocks();
        printf("Allocation shrunk successfully.\n");
        return 1;
    }
    int extra = newSize - oldSize;
    if (index + 1 < count && blocks[index + 1].status == FREE &&blocks[index + 1].size >= extra){
        blocks[index].size = newSize;
        if (blocks[index + 1].size == extra){
            for (int i = index + 1; i < count - 1; i++){
                blocks[i] = blocks[i + 1];
            }
            count--;
        }
        else{
            blocks[index + 1].start += extra;
            blocks[index + 1].size -= extra;
        }
        printf("Expanded in the same location.\n");
        return 1;
    }
    int newIndex = findFreeBlock(newSize);
    if (newIndex == -1){
        printf("Resize failed. Original data unchanged.\n");
        return 0;
    }
    int newStart = blocks[newIndex].start;
    if (!allocateAt(newIndex, newSize, id)){
        printf("Resize failed. Original data unchanged.\n");
        return 0;
    }
    memcpy(memory + newStart, memory + oldStart, oldSize);
    for (int i = 0; i < count; i++){
        if (blocks[i].start == oldStart && blocks[i].id == id && blocks[i].status == ALLOCATED)
        {
            blocks[i].status = FREE;
            blocks[i].id = 0;
            break;
        }
    }
    mergeFreeBlocks();
    printf("Allocation moved successfully.\n");
    printf("New start: %d New size: %d\n", newStart, newSize);
    return 1;
}
void displayMemoryMap(){
    printf("\nMemory Map (%d bytes)\n", poolSize);
    for (int i = 0; i < count; i++){
        if (blocks[i].status == ALLOCATED){
            printf("[Allocated #%d: %d bytes, Start: %d]\n",blocks[i].id,blocks[i].size,blocks[i].start);
        }
        else{
            printf("[Free: %d bytes, Start: %d]\n", blocks[i].size,blocks[i].start);
        }
    }
}
void displayStatistics(){
    int allocated = 0;
    int freeMemory = 0;
    int allocatedBlocks = 0;
    int freeBlocks = 0;
    int largestFree = 0;

    for (int i = 0; i < count; i++){
        if (blocks[i].status == ALLOCATED){
            allocated += blocks[i].size;
            allocatedBlocks++;
        }
        else{
            freeMemory += blocks[i].size;
            freeBlocks++;

            if (blocks[i].size > largestFree)
                largestFree = blocks[i].size;
        }
    }
    printf("\nMemory Statistics\n");
    printf("Total memory: %d bytes\n", poolSize);
    printf("Allocated: %d bytes\n", allocated);
    printf("Free: %d bytes\n", freeMemory);
    printf("Allocated blocks: %d\n", allocatedBlocks);
    printf("Free blocks: %d\n", freeBlocks);
    printf("Largest free block: %d bytes\n", largestFree);
}
int changeStrategy(){
    int choice;
    printf("\n1. First Fit\n");
    printf("2. Best Fit\n");
    printf("3. Worst Fit\n");
    printf("Select strategy: ");
    if (scanf("%d", &choice) != 1){
        printf("Invalid input.\n");
        return 0;
    }
    if (choice < 1 || choice > 3){
        printf("Invalid strategy.\n");
        return 0;
    }
    strategy = choice;
    printf("Strategy changed successfully.\n");
    return 1;
}

int main(){
    int choice;
    if (!initialize()){
        printf("Memory manager initialization failed.\n");
        return 1;
    }
    while (1){
        printf("\n========== MENU ==========\n");
        printf("1. Allocate memory\n");
        printf("2. Release memory\n");
        printf("3. Resize allocation\n");
        printf("4. Display memory map\n");
        printf("5. Display memory statistics\n");
        printf("6. Change allocation strategy\n");
        printf("7. Exit\n");
        printf("Enter choice: ");
        if (scanf("%d", &choice) != 1){
            printf("Invalid input. Exiting.\n");
            break;
        }
        switch (choice){
            case 1:
                allocateMemory();
                break;
            case 2:
                releaseMemory();
                break;
            case 3:
                resizeMemory();
                break;
            case 4:
                displayMemoryMap();
                break;
            case 5:
                displayStatistics();
                break;
            case 6:
                changeStrategy();
                break;
            case 7:
                free(memory);
                free(blocks);
                printf("Memory manager terminated.\n");
                return 0;
            default:
                printf("Invalid choice.\n");
        }
    }
    free(memory);
    free(blocks);
    return 1;
}
