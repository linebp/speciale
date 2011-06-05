#include <stdlib.h>

#include "nfa.h"

struct Range *
range(unsigned int lo, unsigned int hi)
{
  struct Range *r;

  if ( (r = (struct Range *) malloc(sizeof(struct Range))) == NULL ) {
    fprintf(stderr, "Error allocating memory for range");    
    exit(1);
  }

  r->lo = lo;
  r->hi = hi;
  r->next = NULL;

  return r;
}


struct State *
state(unsigned int c, unsigned int type, struct State *s0, struct State *s1) 
{
  struct State *s;

  if ( (s = (struct State *) malloc(sizeof(struct State))) == NULL ) {
    fprintf(stderr, "Error allocating memory for NFA state");    
    exit(1);
  }

  s->c = c;
  s->type = type;
  s->is_negated = false;
  s->out0 = s0;
  s->out1 = s1;
  s->is_seen = false;
#ifdef END_SPLIT_MARKER
  s->parencount = 0;
  s->subtype = NONE;
#endif  
  s->n = NULL;
  s->laststep = 0;

  return s;  
}


void
range_free(struct Range *r){
  struct Range *tmp;

  while(r != NULL){
    tmp = r->next;
    free(r);
    r = tmp;
  }
}


void
nfa_free(struct State *s)
{

  if(s == NULL || s->is_seen)
    return;

  s->is_seen = true;
  
  switch(s->type){
  case NFA_SPLIT:
    nfa_free(s->out0);
    nfa_free(s->out1);
    break;
  case NFA_ACCEPTING:
    break;
  case NFA_RANGE:
    range_free(s->range);
    //fallthrough
  default:
    nfa_free(s->out0);
    break;
  }

  free(s);
}
