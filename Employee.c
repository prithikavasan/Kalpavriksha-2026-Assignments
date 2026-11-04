#include<stdio.h>
#include<string.h>
#define MAX_EMPLOYEES 100
#define MAX_LENGTH 50
typedef struct Employee{
    int id;
    char name[MAX_LENGTH];
    char designation[MAX_LENGTH];
    int managerIndex;
    int reports[MAX_EMPLOYEES];
    int reportCount;
} Employee;

Employee employees[MAX_EMPLOYEES];

int employeeCount=0;

int findEmployee(int id){
    for (int i=0; i<employeeCount; i++){
        if (employees[i].id==id){
            return i;
        }
    }
    return -1;
}
int addReport(int managerIndex, int childIndex){
    if(managerIndex == -1){
        return 1;
    }
    if(employees[managerIndex].reportCount >= MAX_EMPLOYEES){
        return 0;
    }
    employees[managerIndex].reports[employees[managerIndex].reportCount] = childIndex;
    employees[managerIndex].reportCount++;
    return 1;
}
void removeReport(int managerIndex, int childIndex){
    if (managerIndex == -1){
        return;
    }
    for(int i=0;i<employees[managerIndex].reportCount; i++){
        if(employees[managerIndex].reports[i]==childIndex){
            for(int j=i; j<employees[managerIndex].reportCount-1;j++){
                employees[managerIndex].reports[j]=employees[managerIndex].reports[j+1];
            }
            employees[managerIndex].reportCount--;
            return;
        }
    }
}
void addEmployee(void){
    if(employeeCount>=MAX_EMPLOYEES){
        printf("Organization is full.\n");
        return;
    }
    int id, managerID;
    char name[MAX_LENGTH];
    char designation[MAX_LENGTH];
    printf("Enter Employee ID: ");
    if (scanf("%d", &id)!=1) {
        printf("Invalid ID input.\n");
        return;
    }
    if(id<=0 || findEmployee(id)!=-1){
        printf("Invalid or duplicate employee ID.\n");
        return;
    }
    printf("Enter Name: ");
    if(scanf(" %49[^\n]", name)!=1){
        printf("Invalid name.\n");
        return;
    }
    printf("Enter Designation: ");
    if(scanf(" %49[^\n]",designation)!=1){
        printf("Invalid designation.\n");
        return;
    }
    printf("Enter Manager ID (0 for CEO): ");
    if(scanf("%d", &managerID)!=1){
        printf("Invalid manager ID.\n");
        return;
    }
    if(managerID<0 || managerID==id){
        printf("Invalid manager ID.\n");
        return;
    }
    int managerIndex=-1;
    if(managerID==0){
        if(employeeCount>0){
            printf("Only one CEO is allowed.\n");
            return;
        }
    }
    else{
        managerIndex=findEmployee(managerID);

        if(managerIndex==-1){
            printf("Manager not found.\n");
            return;
        }
    }
    employees[employeeCount].id=id;
    strcpy(employees[employeeCount].name, name);
    strcpy(employees[employeeCount].designation, designation);
    employees[employeeCount].managerIndex=managerIndex;
    employees[employeeCount].reportCount=0;
    int newIndex=employeeCount;
    if(!addReport(managerIndex,newIndex)){
        printf("Manager's report list is full.\n");
        return;
    }
    employeeCount++;
    printf("Employee added successfully.\n");
}

int countReports(int index){
    int count=0;
    for(int i=0; i<employees[index].reportCount; i++) {
        int childIndex=employees[index].reports[i];
        count+=1+countReports(childIndex);
    }
    return count;
}

void displayHierarchy(int index, int level) {
    for (int i=0; i<level; i++) {
        printf("    ");
    }
    printf("%d - %s - %s\n",employees[index].id, employees[index].name, employees[index].designation);
    for (int i=0; i<employees[index].reportCount; i++) {
        displayHierarchy(employees[index].reports[i], level+1);
    }
}

void displayEmployeeHierarchy() {
    int id;
    printf("Enter Employee ID: ");
    if(scanf("%d", &id)!=1){
        printf("Invalid ID input.\n");
        return;
    }
    int index=findEmployee(id);
    if(index==-1){
        printf("Employee not found.\n");
        return;
    }
    displayHierarchy(index, 0);
    printf("Total employees under %d: %d\n", id, countReports(index));
}

void displayCompleteHierarchy(){
    if(employeeCount==0){
        printf("Organization is empty.\n");
        return;
    }
    printf("\n===== ORGANIZATION HIERARCHY =====\n");
    for(int i=0; i<employeeCount; i++){
        if(employees[i].managerIndex==-1){
            displayHierarchy(i, 0);
        }
    }
    printf("Total employees: %d\n", employeeCount);
}

void removeEmployee(){
    int id;
    printf("Enter Employee ID to remove: ");
    if(scanf("%d", &id)!=1){
        printf("Invalid ID input.\n");
        return;
    }
    int index=findEmployee(id);
    if(index==-1){
        printf("Employee not found.\n");
        return;
    }
    int superior=employees[index].managerIndex;
    if(superior==-1){
        printf("The CEO cannot be removed.\n");
        return;
    }
    int target=-1;
    int minReports=MAX_EMPLOYEES + 1;
    for(int i=0;i<employees[superior].reportCount; i++){
        int sibling=employees[superior].reports[i];
        if(sibling!=index && employees[sibling].reportCount < minReports) {
            minReports=employees[sibling].reportCount;
            target =sibling;
        }
    }
    if(target==-1){
        target=superior;
    }
    int finalReportCount;
    if(target==superior){
        finalReportCount=employees[superior].reportCount-1+employees[index].reportCount;
    } 
    else{
        finalReportCount=employees[target].reportCount+employees[index].reportCount;
    }
    if(finalReportCount>MAX_EMPLOYEES){
        printf("Cannot transfer reports: target list is full.\n");
        return;
    }
    removeReport(superior, index);
    while(employees[index].reportCount > 0){
        int child=employees[index].reports[employees[index].reportCount-1];
        if(!addReport(target, child)){
            printf("Unable to transfer reports.\n");
            return;
        }
        employees[child].managerIndex=target;
        employees[index].reportCount--;
    }
    for(int i=index;i<employeeCount-1; i++){
        employees[i]=employees[i+1];
    }
    employeeCount--;
    for (int i=0; i<employeeCount; i++) {
        if(employees[i].managerIndex>index){
            employees[i].managerIndex--;
        }
        for(int j=0;j<employees[i].reportCount; j++){
            if (employees[i].reports[j]>index){
                employees[i].reports[j]--;
            }
        }
    }
    printf("Employee removed successfully.\n");
}

int main(){
    int choice;

    while (1) {
        printf("\n===== EMPLOYEE HIERARCHY MANAGEMENT =====\n");
        printf("1. Add Employee\n");
        printf("2. Remove Employee\n");
        printf("3. Display Employee Hierarchy\n");
        printf("4. Display Complete Hierarchy\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid menu input. Exiting program.\n");
            return 0;
        }

        switch (choice) {
            case 1:
                addEmployee();
                break;

            case 2:
                removeEmployee();
                break;

            case 3:
                displayEmployeeHierarchy();
                break;

            case 4:
                displayCompleteHierarchy();
                break;

            case 5:
                printf("Exiting program.\n");
                return 0;

            default:
                printf("Invalid choice. Enter a number from 1 to 5.\n");
        }
    }
}