#include<stdio.h>
#include<stdbool.h>
#include<ctype.h>
#include<string.h>

int stackNumber[100];
int topNumber = -1;

char stackOperator[100];
int topOperator = -1;

bool isSpace(char c){
    if(c==' '|| c=='\t' || c=='\n'){
        return true;
    }
    return false;
}
bool isDigit(char c){
    if(c>='0' && c<='9'){
        return true;
    }
return false;
}
bool isOperator(char c){
    if(c=='+' || c=='/'||c=='*'||c=='-'){
        return true;
    } 
return false;
}
void pushNumber(int number){
    stackNumber[++topNumber]=number;
}
void pushOperator(char c){
    stackOperator[++topOperator]=c;
}
int precedence(char c){
    if(c=='*' || c=='/'){
        return 2;
    }
    if(c=='+' || c=='-'){
        return 1;
    }
    return 0;
}
int calculate(int firstNumber, int secondNumber, char operator){
    int result=0;
    switch(operator){
        case '+':
            result=firstNumber+secondNumber;
            pushNumber(result);
            break;
        case '-':
            result=firstNumber-secondNumber;
            pushNumber(result);
            break;
        case '*': 
            result=firstNumber*secondNumber;
            pushNumber(result);
            break;
        case '/':
            if(secondNumber==0){
                printf("Error: Division by zero.\n");
                return 1;
            }
            result=firstNumber/secondNumber;
            pushNumber(result);
            break;
    }
    return 0;
}
int checkPrecedence(char operator) {
    int value=precedence(operator);

    while(topOperator>=0 && precedence(stackOperator[topOperator])>=value){  
        char topOp=stackOperator[topOperator--];  
        int secondNumber=stackNumber[topNumber--];  
        int firstNumber=stackNumber[topNumber--];  
        if(calculate(firstNumber,secondNumber,topOp)==1){  
            return 1;  
        }  
    }  
    pushOperator(operator);  
    return 0;
}

int main(){
    char expression[50];
    fgets(expression, sizeof(expression), stdin);
    expression[strcspn(expression, "\n")]='\0';
    bool expectNumber=true;
    int index=0;
    while(expression[index]!='\0'){
        if(isSpace(expression[index])){
            index++;
            continue;
        }
        if(isDigit(expression[index])){
            if(!expectNumber){
                printf("Error: Invalid expression.\n");
                return 0;
            }
            int number=0;
            while(isDigit(expression[index])){
                number=number*10 + (expression[index]-'0');
                index++;
            }
            pushNumber(number);
            expectNumber=false;
        }
        else if(isOperator(expression[index])){
            if(expectNumber){
                printf("Error: Invalid expression.\n");
                return 0;
            }
            if(topOperator==-1){
                pushOperator(expression[index]);
            }
            else{
                if(checkPrecedence(expression[index])==1){
                    return 0;
                }
            }
            expectNumber = true;
            index++;
        }
        else {
            printf("Error: Invalid expression.\n");
            return 0;
        }
    }
    if(expectNumber){
        printf("Error: Invalid expression.\n");
        return 1;
    }
    while(topOperator>=0){
        char operator=stackOperator[topOperator--];
        int secondNumber=stackNumber[topNumber--];
        int firstNumber=stackNumber[topNumber--];
        if(calculate(firstNumber,secondNumber,operator)==1){
            return 0;
        }
    }
    printf("%d\n", stackNumber[topNumber]);
}
