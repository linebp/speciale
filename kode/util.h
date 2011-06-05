#ifndef __util_h
#define __util_h

#include <stdlib.h>
#include <graphviz/gvc.h>

enum Boolean 
{
    false,     /* false = 0, true = 1 */
    true 
}; 


int read_file(char *filename, char **buf);

char *init_stream(unsigned int buf_size, FILE *stream);
void close_stream(char *buf, FILE *stream);


void write_bit(char bit, FILE *stream);
char read_bit(FILE *stream);


/******************************
 * Operators
 ******************************/

#define OP_NO 0
#define OP_ALTERNATE 258
#define OP_LEFT_PAREN 259
#define OP_LEFT_CAPT_PAREN 260

/******************************
 * Linked list of pointers
 * to NFA states
 ******************************/

struct Statelist_elem
{
  struct State **outp;
  struct Statelist_elem *next;
};

struct Statelist 
{
  struct Statelist_elem *first;
  struct Statelist_elem *last;
};

struct Statelist *ptrlist_list1(struct State **outp);

// Patches up the dangling pointers in l so they point to s
void ptrlist_patch(struct Statelist *l, struct State *s);

struct Statelist *ptrlist_append(struct Statelist *l1, struct Statelist *l2);

void ptrlist_free(struct Statelist *l);

/******************************
 * NFA fragments
 ******************************/

struct Fragment 
{
  unsigned int op;
#ifdef END_SPLIT_MARKER
  unsigned int parencount;
#endif

  struct State *start;
  struct Statelist *out;
};

struct Fragment fragment(struct State *s, struct Statelist *p, int op);

/******************************
 * Stack of NFA fragments
 ******************************/

#endif
