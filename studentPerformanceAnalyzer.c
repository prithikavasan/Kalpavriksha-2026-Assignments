#include<stdio.h>
struct Student{
    int rollNumber;
    char name[50];
    int mark1;
    int mark2;
    int mark3;
};
int calculateMark(struct Student student){
    return student.mark1+student.mark2+student.mark3;
}
float calculateAverage(int totalMarks){
    return totalMarks/3.0;
}
char calculateGrade(float average){
    if(average>=85){
        return 'A';
    }
    else if(average>=70){
        return 'B';
    }
    else if(average>=50){
        return 'C';
    }
    else if(average>=35){
        return 'D';
    }
   return 'F';
}
void performance(char grade){
    int starValue=0;
    switch(grade){
        case 'A':
            starValue=5;
            break;
        case 'B':
            starValue=4;
            break;
        case 'C':
            starValue=3;
            break;
        case 'D':
            starValue=2;
            break;
    }
    for(int i=1;i<=starValue;i++){
        printf("*");
    }
}
void rollNumberRecursion(struct Student student[], int n, int index){
    if(index>=n){
        return;
    }
    printf("%d ", student[index].rollNumber);
    rollNumberRecursion(student, n, index+1);
}
int main(){
    int n;
    scanf("%d",&n);
    if(n<=0 || n>100){
        printf("Error: Invalid Input.\n");
        return 1;
    }
    struct Student student[n]; 
    for(int i=0;i<n;i++){
        scanf("%d %s %d %d %d",&student[i].rollNumber, student[i].name, &student[i].mark1, &student[i].mark2, &student[i].mark3);
    }
    for(int i=0;i<n;i++){
        if(student[i].mark1 < 0 ||student[i].mark2<0 ||student[i].mark3 < 0 || student[i].mark1 >100 || student[i].mark2>100||student[i].mark3>100){
            printf("Error: Invalid Input.\n");
            return 1;
        }
    }
    for(int i=0;i<n;i++){
        int totalMarks = calculateMark(student[i]);
        float average = calculateAverage(totalMarks);
        char grade = calculateGrade(average);
        printf("Roll: %d\n", student[i].rollNumber);
        printf("Name: %s\n", student[i].name);
        printf("Total: %d\n", totalMarks);
        printf("Average: %.2f\n", average);
        printf("Grade: %c\n", grade);
        if(average<35){
            printf("\n");
            continue;
        }
        printf("Performance: ");
        performance(grade);
        printf("\n\n");
    }
    printf("List of Roll Numbers (via recursion): ");
    rollNumberRecursion(student, n, 0);
}