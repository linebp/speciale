#include "util.h"
#include <stdio.h>


#define BUFSIZE 1024

void
writebuf_init(int size)
{
#if defined(NORMAL) || defined(UNLOCKED)
  /* if((writebuf.buffer = (char *) malloc(BUFSIZE)) == NULL){ */
  /*   fprintf(stderr, "Error allocating memory for read buffer\n"); */
  /*   exit(1); */
  /* } */
  /* setvbuf(stdout, writebuf.buffer, _IOFBF, BUFSIZE); */
#endif
#ifdef BUFFERED
  if((writebuf.buffer = (char *) malloc(size)) == NULL){
    fprintf(stderr, "Error allocating memory for write buffer\n");
    exit(1);
  }
  writebuf.size = size;
  writebuf.count = 0;
  writebuf.is_finished = false;
#endif
}


void
writebuf_free()
{
#if defined(NORMAL) || defined(UNLOCKED)
  //free(writebuf.buffer);
#endif
#ifdef BUFFERED
  free(writebuf.buffer);
#endif
}


void
writebuf_flush(FILE *stream)
{
#ifdef BUFFERED
  writebuf.buffer[writebuf.count] = 0;
  fputs(writebuf.buffer, stream);
  writebuf.count = 0;
#endif
}


void
readbuf_init(int size)
{
#if defined(NORMAL) || defined(UNLOCKED)
  /* if((readbuf.buffer = (char *) malloc(BUFSIZE)) == NULL){ */
  /*   fprintf(stderr, "Error allocating memory for read buffer\n"); */
  /*   exit(1); */
  /* } */
  /* setvbuf(stdin, readbuf.buffer, _IOFBF, BUFSIZE); */
#endif
#ifdef BUFFERED
  if((readbuf.buffer = (char *) malloc(size)) == NULL){
    fprintf(stderr, "Error allocating memory for read buffer\n");
    exit(1);
  }
  readbuf.size = size;
  readbuf.count = size;
#endif
}


void
readbuf_free()
{
#if defined(NORMAL) || defined(UNLOCKED)
  //free(readbuf.buffer);
#endif
#ifdef BUFFERED
  free(readbuf.buffer);
#endif
}


void 
write_bit(char bit, FILE *stream){
#ifdef UNLOCKED
  fputc_unlocked(bit, stream);
#endif
#ifdef NORMAL
  fputc(bit, stream);
#endif
#ifdef BUFFERED
  if(writebuf.count >= (writebuf.size - 1)){
    writebuf.buffer[writebuf.count] = 0;
    fputs(writebuf.buffer, stream);
    writebuf.count = 0;
  }

  writebuf.buffer[writebuf.count++] = bit;
#endif
}


char 
read_bit(FILE *stream){
#ifdef UNLOCKED
  return fgetc_unlocked(stream);
#endif
#ifdef NORMAL
  return fgetc(stream);
#endif
#ifdef BUFFERED
  char *tmp;

  if(writebuf.is_finished){
    if(readbuf.buffer[readbuf.count] == 0)
      return EOF;
    else
      return readbuf.buffer[readbuf.count++];
  }

  if((readbuf.count >= (readbuf.size - 1)) ||
     (readbuf.buffer[readbuf.count] == 0)){
    tmp = fgets(readbuf.buffer, readbuf.size, stream);
    if(tmp == NULL){
      writebuf.is_finished = true;
    } else
      readbuf.count = 0;
  }

  if(writebuf.is_finished && (readbuf.buffer[readbuf.count] == 0))
    return EOF;
  else
    return readbuf.buffer[readbuf.count++];
#endif
}



struct Fragment
fragment(struct State *s, struct Statelist *p, int op) 
{
  struct Fragment f;

  f.start = s;
  f.out = p;
  f.op = op;
#ifdef END_SPLIT_MARKER
  f.parencount = 0;
#endif
  
  return f;
}



/******************************
 * Linked list of pointers
 * to NFA states
 ******************************/

struct Statelist *
ptrlist_list1(struct State **outp)
{
  struct Statelist *p;
  struct Statelist_elem *e;

  if ( (p = (struct Statelist *) malloc(sizeof(struct Statelist))) == NULL ) {
    printf("Error allocating memory for state list");
    exit(1);
  }

  if ( (e = (struct Statelist_elem *) 
	malloc(sizeof(struct Statelist_elem))) == NULL ) {
    printf("Error allocating memory for state list");
    exit(1);
  }
  
  e->outp = outp;
  e->next = NULL;

  p->first = e;
  p->last = e;

  return p;
}

void 
ptrlist_patch(struct Statelist *l, struct State *s) 
{
  struct Statelist_elem *e;
  
  e = l->first;
  while(e != NULL){
    *(e->outp) = s;
    e = e->next;
  }
}

struct Statelist *
ptrlist_append(struct Statelist *l1, struct Statelist *l2) 
{
  if(l1 == NULL)
    return l2;

  if(l1 == l2) {
    return l1;
  }

  l1->last->next = l2->first;
  l1->last = l2->last;
  free(l2);

  return l1;
}

void
ptrlist_free(struct Statelist *s)
{
  struct Statelist_elem *e, *temp;
  
  e = s->first;
  while(e != NULL){
    temp = e;
    e = e->next;
    free(temp);
  }

  free(s);
}


