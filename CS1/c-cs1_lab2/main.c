#include<stdio.h>
#include<stdlib.h>

typedef struct node{
   int data;
   struct node *next;
} node;


node* insert_front(node *root, int item) {// takes an item and insert it in the linked list pointed by root.
   node *temp = (node*)malloc(sizeof(node));
   temp->data = item;
   temp->next = NULL;

   if(root==NULL)
      root = temp;

   else {
      temp->next = root;
      root = temp;
   }

   return root;
}


node* reverse(node* head) {
   if(head == NULL || head->next == NULL)
      return head;

   node* main_list = head->next;
   node* reversed_list = head;
   reversed_list->next = NULL;

   while(main_list != NULL){
      node* temp = main_list;
      main_list = main_list->next;

      temp->next = reversed_list;
      reversed_list = temp;
   }

   return reversed_list;
}


void insertToPlace(node* list, int val, int place) {
   if(list == NULL || place <= 1) {
      printf("\nList is empty or place is not valid.");
      return;
   }

   node* temp = list;
   node* temp2 = (node*)malloc(sizeof(node));
   temp2->data = val;
   temp2->next = NULL;

   for(int i=place-1; i>1; i--){
      if(temp->next==NULL)
         break;
      temp = temp->next;
   }

   temp2->next = temp->next;
   temp->next = temp2;
}


void display(node* t) {
   printf("\nPrinting your linked list.......");
   while(t!=NULL) {
      printf("%d ",t->data);
      t=t->next;
   }
}


int main() {
   node *root=NULL;//   very important line. Otherwise all function will fail
   node *t;
   int ch,ele,v, del;
   while(1) {
      printf("\nMenu: 1. insert at front, 2. reverse list 3. Insert to place 0. exit: ");
      scanf("%d",&ch);
      if(ch==0){
         printf("\nGOOD BYE>>>>\n");
         break;
      }
      if(ch==1) {
         printf("\nEnter data (an integer): ");
         scanf("%d",&ele);
         root = insert_front(root, ele);
         display(root);
      }
      if(ch==2) {
         root = reverse(root);
         printf("List reversed.\n");
         display(root);
      }
      if(ch==3) {
         int place;
         printf("\nEnter data (an integer) and place (>1) separated by space: ");
         scanf("%d %d",&ele, &place);
         insertToPlace(root, ele, place);
         display(root);
      }
   }
   return 0;
}

