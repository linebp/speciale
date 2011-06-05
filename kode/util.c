#include "util.h"
#include <stdio.h>
#include <unistd.h>


char *
init_stream(unsigned int buf_size, FILE *stream){
  char *buf = NULL;

  /* if(buf_size == 0){ */
  /*   if(setvbuf(stream, NULL, _IONBF, 0) != 0){ */
  /*     perror("Failed to set unbuffered IO\n"); */
  /*     exit(1); */
  /*   } */
  /*   buf = NULL; */
  /* } */
  /* else { */
  /*   if((buf = malloc(buf_size)) == NULL){ */
  /*     perror("Can not allocate memory for IO buffering\n"); */
  /*     exit(1); */
  /*   } */

  /*   if(setvbuf(stream, buf, _IOFBF, buf_size) != 0){ */
  /*     perror("Failed to set buffer for IO\n"); */
  /*     exit(1); */
  /*   } */
  /* } */

  return buf;
}

void
close_stream(char *buf, FILE *stream){
  fclose(stream);
  free(buf);
}


void 
write_bit(char bit, FILE *stream){
  fputc(bit, stream);
}


char 
read_bit(FILE *stream){
  return fgetc(stream);
}



int
read_file(char *filename, char **buf)
{
  FILE *fp;
  int buf_size = 1024;
  ssize_t bytes_read = 0;

  if((fp = fopen(filename, "r")) == NULL){
    perror("Can not open file for reading in read_file\n");
    exit(1);
  }

  if((*buf = malloc(buf_size)) == NULL){
    perror("Can not allocate memory for data in read_file\n");
    exit(1);
  }
  
  while(true){
    bytes_read += read(fileno(fp), (*buf) + bytes_read, buf_size - bytes_read);
    if(bytes_read == -1){
      perror("Could not read data from file in read_file\n");
      exit(1);
    }
    if(bytes_read >= buf_size){
      buf_size = buf_size*2;
      if((*buf = realloc(*buf, buf_size)) == NULL){
	perror("Can not reallocate memory for data in read_file\n");
	exit(1);
      }
    } else {
      break;
    }
  }
  return bytes_read;
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


