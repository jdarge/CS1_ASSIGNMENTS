//This code shows performing various operations on linked list recursively.
// insert, delete, free, display, copy

//#pragma ide diagnostic ignored "cert-err34-c"

#include <stdio.h>
#include <stdlib.h>
#include "leak_detector_c.h"
//#include "leak_detector_c.c"

struct node {
    int data;
    struct node *next;
};

struct node *insert(struct node *list, int item);//
struct node *del(struct node *list, int item);//
void print(struct node *list);//
void freeList(struct node *list);//
void copy(struct node *q, struct node **s);//

int main() {

    atexit(report_mem_leak); //for leak detector
    int number = 0, choice = 0;
    struct node *pList = NULL;
    struct node *nList = NULL;

    // Let the user add values until they enter -1.
    while (choice != 4) {
        // Get the operation.
        printf("\nDo you want to (1)insert, (2)delete, (3)Copy (4)quit.\n");
        scanf("%d", &choice);

        printf("Your choice is %d\n", choice);

        // Execute the operation.
        if (choice == 1) {
            // Get the number.
            printf("Enter the value to insert\n");
            scanf("%d", &number);
            pList = insert(pList, number);
            // Look at the list.
            printf("Items in linked list: ");
            print(pList);
            //printf("\n");
        } else if (choice == 2) {    // Get the number.
            printf("Enter the value to delete.\n");
            scanf("%d", &number);
            pList = del(pList, number);
            // Look at the list.
            printf("Items in linked list: ");
            print(pList);
            //printf("\n");
        } else if (choice == 3) {
            if (nList)
                freeList(nList);

            copy(pList, &nList); //passing reference of nList as it is not returning anything
            // Look at the list.
            printf("Items in NEW linked list: ");
            print(nList);
            // printf("\n");


        } else {
            break;

        }

    }
    freeList(nList);
    freeList(pList);
    printf("\nBye..\n");
    return 0;
}

void copy(struct node *q, struct node **s) {

    if (q) {
        *s = (struct node *) malloc(sizeof(struct node));
        (*s)->data = q->data;
        (*s)->next = NULL;
        copy(q->next, &((*s)->next));
    }

}

struct node *insert(struct node *list, int item) {

    if (!list || item <= list->data) {
        struct node *pNew = (struct node *) malloc(sizeof(struct node));
        pNew->data = item;
        pNew->next = list;
        return pNew;
    }

    list->next = insert(list->next, item);

    return list;
}

struct node *del(struct node *list, int item) {

    if (!list) {
        return NULL;
    }

    if (list->data == item) {
        struct node *rest = list->next;
        free(list);
        return rest;
    }

    list->next = del(list->next, item);

    return list;
}

void print(struct node *list) {

    if (list) {
        printf("->%d", list->data);
        print(list->next);
    }

}

void freeList(struct node* list){
    
    if(list){
      freeList(list->next);
      free(list);
    }
    
}

