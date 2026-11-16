#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BLOCK_SIZE 512
#define NUM_BLOCKS 1024
#define MAX_NAME 50
#define MAX_INPUT 16384
char virtualDisk[NUM_BLOCKS][BLOCK_SIZE];

typedef struct FreeBlock{
    int index;
    struct FreeBlock *next,*prev;
}FreeBlock;

typedef struct FileNode{
    char name[MAX_NAME+1];
    int isDirectory;
    struct FileNode *next,*prev,*parent,*children;
    int *blockPointers;
    int blockCount,contentSize;
}FileNode;

FreeBlock freeNodes[NUM_BLOCKS];
FreeBlock *freeHead=NULL,*freeTail=NULL;
FileNode *root=NULL,*cwd=NULL;
int freeCount=0;

char *trim(char *str){
    while(*str==' '||*str=='\t')str++;
    int len=strlen(str);
    while(len>0&&(str[len-1]==' '||str[len-1]=='\t'||str[len-1]=='\n'||str[len-1]=='\r'))str[--len]='\0';
    return str;
}

void initializeDisk(){
    freeHead=NULL;
    freeTail=NULL;
    freeCount=0;
    for(int i=0;i<NUM_BLOCKS;i++){
        freeNodes[i].index=i;
        freeNodes[i].next=NULL;
        freeNodes[i].prev=freeTail;
        if(freeHead==NULL){
            freeHead=&freeNodes[i];
        }
        else{
            freeTail->next=&freeNodes[i];
        }
        freeTail=&freeNodes[i];
        freeCount++;
    }
}

int allocateBlock(){
    if(freeHead==NULL)return -1;
    FreeBlock *temp=freeHead;
    int index=temp->index;
    freeHead=temp->next;
    if(freeHead!=NULL){
        freeHead->prev=NULL;
    }
    else{ 
        freeTail=NULL;
    }
    temp->next=NULL;
    temp->prev=NULL;
    freeCount--;
    return index;
}

void freeBlock(int index){
    if(index<0||index>=NUM_BLOCKS)return;
    FreeBlock *temp=&freeNodes[index];
    temp->next=NULL;
    temp->prev=freeTail;
    if(freeTail==NULL){
        freeHead=temp;
    }
    else{
        freeTail->next=temp;
    }
    freeTail=temp;
    freeCount++;
}

void initializeRoot(){
    root=calloc(1,sizeof(FileNode));
    if(root==NULL){
        printf("Memory allocation failed.\n");
        exit(1);
    }
    strcpy(root->name,"/");
    root->isDirectory=1;
    root->next=root;
    root->prev=root;
    cwd=root;
}

FileNode *findChild(FileNode *dir,const char *name){
    if(dir==NULL||dir->children==NULL){
        return NULL;
    }
    FileNode *temp=dir->children;
    do{
        if(strcmp(temp->name,name)==0){
            return temp;
        }
        temp=temp->next;
    }
    while(temp!=dir->children);
    return NULL;
}

void insertChild(FileNode *dir,FileNode *node){
    node->parent=dir;
    if(dir->children==NULL){
        node->next=node;
        node->prev=node;
        dir->children=node;
    }
    else{
        FileNode *head=dir->children;
        FileNode *tail=head->prev;
        tail->next=node;
        node->prev=tail;
        node->next=head;
        head->prev=node;
    }
}

void removeChild(FileNode *dir,FileNode *node){
    if(node->next==node){
        dir->children=NULL;
    }
    else{
        node->prev->next=node->next;
        node->next->prev=node->prev;
        if(dir->children==node)dir->children=node->next;
    }
    node->next=NULL;
    node->prev=NULL;
    node->parent=NULL;
}

int validName(const char *name){
    int len=strlen(name);
    if(len==0||len>MAX_NAME){
        return 0;
    }
    if(strchr(name,'/')!=NULL){
        return 0;
    }
    if(strcmp(name,".")==0||strcmp(name,"..")==0){
        return 0;
    }
    return 1;
}

void mkdirCommand(char *name){
    if(!validName(name)){
        printf("Invalid directory name.\n");
        return;
    }
    if(findChild(cwd,name)!=NULL){
        printf("Name already exists.\n");
        return;
    }
    FileNode *node=calloc(1,sizeof(FileNode));
    if(node==NULL){
        printf("Memory allocation failed.\n");
        return;
    }
    strcpy(node->name,name);
    node->isDirectory=1;
    insertChild(cwd,node);
    printf("Directory '%s' created successfully.\n",name);
}

