#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "util.h"

#define DEFAULT_BIT_SIZE 100
#define DEFAULT_CHANNEL_COUNT 100


struct Channel {
  char *bits;
  char *b;
  unsigned int size;
  
  struct Channel *next;
};


struct Channel *
channel(){
  struct Channel *new;

  if ((new = (struct Channel *) malloc(sizeof(struct Channel))) == NULL ) {
    printf("Error allocating memory for channel\n");    
    exit(1);
  }

  if ((new->bits = (char *) malloc(sizeof(char)*DEFAULT_BIT_SIZE)) == NULL ) {
    printf("Error allocating memory for bits\n");    
    exit(1);
  }

  new->size = DEFAULT_BIT_SIZE;
  new->b = new->bits;
  new->next = NULL;

  return new;
}

struct Channel *
channel_copy(struct Channel *old){
  struct Channel *new;
  unsigned int size;

  if ((new = (struct Channel *) malloc(sizeof(struct Channel))) == NULL ) {
    printf("Error allocating memory for channel\n");    
    exit(1);
  }

  if ((new->bits = (char *) malloc(sizeof(char)*DEFAULT_BIT_SIZE)) == NULL ) {
    printf("Error allocating memory for bits\n");    
    exit(1);
  }
  
  size = old->b - old->bits;
  memcpy(new->bits, old->bits, size);

  new->size = old->size;
  new->b = new->bits+size;
  new->next = old->next;

  return new;
}


void
channel_free(struct Channel *c){
  if(c != NULL){
    free(c->bits);
    free(c);
  }
}


void
channel_write_bit(struct Channel *c, char bit)
{
  // Should also extend bits array if to small!
  *c->b++ = bit;
}


// Inserts element cur in list nlist, where new points to the last 
// element. If cur is NULL, nothing happens
void
channel_append(struct Channel **nlist, struct Channel **new, 
		struct Channel **cur)
{
  if(*cur == NULL)
    return;

  // Insert element in empty list
  if(*new == NULL){
    *nlist = *cur;
    *new = *cur;
    (*cur)->next = NULL;
  } else {
    // Insert element in non-empty list
    (*cur)->next = NULL;
    (*new)->next = *cur;
    *new = *cur;
  }

}


// Inserts element cur in list nlist, where new points to the last 
// element and advances cur to next. If cur is NULL, then nothing happens
void
channel_swap(struct Channel **nlist, struct Channel **new, 
	     struct Channel **cur){
  struct Channel *tmp;

  if(*cur == NULL)
    return;

  tmp = *cur;
  channel_write_bit(tmp, '|');

  // Advance old list 
  *cur = (*cur)->next;
  channel_append(nlist, new, &tmp);
}


void
merge_and_print(struct Channel *mlist)
{
  struct Channel *match;
  enum Boolean not_finished;
  enum Boolean is_first;

  is_first = true;
  not_finished = false;
  for(match = mlist; match != NULL; match = match->next){
    if(is_first)
      is_first = false;
    else
      putc(':', stdout);
    
    // Should we split the channel?
    if(match->next != NULL){
      putc('=', stdout);
    }

    match->b = 0;
    for(match->b = match->bits; *match->b != '|' && *match->b != 0; 
	match->b++){
      putc(*match->b, stdout);
    }
    match->b++;      
    if(*(match->b) != 0)
      not_finished = true;
  }
  
  while(not_finished){
    putc('|', stdout);
    is_first = true;
    for(match = mlist; match != NULL; match = match->next){
      if(is_first)
	is_first = false;
      else
	putc(':', stdout);
      for(; *match->b != '|' && *match->b != 0; match->b++){
	putc(*match->b, stdout);
      }
      match->b++;
      if(*match->b == 0){
	putc('t', stdout);
	not_finished = false;
      }
    }
  }
}


int 
main(void) 
{
  char c;
  // b indicates whether channels should be swapped on channel change
  // (some meta characters, like t and b, cause an automatic channel swap)
  int b;
  // Channels for next iteration is stored in nlist, where
  // nlist is the head of the list and new is the last element
  // matches are stored in the mlist, where 
  // mlist is the head of the list and match is the last element
  struct Channel *nlist, *new, *tmp, *cur, *mlist, *match;

  b = 0;

  nlist = NULL;
  new = NULL;
  mlist = NULL;
  match = NULL;
  cur = channel();

  while ( (c = getchar()) != EOF ){
    //printf("received char: %c\n", c);
    switch(c){
    case '|':
      if(b == 0)
	channel_swap(&nlist, &new, &cur);
      else
	b = 0;

      if(cur != NULL){
	fprintf(stderr, "Error: Channel corruption (%i)\n", __LINE__);
	exit(0);
      } 

      cur = nlist; 
      nlist = NULL;
      new = NULL;
      break;

    case ':':
      if(b == 0)
	channel_swap(&nlist, &new, &cur);
      else
	b = 0;
      break;

    case '=':
      b = 0;
      if(cur == NULL){
	fprintf(stderr, "Error: Channel corruption (%i)\n", __LINE__);
	exit(0);
      } 
      tmp = channel_copy(cur);
      cur->next = tmp;
      break;

    /* case '>': */
    /*   b = 0; */
    /*   channel_append(&nlist, &new, &cur); */
    /*   cur = channel(); */
    /*   break; */

    case 't':
      b = 1;
      if(cur == NULL){
	fprintf(stderr, "Error: Channel corruption (%i)\n", __LINE__);
	exit(0);
      } 
      channel_swap(&mlist, &match, &cur);
      break;

    case 'b':
      b = 1;

      if(cur == NULL){
	fprintf(stderr, "Error: Channel corruption (%i)\n", __LINE__);
	exit(0);
      } 

      tmp = cur;
      cur = cur->next;
      channel_free(tmp);
      break;

    case '0':
      b = 0;
      if(cur == NULL){
	fprintf(stderr, "Error: Channel corruption (%i)\n", __LINE__);
	exit(0);
      } 

      channel_write_bit(cur, '0');
      break;

    case '1':
      b = 0;
      if(cur == NULL){
	fprintf(stderr, "Error: Channel corruption (%i)\n", __LINE__);
	exit(0);
      } 

      channel_write_bit(cur, '1');
      break;

    case '\\':
      c = getchar();
      if(c == EOF){
	fprintf(stderr, "Error: Bad escape (%i)\n", __LINE__);
      }
      channel_write_bit(cur, '\\');
      channel_write_bit(cur, c);
      break;

    default:
      fprintf(stderr, "Error: Bad character (%i)\n", __LINE__);
      exit(0);
    }
  }
  merge_and_print(mlist);
  return 0;
}
