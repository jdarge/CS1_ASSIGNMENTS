//EXTERNAL HEADERS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//INTERNAL HEADER
#include "leak_detector_c.h"
//#include "leak_detector_c.c"

//CONSTANTS
#define GROUND 10
#define LENGTH 50 + 1

//--------------------------------------------------------------------STRUCTURES

typedef struct Node {
    int info;
    struct Node* next;
    struct queue* Q;
} Node;

typedef struct soldier {
    int data;
    struct soldier* next;
    struct soldier* prev;
} soldier;

typedef struct queue {
    int loc;//GROUND PLACEMENT
    int soldierCount;//STARTING SOLDIER COUNT
    int n;//CURRENT SOLDIER COUNT
    int k;//K VALUE
    int th;//TH VALUE
    char* type;//GROUND NAME
    soldier* head;
    soldier* tail;
} queue;

//--------------------------------------------------------------------PROTOTYPES

//FUNCTION REQUIRED
int peek(queue* q);
int isEmpty(queue *qPtr);
void enQueue(queue* q, int n);
int deQueue(queue* q);
soldier* createSoldier(int sequence);
void createReverseCircle(queue *q);
void rearrangeCircle(queue* q);
void display(FILE* out, queue q);

//FUNCTION INITIALIZING
void init(queue* q);
Node *sortNode(Node* head, int* location, char* type, int* numSoldier, int* k, int* th);
Node* fillNode(Node* store, int* location, char* type, int* numSoldier, int* k, int* th);
queue* fillQueue(queue* qPtr, Node* head);

//FUNCTION UTILITY
int shift(queue* q, int k);
void displayStart(FILE* out, queue **qPtr, int n);
void printBoth(FILE* out, char* str);

//FUNCTION PHASES
void start(FILE* in, FILE* out, int G, queue** qPtr);
void phase1(FILE* out, queue** q);
void phase2(FILE* out, queue** q, int G);

//--------------------------------------------------------------------------MAIN

int main(void){
    atexit(report_mem_leak);

    FILE *in = fopen("in.txt", "r");
    FILE *out = fopen("out.txt", "w");

    queue **qPtr = (queue**)malloc(sizeof(queue*) * GROUND);

    int* G = (int*)malloc(sizeof(int));
    fscanf(in, "%d ", G);

    //ALL THE ACTION
    start(in, out, *G, qPtr);
    phase1(out, qPtr);
    phase2(out, qPtr, *G);
    free(G);//NO LONGER USED

    //FREEING QUEUE
    for (int i = 0; i < GROUND; i++) {
        free(qPtr[i]->type);
        free(qPtr[i]);
    }
    free(qPtr);

    //CLOSE OPEN FILES
    fclose(in);
    fclose(out);
    //printf("\n\nREACHED END\n\n");
}

//------------------------------------------------------------------------PHASES

void start(FILE* in, FILE* out, int G, queue** qPtr){
    Node* head = NULL;
    int* location = (int*)malloc(sizeof(int));
    int* numSoldier = (int*)malloc(sizeof(int));
    int* k = (int*)malloc(sizeof(int));
    int* th = (int*)malloc(sizeof(int));
    char* type = (char*)malloc(sizeof(char) * LENGTH);
    char* print = (char*)malloc(sizeof(char) * LENGTH);

    // SORTING INFORMATION BASED ON THEIR LOCATION
    for (int i = 0; i < G; i++) {
        fscanf(in, "%d %s %d %d %d", location, type, numSoldier, k, th);
        head = sortNode(head, location, type, numSoldier, k, th);
    }

    //ALLOCATING SPACE FOR QUEUE
    for (int i = 0; i < GROUND; i++) {
        qPtr[i] = (struct queue *) malloc(sizeof(queue));
        init(qPtr[i]);
    }

    //CREATING SOLDIERS AND LINKING THEM
    Node* temp = head;
    for(int i = 0; i < G; i++){
        qPtr[i] = fillQueue(qPtr[i],temp);
        createReverseCircle(qPtr[i]);
        temp = temp->next;
    }

    printBoth(out, strcpy(print,"Initial nonempty lists status"));
    displayStart(out, qPtr, G);

    for(int i = 0; i < G; i++)//REARRANGING EACH ELEMENT
        rearrangeCircle(qPtr[i]);

    printBoth(out, strcpy(print, "\nAfter ordering nonempty lists status"));
    displayStart(out , qPtr, G);

    //FREEING NODE
    while(head){
        temp = head;
        head = head->next;
        free(temp->Q->type);
        free(temp->Q);
        free(temp);
    }

    free(print);
    free(type);
    free(th);
    free(k);
    free(numSoldier);
    free(location);

    //printf("\nReached end of Start\n\n\n");
}