void createCommand(char *name){
    if(!validName(name)){
        printf("Invalid file name.\n");
        return;
    }
    if(findChild(cwd,name)!=NULL){
        printf("Name already exists.\n");
        return;
    }
    FileNode *node=calloc(1,sizeof(FileNode));
    if(node==NULL){
        printf("Memory allocation failed.\n");
        return;
    }
    strcpy(node->name,name);
    node->isDirectory=0;
    insertChild(cwd,node);
    printf("File '%s' created successfully.\n",name);
}

void writeCommand(char *name,char *content){
    FileNode *file=findChild(cwd,name);
    if(file==NULL||file->isDirectory){
        printf("File not found.\n");
        return;
    }
    int size=strlen(content);
    int requiredBlocks=(size+BLOCK_SIZE-1)/BLOCK_SIZE;
    if(requiredBlocks>freeCount+file->blockCount){
        printf("Disk full! Not enough free blocks.\n");
        return;
    }
    int *newPointers=NULL;
    if(requiredBlocks>0){
        newPointers=malloc(requiredBlocks*sizeof(int));
        if(newPointers==NULL){
            printf("Memory allocation failed.\n");
            return;
        }
    }
    for(int i=0;i<file->blockCount;i++){
        freeBlock(file->blockPointers[i]);
    }
    free(file->blockPointers);
    
    file->blockPointers=newPointers;
    file->blockCount=0;
    
    for(int i=0;i<requiredBlocks;i++){
        int index=allocateBlock();
        if(index==-1){
            printf("Block allocation failed.\n");
            
            for(int j=0;j<file->blockCount;j++){
                freeBlock(file->blockPointers[j]);
            }
            free(file->blockPointers);
            file->blockPointers=NULL;
            file->blockCount=0;
            file->contentSize=0;
            return;
        }
        file->blockPointers[i]=index;
        file->blockCount++;
    }
    int offset=0;
    for(int i=0;i<requiredBlocks;i++){
        int bytes=size-offset;
        
        if(bytes>BLOCK_SIZE)bytes=BLOCK_SIZE;
        memset(virtualDisk[file->blockPointers[i]],0,BLOCK_SIZE);
        memcpy(virtualDisk[file->blockPointers[i]],content+offset,bytes);
        
        offset+=bytes;
    }
    file->contentSize=size;
    printf("Data written successfully (size=%d bytes).\n",size);
}

void readCommand(char *name){
    FileNode *file=findChild(cwd,name);
    if(file==NULL||file->isDirectory){
        printf("File not found.\n");
        return;
    }
    if(file->contentSize==0){
        printf("File is empty.\n");
        return;
    }
    int remaining=file->contentSize;
    for(int i=0;i<file->blockCount;i++){
        int bytes=remaining;
        if(bytes>BLOCK_SIZE){
            bytes=BLOCK_SIZE;
        }
        fwrite(virtualDisk[file->blockPointers[i]],1,bytes,stdout);
        remaining-=bytes;
    }
    printf("\n");
}

void deleteCommand(char *name){
    FileNode *file=findChild(cwd,name);
    if(file==NULL||file->isDirectory){
        printf("File not found.\n");
        return;
    }
    for(int i=0;i<file->blockCount;i++){
        freeBlock(file->blockPointers[i]);
    }
    free(file->blockPointers);
    removeChild(cwd,file);
    free(file);
    printf("File deleted successfully.\n");
}

void rmdirCommand(char *name){
    FileNode *dir=findChild(cwd,name);
    if(dir==NULL||!dir->isDirectory){
        printf("Directory not found.\n");
        return;
    }
    if(dir->children!=NULL){
        printf("Directory is not empty.\n");
        return;
    }
    removeChild(cwd,dir);
    free(dir);
    printf("Directory removed successfully.\n");
}

void lsCommand(FileNode *dir){
    if(dir==NULL||dir->children==NULL){
        printf("(empty)\n");
        return;
    }
    FileNode *temp=dir->children;
    do{
        if(temp->isDirectory){
            printf("[DIR]  %s/\n",temp->name);
        }
        else{
            printf("[FILE] %s (%d bytes)\n",temp->name,temp->contentSize);
        }
        temp=temp->next;
    }
    while(temp!=dir->children);
}

FileNode *resolveDirectory(char *path){
    if(path==NULL||strlen(path)==0){
        return cwd;
    }
    char temp[4096];
    if(strlen(path)>=sizeof(temp)){
        return NULL;
    }
    strcpy(temp,path);
    FileNode *current;
    if(path[0]=='/'){
        current = root;
    }
    else{
        current=cwd;
    }
    char *part=strtok(temp,"/");
    while(part!=NULL){
        if(strcmp(part,".")==0){
            current=current;
        }
        else if(strcmp(part,"..")==0){
            if(current->parent!=NULL){
                current=current->parent;
            }
        }
        else{
            FileNode *child=findChild(current,part);
            if(child==NULL||!child->isDirectory){
                return NULL;
            }
            current=child;
        }
        part=strtok(NULL,"/");
    }
    return current;
}

