#include<stdio.h>
#include<string.h>
#include<stdbool.h>
#define FILE_NAME "users.txt"

bool isExit=false;

struct User{
    int id;
    char name[50];
    int age;
};

int isIdExist(int id){
    FILE *file = fopen(FILE_NAME,"r");
    struct User user;
    if(file==NULL){
        printf("Unable to open file\n");
        return 0;
    }
    while(fscanf(file, "%d, %[^,], %d", &user.id, user.name, &user.age)==3){
        if(user.id==id){
            fclose(file);
            return 1;
        }
    }
    fclose(file);
    return 0;
}

void createFile(){
    FILE *fp=fopen(FILE_NAME, "a");
    if(fp==NULL){
        printf("Unable to create/open file.\n");
        isExit=true;
        return;
    }
    fclose(fp);
}
void createUser(){
    struct User user;
    printf("\nEnter User ID: ");
    scanf("%d",&user.id);
    getchar();
    if(isIdExist(user.id)){
        printf("ID already exists\n");
        return;
    }
    printf("Enter Name: ");
    fgets(user.name, sizeof(user.name), stdin);
    user.name[strcspn(user.name, "\n")]='\0';
    
    printf("Enter Age: ");
    scanf("%d", &user.age);
    
    FILE *file;
    file=fopen(FILE_NAME,"a");
    if(file==NULL){
        printf("Unable to open file\n");
        return;
    }
    fprintf(file, "%d, %s, %d\n", user.id, user.name, user.age);
    fclose(file);
    
    printf("User created successfully");
    
}

void readUser(){
    FILE *file;
    struct User user;
    int found=0;
    file=fopen(FILE_NAME, "r");
    if(file==NULL){
        printf("No users found\n");
        return;
    }
    while(fscanf(file,"%d, %[^,], %d", &user.id, user.name, &user.age)==3){
        found=1;
        printf("ID: %d\n",user.id);
        printf("Name: %s\n",user.name);
        printf("Age: %d\n", user.age);
    }
    fclose(file);
    if(found==0){
        printf("No users found\n");
    }
}
void updateUser(){
    FILE *file;
    FILE *temp;
    struct User user;
    int found=0;
    int searchId;
    printf("Enter ID to Update: ");
    scanf("%d", &searchId);
    
    file = fopen(FILE_NAME, "r");
    
    if(file==NULL){
        printf("File not found\n");
        return;
    }
    temp = fopen("temp.txt", "w");
    if(temp==NULL){
        printf("Unable to create temporary file.\n");
        fclose(file);
        return;
    }
    while(fscanf(file,"%d, %[^,], %d",&user.id, user.name, &user.age)==3){
        if(user.id==searchId){
            found=1;
            getchar();
            printf("Enter new name: ");
            fgets(user.name, sizeof(user.name),stdin);
            user.name[strcspn(user.name, "\n")]='\0';
            
            printf("Enter new age: ");
            scanf("%d",&user.age);
        }
        fprintf(temp,"%d, %s, %d\n", user.id, user.name, user.age);
    }
    fclose(file);
    fclose(temp);
    
    remove(FILE_NAME);
    rename("temp.txt", FILE_NAME);
    if(found)
        printf("User updated successfully\n");
    else
        printf("User ID not found.\n");
}

void deleteUser(){
    FILE *file;
    FILE *temp;
    struct User user;
    int deleteId;
    int found=0;
    printf("Enter ID to delete: ");
    scanf("%d",&deleteId);
    
    file=fopen(FILE_NAME, "r");
    
    if(file==NULL){
        printf("File not found.\n");
        return;
    }
    temp=fopen("temp.txt", "w");
    if(temp==NULL){
        printf("Unable to create temporary file.\n");
        fclose(file);
        return;
    }
    while(fscanf(file,"%d, %[^,], %d",&user.id, user.name, &user.age)==3){
        if(user.id==deleteId){
            found=1;
        }
        else{
            fprintf(temp,"%d, %s, %d\n",user.id, user.name, user.age);
        }
    }
    fclose(file);
    fclose(temp);
    
    remove(FILE_NAME);
    rename("temp.txt",FILE_NAME);
    if(found)
    printf("User deleted successfully.\n");
    else
    printf("User ID not found.\n");
}

int main(){
    int choice;
    createFile();
    while(!isExit){
        printf("\n======USER CRUD======\n");
        printf("1. Create User\n");
        printf("2. Read Users\n");
        printf("3. Update User\n");
        printf("4. Delete User\n");
        printf("5. Exit\n");
        printf("=========================\n");
        printf("Enter your choice: ");
        scanf("%d",&choice);
        switch(choice){
            case 1:
                createUser();
                break;
            case 2:
                readUser();
                break;
            case 3:
                updateUser();
                break;
            case 4:
                deleteUser();
                break;
            case 5:
                isExit=true;
                break;
            default:
                printf("Invalid choice\n");
        }
    }
}