void phase1(FILE* out, queue** q){
    int* temp = (int*)malloc(sizeof(int));
    char* print = (char*)malloc(sizeof(char) * LENGTH);

    printBoth(out, strcpy(print, "\nPhase1 execution\n"));
    for(int i = 0; i < GROUND; i++, *temp = 0){
        if(isEmpty(q[i])) continue;

        fprintf(out, "Line# %d %s\n", q[i]->loc, q[i]->type);
        printf("Line# %d %s\n", q[i]->loc, q[i]->type);

        for(int j = q[i]->n; j > q[i]->th; j--){//MOVING QUEUE AND DEQUEUEing
            *temp = shift(q[i], q[i]->k - 1);
            q[i]->n--;
            fprintf(out, "Soldier# %d executed\n", *temp);
            printf("Soldier# %d executed\n", *temp);
        }

        printBoth(out, strcpy(print, ""));
    }

    //SORTING QUEUE AFTER EXECUTION
    for (int i = 0; i < GROUND; i++) {
        if (isEmpty(q[i])) continue;
        int low = 999;
        soldier* temp = q[i]->head;
        for(int j = 0; j < q[i]->n; j++){//GETTING LOWEST VALUE
            if(temp->data < low)
                low = temp->data;
            temp = temp->next;
        }
        while (q[i]->head->data > q[i]->head->next->data || q[i]->head->data != low){
            q[i]->head = q[i]->head->next;
        }
        q[i]->tail = q[i]->head->prev;
    }

    free(temp);
    free(print);

    //printf("\nReached end of Phase 1\n\n\n");
}

void phase2(FILE* out, queue** q, int G){

    //displayStart(out,q,G);

    int max = 0;
    int total = 0;
    fprintf(out, "Phase2 execution\n");
    printf("Phase2 execution\n");

    for (int i = 0; i < G; i++) {
        total += q[i]->n;
    }

    for (int i = 0; i < G; i++, max = 0) {
        if (isEmpty(q[i])) break;

        int high = -1;
        int store = 0;
        for(int j = 0; j < G; j++){
            if (isEmpty(q[j])) continue;
            if(peek(q[j]) > high && q[j]->soldierCount != 0){
                high = q[j]->head->data;
                store = j;
            }
        }

        if(high != -1){
            max = store;
        }

        else {
            for (int j = 0; j < G; j++) {//SEARCHING FOR NODE WITH HIGHEST SOLDIER COUNT
                if (isEmpty(q[j])) break;

                if (q[i]->soldierCount > q[j]->soldierCount){
                    if (q[i]->soldierCount > q[max]->soldierCount)
                        max = i;
                }
                else if (q[j]->soldierCount > q[i]->soldierCount){
                    if(q[j]->soldierCount > q[max]->soldierCount){
                        max = j;
                    }
                }
            }
        }

        for (int k = 0; k < q[max]->n; k++) {
            if (total == 1) {
                int survivor = deQueue(q[max]);
                fprintf(out, "\nSoldier %d from line %d will survive", survivor, q[max]->loc);
                printf("\nSoldier %d from line %d will survive", survivor, q[max]->loc);
                break;
            }
            int executed = deQueue(q[max]);
            fprintf(out, "Executed Soldier %d from line %d\n", executed, q[max]->loc);
            printf("Executed Soldier %d from line %d\n", executed, q[max]->loc);
            total--;
            q[max]->soldierCount = 0;
        }
    }
}

//----------------------------------------------------------------------REQUIRED

int peek(queue* q){
    return q->head->data;
}

int isEmpty(queue *qPtr){
    if(qPtr->head == NULL)//EMPTY!!!
        return 1;
    return 0;
}

void enQueue(queue* qPtr, int n){
    soldier* temp = createSoldier(n);

    if(!qPtr->head){//NOTHING IN THE LIST YET
        qPtr->head = temp;
        qPtr->head->next = qPtr->head;
        qPtr->head->prev = qPtr->tail;
        qPtr->tail = qPtr->head;
    }

    else{
        temp->next = qPtr->head;
        temp->prev = qPtr->tail;
        qPtr->head->prev = temp;
        qPtr->tail->next = temp;
        qPtr->tail = temp;
    }
}

int deQueue(queue* q){
    int* temp = (int*)malloc(sizeof(int));
    soldier* s;
    soldier* del;

    *temp = q->head->data;//SOLDIER NUMBER TO DELETE
    s = q->head;
    s->prev->next = s->next;
    s->next->prev = s->prev;

    del = q->head;//STORING OLD HEAD

    q->head = q->head->next;
    q->tail = q->head->prev;
    if(!q->head) q->tail = NULL;

    free(del);

    return *temp;//SOLDIER NUMBER
}

