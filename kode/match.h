#ifndef __match_h
#define __match_h

#include "nfa.h"

struct List
{
	struct State **s;
	unsigned int n;
};

struct List *list(unsigned int size)
{
  struct List *l;

  if ( (l = (struct List *) malloc(sizeof(struct List ))) == NULL ) {
    fprintf(stderr, "Could not allocate memory for state-list\n");
  }
  if ( (l->s=(struct State **)malloc(sizeof(struct State *)*size))==NULL ) {
    fprintf(stderr, "Could not allocate memory for state-list array\n");
  }
  return l;
}

#endif