void printPath(){
    FileNode *stack[NUM_BLOCKS];
    int count=0;
    FileNode *temp=cwd;
    while(temp!=NULL && temp!=root && count<NUM_BLOCKS){
        stack[count++]=temp;
        temp=temp->parent;
    }
    printf("/");
    for(int i=count-1;i>=0;i--){
        printf("%s",stack[i]->name);
        if(i>0){
            printf("/");
        }
    }
}

void cdCommand(char *path){
    FileNode *dir=resolveDirectory(path);
    if(dir==NULL){
        printf("Directory not found.\n");
        return;
    }
    cwd=dir;
    printf("Moved to ");
    printPath();
    printf("\n");
}

void pwdCommand(){
    printPath();
    printf("\n");
}

void dfCommand(){
    int usedBlocks=NUM_BLOCKS-freeCount;
    float usage=((float)usedBlocks/NUM_BLOCKS)*100;
    printf("Total Blocks: %d\n", NUM_BLOCKS);
    printf("Used Blocks: %d\n", usedBlocks);
    printf("Free Blocks: %d\n", freeCount);
    printf("Disk Usage: %.2f%%\n", usage);
}

void releaseDirectory(FileNode *dir){
    if(dir==NULL||dir->children==NULL){
        return;
    }
    FileNode *head=dir->children;
    FileNode *temp=head;
    do{
        FileNode *next=temp->next;
        if(temp->isDirectory){
            releaseDirectory(temp);
        }
        else{
            for(int i=0;i<temp->blockCount;i++){
                freeBlock(temp->blockPointers[i]);
            }
            free(temp->blockPointers);
        }
        free(temp);
        temp=next;
    }
    while(temp!=head);
    dir->children=NULL;
}

int main(){
    initializeDisk();
    initializeRoot();
    printf("Compact VFS - ready. Type 'exit' to quit.\n");
    char line[MAX_INPUT];
    while(1){
        printPath();
        printf(" > ");
        if(fgets(line,sizeof(line),stdin)==NULL){
            break;
        }
        char *input=trim(line);
        if(*input=='\0'){
            continue;
        }
        char *space=strpbrk(input," \t");
        char *args="";
        if(space!=NULL){
            *space='\0';
            args=trim(space+1);
        }
        if(strcmp(input,"exit")==0)
            break;
        else if(strcmp(input,"mkdir")==0){
            if(*args=='\0')
                printf("Usage: mkdir directory_name\n");
            else 
                mkdirCommand(args);
        }
        else if(strcmp(input,"create")==0){
            if(*args=='\0')
                printf("Usage: create filename\n");
            else 
                createCommand(args);
        }
        else if(strcmp(input,"write")==0){
            char *content;
            char *name=args;
            char *separator=strpbrk(name," \t");
            if(separator==NULL){
                printf("Usage: write filename \"content\"\n");
                continue;
            }
            *separator='\0';
            content=trim(separator+1);
            if(content[0]=='"'){
                int len=strlen(content);
                if(len<2||content[len-1]!='"'){
                    printf("Invalid quoted content.\n");
                    continue;
                }
                content[len-1]='\0';
                content++;
            }
            writeCommand(name,content);
        }
        else if(strcmp(input,"read")==0){
            if(*args=='\0')
                printf("Usage: read filename\n");
            else 
                readCommand(args);
        }
        else if(strcmp(input,"delete")==0){
            if(*args=='\0')
                printf("Usage: delete filename\n");
            else 
                deleteCommand(args);
        }
        else if(strcmp(input,"rmdir")==0){
            if(*args=='\0')
                printf("Usage: rmdir directory_name\n");
            else 
                rmdirCommand(args);
        }else if(strcmp(input,"ls")==0){
            FileNode *dir;
            if(*args=='\0'){
                dir=cwd;
            }
            else{
                dir=resolveDirectory(args);
            }
            if(dir==NULL)
                printf("Directory not found.\n");
            else 
                lsCommand(dir);
        }
        else if(strcmp(input,"cd")==0){
            if(*args=='\0')
                printf("Usage: cd directory_name\n");
            else 
                cdCommand(args);
        }
        else if(strcmp(input,"pwd")==0)
            pwdCommand();
        else if(strcmp(input,"df")==0)
            dfCommand();
        else 
            printf("Unknown command: %s\n",input);
    }
    releaseDirectory(root);
    free(root);
    root=NULL;
    cwd=NULL;
    printf("Memory released. Exiting program...\n");
    return 0;
}
