#include <stdio.h>
#include <string.h>
#include "util.h"
#include "nfa.h"
#include "match.h"

unsigned int stepid;


void
addstate(struct List *l, struct State *s, FILE *outstream)
{
  if(s == NULL)
    return;

  if(s->laststep == stepid){
    write_bit('b', outstream);
    return;
  }

  s->laststep = stepid;

  switch(s->type){
  case NFA_SPLIT:
    /* follow unlabeled arrows */
    write_bit('=', outstream);
    write_bit('0', outstream);
    addstate(l, s->out0, outstream);

    write_bit(':', outstream);
    write_bit('1', outstream);
    addstate(l, s->out1, outstream);

    return;
  case NFA_EPSILON:
    addstate(l, s->out0, outstream);
    return;
  }
  
  l->s[l->n] = s;
  l->n++;
  return;
}


enum Boolean
is_in_range(struct Range *r, unsigned int c, enum Boolean is_negated, 
	    FILE *outstream)
{
  struct Range *tmp;

  tmp = r;

  if(!is_negated){
    while(tmp != NULL){
      if(c >= tmp->lo && c <= tmp->hi){
	write_bit('\\', outstream);
	write_bit(c, outstream);
	return true;
      }
      tmp = tmp->next;
    }
    return false;
  }
  else {
    while(tmp!= NULL){
      if(c >= tmp->lo && c <= tmp->hi){
	return false;
      }
      tmp = tmp->next;
    }
    write_bit('\\', outstream);
    write_bit(c, outstream);
    return true;
  }
}


void
step(struct List *clist, unsigned int c, struct List *nlist, FILE *outstream)
{
  unsigned int i;
  struct State *s;
  
  nlist->n = 0;

  for(i = 0; i < clist->n; i++){
    s = clist->s[i];
    if(i != 0)
      write_bit(':', outstream);
    
    if((s->type == NFA_LITERAL && s->c == c) ||
       (s->type == NFA_RANGE && is_in_range(s->range, c, s->is_negated, 
					    outstream)))
      addstate(nlist, s->out0, outstream);
    else
      write_bit('b', outstream);
  }
}


enum Boolean
last_addstate(struct State *s, FILE *outstream)
{
  enum Boolean result, result1;

  if(s == NULL)
    return false;

  if(s->laststep == stepid){
    write_bit('b', outstream);
    return false;
  }

  s->laststep = stepid;

  switch(s->type){
  case NFA_SPLIT:

    /* follow unlabeled arrows */
    write_bit('=', outstream);
    write_bit('0', outstream);
    result = last_addstate(s->out0, outstream);

    write_bit(':', outstream);
    write_bit('1', outstream);
    result1 = last_addstate(s->out1, outstream);

    return result || result1;
  case NFA_EPSILON:
    result = last_addstate(s->out0, outstream);
    return result;
  case NFA_ACCEPTING:
    write_bit('t', outstream);
    return true;
  }

  write_bit('b', outstream);
  return false;
}


enum Boolean
last_step(struct List *clist, unsigned int c, FILE *outstream)
{
  unsigned int i;
  struct State *s;
  enum Boolean result;

  result = false;
  for(i = 0; i < clist->n; i++){
    s = clist->s[i];
    if(i != 0)
      write_bit(':', outstream);
    
    if((s->type == NFA_LITERAL && s->c == c) ||
       (s->type == NFA_RANGE && is_in_range(s->range, c, s->is_negated, 
					    outstream)))
      result = last_addstate(s->out0, outstream) || result;
    else
      write_bit('b', outstream);
  }
  return result;
}


enum Boolean
match(struct NFA nfa, FILE *instream, FILE *outstream)
{
  unsigned int i;
  char cc, cn;
  enum Boolean result;

  struct List *clist, *nlist, *tmp;
  
  clist = list(nfa.statecount);
  clist->n = 0;
  nlist = list(nfa.statecount);
  
  stepid = 1;

  if((cc = read_bit(instream)) == EOF){
    result = last_addstate(nfa.start, outstream);
    return result;
  } 

  addstate(clist, nfa.start, outstream);

  while ( (cn = read_bit(instream)) != EOF ){
    write_bit('|', outstream);
    stepid++;
    step(clist, cc, nlist, outstream);
    tmp = clist; clist = nlist; nlist = tmp;

    cc = cn;
  }

  write_bit('|', outstream);
  stepid++;
  result = last_step(clist, cc, outstream);
  return result;
}