soldier* createSoldier(int sequence){
    soldier* heishi = (soldier*)malloc(sizeof(soldier));

    heishi->data = sequence;//ASSIGNS THE SOLDIER A SPECIFIED 'n' VALUE FROM createReverseCircle lOOP
    heishi->next=NULL;
    heishi->prev=NULL;

    return heishi;
}

void createReverseCircle(queue *q){
    int n = q->soldierCount;

    for(int i = 0; i < GROUND ; i++, n--)//DROPPING SOLDIER COUNT AND ADDING 'n' TO THE LIST
        if(n > 0)
            enQueue(q,n);
}

void rearrangeCircle(queue* q){
    soldier* s;
    soldier* temp;

    s = q->head;
    do {//DO-WHILE NEEDED IN CASE THE CURRENT SOLDIER IS ALREADY == TO q->head
        temp = s->prev;
        s->prev = s->next;
        s->next = temp;
        s = s->next;
    } while(s != q->head);

    //REARRANGES FINAL SOLDIER
    temp = q->head;
    q->head = q->tail;
    q->tail = temp;
}

void display(FILE* out, queue q){
    soldier *start = q.head;//CHECKS FOR A FULL LOOP

    printf("%d %s %d", q.loc, q.type, q.head->data);
    fprintf(out, "%d %s %d", q.loc, q.type, q.head->data);
    q.head = q.head->next;//MOVING OFF THE HEAD FOR WHILE LOOP

    while(start != q.head){
        printf(" %d", q.head->data);
        fprintf(out, " %d", q.head->data);
        q.head = q.head->next;
    }

    printf("\n");
    fprintf(out, "\n");
}

//------------------------------------------------------------------INITIALIZING

void init(queue* q){
    q->head = NULL;
    q->tail = NULL;
}

Node *sortNode(Node* head, int* location, char* type, int* numSoldier, int* k, int* th){
    Node* store = (Node*)malloc(sizeof(Node));
    Node* t;

    store = fillNode(store, location, type, numSoldier, k, th);

    if(!head || head->info >= *location){//NOTHING OR FIRST
        store->next = head;
        head = store;
    }

    else{
        t = head;
        while(t->next && t->next->info < *location)
            t = t->next;
        store->next = t->next;
        t->next = store;
    }

    return head;
}

Node* fillNode(Node* store, int* location, char* type, int* numSoldier, int* k, int* th){
    store->Q = (queue*)malloc(sizeof(queue));
    store->Q->type = (char*)malloc(sizeof(char) * LENGTH);

    store->next = NULL;
    store->info = *location;

    //FILLING NODE WITH COLLECTED DATA
    store->Q->loc = *location;
    store->Q->n = *numSoldier;
    store->Q->soldierCount = *numSoldier;
    store->Q->k = *k;
    store->Q->th = *th;
    store->Q->type = strcpy(store->Q->type, type);

    return store;
}

queue* fillQueue(queue* qPtr, Node* head){
    // qPtr = (queue*)malloc(sizeof(queue));
    // init(qPtr);

    //FILLING QUEUE WITH NODE DATA
    qPtr->loc = head->Q->loc;
    qPtr->soldierCount = head->Q->soldierCount;
    qPtr->n = head->Q->n;
    qPtr->k = head->Q->k;
    qPtr->th = head->Q->th;

    qPtr->type = (char*)malloc(sizeof(char) * LENGTH);
    strcpy(qPtr->type, head->Q->type);
    //qPtr->type = head->Q->type;

    return qPtr;
}

//-----------------------------------------------------------------------UTILITY

int shift(queue* q, int k){
    int* temp = (int*)malloc(sizeof(int));
    soldier* s;
    soldier *swapper;

    if(k != 0){//ROTATING THE QUEUE
        s = q->head;
        *temp = 0;
        while (*temp < k - 1) {
            s = s->next;
            *temp = *temp + 1;
        }//
        swapper =  s;
        while (s->next != q->head)
            s = s->next;
        s->next = q->head;
        q->head = swapper->next;
        swapper->next = q->head;
    }

    free(temp);

    return deQueue(q);
}

void printBoth(FILE* out, char* str){
    printf("%s\n", str);
    fprintf(out, "%s\n", str);
}

void displayStart(FILE* out, queue** qPtr, int n){
    for(int i = 0; i < n; i++){
        display(out, *qPtr[i]);
    }
}

