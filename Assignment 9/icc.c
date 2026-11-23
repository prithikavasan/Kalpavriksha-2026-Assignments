#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Players_data.h"
#define T 10
#define MAX 51
typedef struct PlayerRecord{
    int id;
    char name[51];
    int teamId;
    int role;
    int runs;
    float average, strikeRate;
    int wickets;
    float economy, performance;
    struct PlayerRecord *next;
    struct PlayerRecord *roleNext;
} PlayerRecord;

typedef struct{
    int id;
    char name[51];
    int totalPlayers;
    PlayerRecord *head;
    PlayerRecord *tail;
    PlayerRecord *roleHead[3];
} Team;

typedef struct{
    PlayerRecord *player;
    int teamIndex;
} HeapNode;

Team teamsList[T];
char *roles[]={"", "Batsman", "Bowler", "All-rounder"};

float calculatePerformance(PlayerRecord *p){
    if (p->role == 1)
        return (p->average*p->strikeRate)/100.0;

    if (p->role == 2)
        return (p->wickets*2)+(100-p->economy);

    return ((p->average*p->strikeRate)/100.0)+(p->wickets*2);
}

int getRole(char *role){
    if(strcmp(role, "Batsman")==0)
        return 1;

    if(strcmp(role, "Bowler")==0)
        return 2;

    if(strcmp(role, "All-rounder")==0)
        return 3;

    return 0;
}

PlayerRecord *createPlayer(int id, char *name, int teamId, int role, int runs, float avg, float sr, int wickets, float economy){
    PlayerRecord *p=malloc(sizeof(PlayerRecord));
    if(p==NULL){
        printf("Memory allocation failed\n");
        exit(1);
    }
    p->id=id;
    strcpy(p->name, name);
    p->teamId=teamId;
    p->role=role;
    p->runs=runs;
    p->average=avg;
    p->strikeRate=sr;
    p->wickets=wickets;
    p->economy=economy;
    p->performance=calculatePerformance(p);
    p->next=NULL;
    p->roleNext=NULL;
    return p;
}

int searchTeam(int id){
    int low=0, high=T-1;
    while (low<=high) {
        int mid=(low+high)/2;
        if (teamsList[mid].id==id)
            return mid;
        if (teamsList[mid].id<id)
            low=mid+1;
        else
            high=mid-1;
    }
    return -1;
}

void insertPlayer(Team *team, PlayerRecord *p){
    if(team->head==NULL){
        team->head = p;
        team->tail = p;
    } 
    else{
        team->tail->next = p;
        team->tail = p;
    }
    int index=p->role-1;
    PlayerRecord **temp=&team->roleHead[index];
    while (*temp!=NULL && (*temp)->performance >= p->performance) {
        temp = &((*temp)->roleNext);
    }
    p->roleNext = *temp;
    *temp = p;
    team->totalPlayers++;
}

void initialize(){
    for (int i=0; i<T; i++) {
        teamsList[i].id=i+1;
        strcpy(teamsList[i].name, teams[i]);
        teamsList[i].totalPlayers=0;
        teamsList[i].head=NULL;
        teamsList[i].tail=NULL;
        for (int j=0; j<3; j++)
            teamsList[i].roleHead[j]=NULL;
    }
    for (int i=0; i<playerCount; i++) {
        int low=0, high=T-1;
        int index=-1;
        while(low<=high) {
            int mid=(low+high)/2;
            int result=strcmp(teams[mid], players[i].team);
            if (result==0) {
                index=mid;
                break;
            }
            if(result<0)
                low=mid+1;
            else
                high=mid-1;
        }
        int role=getRole((char *)players[i].role);
        if(index!=-1 && role!=0){
            PlayerRecord *p = createPlayer(players[i].id,(char *)players[i].name,index+1,role,players[i].totalRuns,players[i].battingAverage,players[i].strikeRate,players[i].wickets,players[i].economyRate);
            insertPlayer(&teamsList[index], p);
        }
    }
    printf("%d players initialized successfully!\n", playerCount);
}

float calculateAverageSR(Team *team) {
    float total=0;
    int count=0;
    PlayerRecord *temp=team->head;
    while (temp!=NULL) {
        if (temp->role==1 || temp->role==3) {
            total+=temp->strikeRate;
            count++;
        }
        temp=temp->next;
    }
    if (count==0)
        return 0;
    return total/count;
}

