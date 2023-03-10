#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "llist.h"
#include "hashmap.h"
#include "boggle.h"

//constant values on all the dice in global
const char * boggle_dice[] = {
        "AAAFRS", "AAEEEE", "AAFIRS", "ADENNN", "AEEEEM",
        "AEEGMU", "AEGMNN", "AFIRSY", "BJKQXZ", "CCENST",
        "CEIILT", "CEILPT", "CEIPST", "DDHNOT", "DHHLOR",
        "DHLNOR", "DHLNOR", "EIIITT", "EMOTTT", "ENSSSU",
        "FIPRSY", "GORRVW", "IPRRRY", "NOOTUW", "OOOTTU",
};


//initialize a boggle instance (do not edit until after the TODO: comment)
boggle_t * bg_init(int seed, char * dict_fn){
  //You get this first part of the function for free :)

  //set a random seed
  srandom(seed);

  //create the instance
  boggle_t * bg = calloc(sizeof(boggle_t), 1);

  //setup the board by rolling dice
  int used[BOGGLE_NUM_DICE] = {0};
  for(int i=0;i<BOGGLE_NUM_DICE;i++){
    while(1){
      //choose a dice to roll
      int d = random()%BOGGLE_NUM_DICE;
      if(used[d]) continue; //keep choosing if already rolles

      //choose a random side
      int s = random()%BOGGLE_DICE_SIDES;
      bg->board[i/BOGGLE_DIMENSION][i%BOGGLE_DIMENSION] = boggle_dice[d][s];
      used[d] = 1; //mark that dice as rolled
      break;
    }
  }

  //use initilize hashmap function and then read the dictionary.txt file and use the add function to put those words into the new hashmap

  //initialize dictionary hashmap
  hashmap_t * dictionary = hm_init();
  bg -> dict = dictionary;

  //open the txt file and read in the words
  FILE * stream = fopen(dict_fn, "r");

  //make a new char array of strings to hold the value of the words
  char added_word[1024];

  // while loop to go through and add in each word into the dictionary hashmap
  while(fscanf(stream, "%s", added_word) != EOF){
    //add the words into the hashmap using add funciton 
    for (char *move = added_word; (*move = toupper(*move)) != 0; move++){
      if(strlen(added_word) >=3 && strlen(added_word) <=8) {
        hm_add(dictionary, added_word);
      }
    }
  }


  fclose(stream);

  return bg;
}

//delete/free the boggle instance (do not edit)
void bg_delete(boggle_t * bg){
  //You get this function for free :)
  hm_delete(bg->dict); 
  free(bg);
}


//print the board using fancy unicode (do not edit)
void bg_print(boggle_t * bg){
  printf(".-----------.\n");
  for (int r = 0; r < 5; r++) {
    printf("| ");
    for (int c = 0; c < 5; c++) {
      printf("%c",bg->board[r][c]);      
      if (bg->board[r][c]=='Q')
	printf("u");
      else
	printf(" ");
    }
    printf("|\n");
  }
  printf("'-----------'\n");
}


//count the points up (do not edit)
// 3 and 4 length words are worth 1 point
// 5 length worth 2
// 6 length worth 3
// 7 length worth 5
// 8 length worth 11
// note we stop searching at 8 length words 
int bg_count_points(boggle_t * bg, llist_t * words){
  int pts=0;
  for(ll_node_t * n = words->head; n; n = n->next){
    if(hm_check(bg->dict,n->val)){
      switch(strlen(n->val)){
      case 0:
      case 1:
      case 2: break;
      case 3:
      case 4:
	pts+=1;
	break;
      case 5:
	pts+=2;
	break;
      case 6:
	pts+=3;
	break;
      case 7:
	pts+=5;
	break;
      default:
	pts+=11;
      }
    }
  }
  return pts; 
}

//helper functions for the recursion, declaring here so can call below
void _bg_all_words(boggle_t * bg, //boggle instance
		   hashmap_t * hm, //hashmap to track found words
		   int r, int c, //current row (r) / column (c)
		   char * last_word, //what the last word was
		   int last_visited[BOGGLE_DIMENSION][BOGGLE_DIMENSION] //what has been previously visited
		                                                        //don't want to double back
		   );

//entry function for finding all the words
llist_t * bg_all_words(boggle_t *bg){

  //0'ed out -- currently visited nowhere (all 0's), and the current word is the empty string
  int visited[BOGGLE_DIMENSION][BOGGLE_DIMENSION] = {0};
  char cur_word[9] = {0}; //empty string 
  // Note using byte array of length 9 b/c max word length is 8!

  //initialize a hashmap to track status
  hashmap_t * hm = hm_init();

  //start a recurssion at every space on the board
  for(int i=0;i<BOGGLE_DIMENSION;i++){
    for(int j=0;j<BOGGLE_DIMENSION;j++){
      _bg_all_words(bg, hm, i, j, cur_word, visited);
    }
  }

  //one linked list for all of the valid words that were created in recurievse function if they are also found in dictionary
  llist_t * found_words = ll_init();

  for(int i=0; i < hm -> num_buckets; i++){
      if(hm->buckets[i] != NULL){
      ll_node_t * cur = hm -> buckets[i] -> head;
          while(cur != NULL){
            ll_push(found_words, cur -> val);
            cur = cur -> next;

          }
      }
      
  }

  hm_delete(hm); //delete the hashmap

  //return the list of words
  return found_words; //clearly, this won't be null when your done.
}

void _bg_all_words(boggle_t *bg, hashmap_t * hm, int r, int c,
		   char * last_word, int last_visited[BOGGLE_DIMENSION][BOGGLE_DIMENSION]){

  char cur_word[9] = {0}; //use to store the current word
  int visited[BOGGLE_DIMENSION][BOGGLE_DIMENSION] = {0};


  if(strlen(last_word) > 8){
    return;
  }

  //checking that it is not off the board
  if((r>=5 || r<0 )||(c>=5 || c<0)){
    return;
  }

  //check if we have already visited this dice
  if(last_visited[r][c] == 1){
    return;
  }

  for(int i=0; i < BOGGLE_DIMENSION; i++){
      for(int j=0; j < BOGGLE_DIMENSION; j++){
          visited[i][j] = last_visited[i][j];
      }
  }

  //string copy to make cur_word last_word
  strcpy(cur_word, last_word);

  //add the letter in
  cur_word[strlen(cur_word)] = bg -> board[r][c];


  if(bg -> board[r][c] == 'Q' && strlen(last_word) < 8){
      cur_word[strlen(cur_word)] = 'U';
   }
 
  cur_word[strlen(cur_word)] = '\0';
  
  //checking if it is a word in the dictionary 
  //checking size of the word 
    //put this after we apppend 
    
    if(strlen(cur_word) >= 3 ){
      if(hm_check(bg -> dict, cur_word) ){
        if(!hm_check(hm, cur_word)){
          hm_add(hm, cur_word);
        }
      }
    }
    //update visited
    visited[r][c]++;
 
  
  //Perform 8 recursive calls on all surrounding dice...
  _bg_all_words(bg,hm,r-1,c,cur_word,visited);
  _bg_all_words(bg,hm,r-1,c-1,cur_word,visited);
  _bg_all_words(bg,hm,r-1,c+1,cur_word,visited);
  _bg_all_words(bg,hm,r+1,c,cur_word,visited);
  _bg_all_words(bg,hm,r+1,c-1,cur_word,visited);
  _bg_all_words(bg,hm,r+1,c+1,cur_word,visited);
  _bg_all_words(bg,hm,r,c-1,cur_word,visited);
  _bg_all_words(bg,hm,r,c+1,cur_word,visited);

}
