#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <math.h>
#include <string.h>

#include "llist.h"
#include "hashmap.h"

//local function for hashing an input value 
unsigned int _hash(hashmap_t * hm, char * val){
  //based on Java's String.hashcode()
  double k = 0;
  double len = (double) strlen(val);
  int i=1;
  for(char * c = val; *c ; c++,i++){
    k += (*c) * pow(31,len-i);
  }

  return (unsigned int) (((unsigned long) k) % hm->num_buckets);
}


//local function: resize the hashmap by doubling the number of buckets and rehashing
void _resize(hashmap_t * hm){

  // maintain a pointer to old buckets 
  llist_t ** old_buckets = hm->buckets;
  ll_node_t * cur;

  int new_size = hm->num_buckets * 2;

  hm -> buckets = calloc(sizeof(llist_t *), new_size);
  hm -> num_buckets = new_size;
  hm -> size = 0;

  for(int i=0; i < new_size; i++){
      hm -> buckets[i] = ll_init();
  }

  // for loop to go through old buckets 
  for(int i=0; i < hm->num_buckets/2; i++ ){

    // get the head of the list at i
    cur = old_buckets[i] -> head;

     while(cur != NULL){
      //add pushes and hashes correct value 
      hm_add(hm, cur -> val);
      // move node forward
      cur = cur -> next;

     }
     //you can free the old linked list in after the while loop but still in the for-loop
     ll_delete(old_buckets[i]);
  }

  free(old_buckets);
 
}

//initliaze a hashmap with initial number of buckets
hashmap_t * hm_init(){

  //create the hashmap
  hashmap_t * hm = calloc(sizeof(hashmap_t),1);

    //What do i need to make num_buckets point to
  hm -> buckets = calloc(sizeof(llist_t *), HM_INIT_NUM_BUCKETS);
  hm -> num_buckets = HM_INIT_NUM_BUCKETS;

  for(int i=0; i < HM_INIT_NUM_BUCKETS; i++){
    hm -> buckets[i] = ll_init();
  }
// if errors with size make size 0
  return hm;
}

//delete/deallocate the hashmap
void hm_delete(hashmap_t * hm){
  
  //make pointer to buckets

  for(int i=0; i < hm -> num_buckets; i++){
      ll_delete(hm -> buckets[i]);
  }
  //delete the hashmap
  free(hm -> buckets);
  free(hm);
}

//add a string value to the hashmap
void hm_add(hashmap_t * hm, char * val){
  // You get this function for free :)
  
  //check the load on the hashtable, if > max load, resize!
  if(((hm->size+1)/(double) hm->num_buckets) > HM_MAX_LOAD){
    _resize(hm); 
  }

  unsigned int h = _hash(hm,val); //get hash
  ll_push(hm->buckets[h],val); //note that list does string dup
  hm->size++; //increase size

}

//see if a string value is in the hashmap
bool hm_check(hashmap_t * hm, char * val){

  ll_node_t * cur;

  //hash and then find correct bucket and traverse through linked list to find the value in the while loop. if we dont find it exit and return false
  unsigned int hashvalue = _hash(hm, val);

  cur = hm -> buckets[hashvalue] -> head;

  while(cur != NULL){
    if (strcmp(cur -> val, val)==0){
        return true;
    } 
    cur = cur -> next;
  }

  return false; //otherwise false and we did not find the value
}
