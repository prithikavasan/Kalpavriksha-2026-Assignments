#include<stdio.h>
#include<stdlib.h>
#include<time.h>

void generateMatrix(int n, int (*matrix)[n]){
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            *(*(matrix+i)+j)=rand()%256;
        }
    }
}

void printMatrix(int n, int (*matrix)[n]){
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            printf("%d ",*(*(matrix+i)+j));
        }
        printf("\n");
    }
    printf("\n");
}

void transposeMatrix(int n, int (*matrix)[n]){
    for(int i=0;i<n;i++){
        for(int j=i+1;j<n;j++){
            int temporaryVariable=*(*(matrix+i)+j);
            *(*(matrix+i)+j)=*(*(matrix+j)+i);
            *(*(matrix+j)+i)=temporaryVariable;
        }
    }
}

void rotateMatrix(int n, int (*matrix)[n]){
    
    transposeMatrix(n, matrix);
    
    for(int i=0;i<n;i++){
        int leftIndex=0, rightIndex=n-1;
        while(leftIndex<rightIndex){
            int temporaryVariable=*(*(matrix+i)+leftIndex);
            *(*(matrix+i)+leftIndex)=*(*(matrix+i)+rightIndex);
            *(*(matrix+i)+rightIndex)=temporaryVariable;
            leftIndex++;
            rightIndex--;
        }
    }
}

void smoothMatrix(int n, int (*matrix)[n]){
    
    int row1[n], row2[n], row3[n];
    int *previousRow=row1, *currentRow=row2, *nextRow=row3;
    
    for(int j=0;j<n;j++){
        *(currentRow+j)=*(*(matrix+0)+j);
    }
    if(n>1){
        for(int j=0;j<n;j++){
            *(nextRow+j)=*(*(matrix+1)+j);
        }
    }
    
    
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            int sumOfElements=0;
            int countElements=0;
            
            if(i>0){
                for(int column=j-1;column<=j+1;column++){
                    if(column>=0 && column<n){
                        sumOfElements+=*(previousRow+column);
                        countElements++;
                    }
                }
            }
            for(int column=j-1;column<=j+1;column++){
                if(column>=0 && column<n){
                    sumOfElements+=*(currentRow+column);
                    countElements++;
                }
            }
            if(i<n-1){
                for(int column=j-1;column<=j+1;column++){
                    if(column>=0 && column<n){
                        sumOfElements+=*(nextRow+column);
                        countElements++;
                    }
                }
            }
            int average=sumOfElements/countElements;
            *(*(matrix+i)+j)=average;
        }
        
        if(i<n-1){
            int *temporaryVariable=previousRow;
            previousRow=currentRow;
            currentRow=nextRow;
            nextRow=temporaryVariable;
            
            if(i+2<n){
                for(int j=0;j<n;j++){
                    *(nextRow+j)=*(*(matrix+i+2)+j);
                }
            }
        }
    }
}
int main(){
    printf("Enter matrix size (2-10):");
    int n;
    if(scanf("%d",&n)!=1||n<2||n>10){
        printf("Error: Invalid input.");
        return 1;
    }
    int matrix[n][n];
    srand(time(NULL));
    
    generateMatrix(n, matrix);
    
    printf("Original Randomly Generated Matrix:\n");
    printMatrix(n, matrix);
    
    rotateMatrix(n, matrix);
    printf("Matrix after 90 degree Clockwise Rotation:\n");
    printMatrix(n, matrix);
    
    smoothMatrix(n, matrix);
    printf("Matrix after Applying %dx%d Smoothing Filter:\n", n, n);
    printMatrix(n, matrix);
    
    return 0;
}