void displayPlayer(PlayerRecord *p, int showTeam) {
    if(showTeam){
        printf("%-6d %-23s %-15s %-13s %-7d %-7.1f %-7.1f %-7d %-7.1f %-8.2f\n",p->id, p->name, teamsList[p->teamId - 1].name, roles[p->role], p->runs, p->average, p->strikeRate,p->wickets, p->economy,p->performance);
    } 
    else{
        printf("%-6d %-23s %-13s %-7d %-7.1f %-7.1f %-7d %-7.1f %-8.2f\n", p->id, p->name,roles[p->role], p->runs,p->average, p->strikeRate,p->wickets, p->economy,p->performance);
    }
}

void displayHeader(int showTeam){
    if(showTeam){
        printf("\n%-6s %-23s %-15s %-13s %-7s %-7s %-7s %-7s %-7s %-8s\n","ID", "Name", "Team", "Role", "Runs","Avg", "SR", "Wkts", "Econ", "Index");
    } 
    else{
        printf("\n%-6s %-23s %-13s %-7s %-7s %-7s %-7s %-7s %-8s\n","ID", "Name", "Role", "Runs","Avg", "SR", "Wkts", "Econ", "Index");
    }
    printf("------------------------------------------------------------------------------------------------------------\n");
}

void addPlayer(){
    int teamId, id, role, runs, wickets;
    float avg, sr, economy;
    char name[51];
    printf("Enter Team ID: ");
    scanf("%d", &teamId);
    int index=searchTeam(teamId);
    if(index==-1){
        printf("Team not found!\n");
        return;
    }
    Team *team = &teamsList[index];
    if(team->totalPlayers>=50){
        printf("Maximum players reached!\n");
        return;
    }
    printf("Enter Player ID: ");
    scanf("%d", &id);
    if (id<1 || id>1000){
        printf("Invalid Player ID!\n");
        return;
    }
    for (int i=0; i<T; i++) {
        PlayerRecord *temp=teamsList[i].head;
        while (temp!=NULL){
            if (temp->id==id) {
                printf("Player ID already exists!\n");
                return;
            }
            temp=temp->next;
        }
    }
    printf("Enter Name: ");
    scanf(" %50[^\n]", name);
    printf("Role (1-Batsman, 2-Bowler, 3-All-rounder): ");
    scanf("%d", &role);
    if (role<1 || role>3) {
        printf("Invalid role!\n");
        return;
    }
    printf("Total Runs: ");
    scanf("%d", &runs);
    printf("Batting Average: ");
    scanf("%f", &avg);
    printf("Strike Rate: ");
    scanf("%f", &sr);
    printf("Wickets: ");
    scanf("%d", &wickets);
    printf("Economy Rate: ");
    scanf("%f", &economy);
    if (runs<0 || avg<0 || sr<0 || wickets<0 || economy<0) {
        printf("Invalid player statistics!\n");
        return;
    }

    PlayerRecord *p = createPlayer(id, name, teamId, role, runs, avg, sr, wickets, economy);
    insertPlayer(team, p);
    printf("Player added successfully to Team %s!\n",team->name);
    printf("Performance Index: %.2f\n", p->performance);
}

void displayTeamPlayers() {
    int id;
    printf("Enter Team ID: ");
    scanf("%d", &id);
    int index = searchTeam(id);
    if (index == -1) {
        printf("Team not found!\n");
        return;
    }
    Team *team = &teamsList[index];
    printf("\nPlayers of Team %s\n", team->name);
    displayHeader(0);
    PlayerRecord *temp = team->head;
    while(temp != NULL){
        displayPlayer(temp, 0);
        temp=temp->next;
    }
    printf("\nTotal Players: %d\n", team->totalPlayers);
    printf("Average Batting Strike Rate: %.2f\n", calculateAverageSR(team));
}

void displayTeamsSorted() {
    Team *arr[T];
    for (int i=0; i<T; i++)
        arr[i]=&teamsList[i];
    for (int i=1; i<T; i++) {
        Team *key=arr[i];
        int j=i-1;
        while(j>=0 && calculateAverageSR(arr[j])<calculateAverageSR(key)) {
            arr[j+1]=arr[j];
            j--;
        }
        arr[j+1]=key;
    }
    printf("\nTeams Sorted by Average Batting Strike Rate\n");
    printf("-------------------------------------------------------------\n");
    printf("%-8s %-22s %-18s %-10s\n","ID", "Team Name", "Average SR", "Players");

    for (int i=0; i<T; i++) {
        printf("%-8d %-22s %-18.2f %-10d\n",arr[i]->id,arr[i]->name,calculateAverageSR(arr[i]),arr[i]->totalPlayers);
    }
}

