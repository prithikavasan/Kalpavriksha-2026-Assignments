#include<stdio.h>
#include<string.h>
int main(){
    const int standard = 150;
    const int premium = 250;
    const int recliner = 400;
    
    char name[50];
    scanf("%49s",name);
    
    int ticketsCount;
    scanf("%d",&ticketsCount);
    
    if(ticketsCount<1 || ticketsCount>20){
        printf("Invalid ticket count\n");
        return 1;
    }
    
    int ticketCategory;
    scanf("%d", &ticketCategory);
    
    int ticketPrice;
    char category[50];
    
    switch(ticketCategory){
        case 1:
            ticketPrice=standard;
            strcpy(category, "Standard");
            break;
        case 2:
            ticketPrice=premium;
            strcpy(category, "Premium");
            break;
        case 3:
            ticketPrice=recliner;
            strcpy(category, "Recliner");
            break;
        default:
            printf("Invalid category\n");
            return 1;
    }
    
    float subtotal = ticketsCount * ticketPrice;
    
    float finalBill, discount;
    
    if(ticketsCount>=5){
        discount = subtotal / 10.0;
        finalBill=subtotal-discount;
    }
    else{
        discount=0;
        finalBill=subtotal;
    }
    
    printf("Customer: %s\n", name);
    printf("Ticket Category: %s\n", category);
    printf("Tickets: %d\n", ticketsCount);
    printf("Subtotal: %.2f\n", subtotal);
    printf("Discount: %.2f\n", discount);
    printf("Final Bill: %.2f\n", finalBill);
    
    return 0;
}