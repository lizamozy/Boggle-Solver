#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "llist.h"

//initialize a linked list
llist_t * ll_init(){

//make a pointer to new linked list 
    
    llist_t * new_ll = malloc(sizeof(llist_t));
    new_ll -> head = NULL;
    new_ll -> size = 0;
  return new_ll; //clearly it shouldn't return null when you finish it
}

//delete a linked list
void ll_delete(llist_t * ll){

// make head pointer equal to current iterator and set next equal to NULL.
ll_node_t * cur;
ll_node_t * next;

  cur = ll -> head;
  //iterate through while current is not null
  while(cur != NULL){
    next = cur;
    cur = cur -> next;
    //move next at top of function
    free(next->val);
    free(next); 
    ll -> size--;
        
  }

  free(ll);
}

//push an item onto the front of the list
void ll_push(llist_t * ll, char * s){

    //make temp pointer to point to first node while inserting new
    ll_node_t * temp = malloc(sizeof(ll_node_t));
    temp -> val = strdup(s);
    temp -> next = ll -> head; 
    ll -> head = temp;
    ll -> size++;
  
}