void displayTopK() {
    int id, role, k;
    printf("Enter Team ID: ");
    scanf("%d", &id);
    int index=searchTeam(id);
    if (index==-1) {
        printf("Team not found!\n");
        return;
    }
    printf("Enter Role (1-Batsman, 2-Bowler, 3-All-rounder): ");
    scanf("%d", &role);
    if (role<1 || role>3) {
        printf("Invalid role!\n");
        return;
    }
    printf("Enter K: ");
    scanf("%d", &k);
    if (k<=0) {
        printf("Invalid K!\n");
        return;
    }
    PlayerRecord *temp = teamsList[index].roleHead[role - 1];
    printf("\nTop %d %s of Team %s\n", k, roles[role], teamsList[index].name);
    displayHeader(0);
    int count = 0;
    while (temp!=NULL && count<k) {
        displayPlayer(temp, 0);
        temp=temp->roleNext;
        count++;
    }
    if (count==0)
        printf("No players found!\n");
}

void swap(HeapNode *a, HeapNode *b) {
    HeapNode temp=*a;
    *a=*b;
    *b=temp;
}

void heapPush(HeapNode heap[], int *size, HeapNode value) {
    int i=(*size)++;
    heap[i]=value;
    while(i>0){
        int parent=(i-1)/2;
        if (heap[parent].player->performance >= heap[i].player->performance) break;

        swap(&heap[parent], &heap[i]);
        i=parent;
    }
}

HeapNode heapPop(HeapNode heap[], int *size) {
    HeapNode result=heap[0];
    heap[0]=heap[--(*size)];
    int i=0;
    while(1){
        int left=2*i+1;
        int right=2*i+2;
        int largest=i;
        if(left < *size && heap[left].player->performance > heap[largest].player->performance){
            largest=left;
        }

        if(right < *size && heap[right].player->performance >heap[largest].player->performance)
            largest=right;

        if(largest==i)
            break;

        swap(&heap[i], &heap[largest]);
        i=largest;
    }
    return result;
}

void displayAllRolePlayers() {
    int role;
    printf("Enter Role (1-Batsman, 2-Bowler, 3-All-rounder): ");
    scanf("%d", &role);
    if (role<1 || role>3) {
        printf("Invalid role!\n");
        return;
    }
    HeapNode heap[T];
    int size=0;
    for (int i=0; i<T; i++) {
        PlayerRecord *temp = teamsList[i].roleHead[role-1];
        if (temp != NULL) {
            HeapNode node;
            node.player=temp;
            node.teamIndex=i;
            heapPush(heap, &size, node);
        }
    }
    printf("\nAll %s Players Across All Teams\n", roles[role]);
    displayHeader(1);
    int count=0;
    while(size>0){
        HeapNode node=heapPop(heap, &size);
        PlayerRecord *p=node.player;
        displayPlayer(p, 1);
        count++;
        if(p->roleNext != NULL){
            HeapNode next;
            next.player=p->roleNext;
            next.teamIndex=node.teamIndex;
            heapPush(heap, &size, next);
        }
    }
    printf("\nTotal Players: %d\n", count);
}

void freeMemory() {
    for(int i=0; i<T; i++){
        PlayerRecord *temp = teamsList[i].head;
        while(temp != NULL){
            PlayerRecord *next=temp->next;
            free(temp);
            temp=next;
        }
    }
}
int main(){
    int choice;
    initialize();
    while(1){
        printf("\n");
        printf("====================================================\n");
        printf("       ICC ODI PLAYER PERFORMANCE ANALYZER\n");
        printf("====================================================\n");
        printf("1. Add Player to Team\n");
        printf("2. Display Players of a Specific Team\n");
        printf("3. Display Teams by Average Batting Strike Rate\n");
        printf("4. Display Top K Players of a Specific Team by Role\n");
        printf("5. Display All Players of a Specific Role Across All Teams\n");
        printf("6. Exit\n");
        printf("====================================================\n");
        printf("Enter your choice: ");
        if(scanf("%d", &choice) != 1)
            break;

        switch(choice){
            case 1:
                addPlayer();
                break;
            case 2:
                displayTeamPlayers();
                break;
            case 3:
                displayTeamsSorted();
                break;
            case 4:
                displayTopK();
                break;
            case 5:
                displayAllRolePlayers();
                break;
            case 6:
                freeMemory();
                printf("Exiting program...\n");
                return 0;
            default:
                printf("Invalid choice!\n");
        }
    }
    freeMemory();
    return 0;
}