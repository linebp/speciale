#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "util.h"
#include "nfa.h"

#ifdef END_SPLIT_MARKER
unsigned int parendepth;
#endif

// Concatenate the top 2 fragments on the stack if possible
void
maybe_concat(struct Fragment **stackp, struct Fragment *stack)
{
  struct Fragment e1, e2;
  if(*stackp - stack >= 2 && (*stackp)[-1].op == OP_NO 
                          && (*stackp)[-2].op == OP_NO){
    e2 = *--(*stackp);
    e1 = *--(*stackp);
    ptrlist_patch(e1.out, e2.start);
    ptrlist_free(e1.out);
    *(*stackp)++ = fragment(e1.start, e2.out, OP_NO);
#ifdef END_SPLIT_MARKER
    (*stackp)[-1].parencount = e1.parencount + e2.parencount;
#endif
  }
}


// Alternate top fragments if possible
// Returns number of new states created
unsigned int
maybe_alternate(struct Fragment **stackp, struct Fragment *stack)
{
  struct State *s;
#ifdef END_SPLIT_MARKER
  struct State *s1;
#endif
  struct Fragment e1, e2;

  if(*stackp - stack >= 3 &&
     (*stackp)[-1].op == OP_NO && 
     (*stackp)[-2].op == OP_ALTERNATE && 
     (*stackp)[-3].op == OP_NO){

    e2 = *--(*stackp);
    // Just pop the alternate marker, no need to look at it
    --(*stackp);
    e1 = *--(*stackp);
    s = state(0, NFA_SPLIT, e1.start, e2.start);
#ifdef END_SPLIT_MARKER
    s->parencount = e1.parencount;

    s1 = state(0, NFA_EPSILON, NULL, NULL);
    s1->subtype = END_SPLIT;
    s1->parencount = e2.parencount;
    ptrlist_patch(e1.out, s1);

    *(*stackp)++ = fragment(s, ptrlist_append(e2.out, 
					      ptrlist_list1(&s1->out0)), 
			    OP_NO);
    (*stackp)[-1].parencount = e1.parencount + e2.parencount;
    return 2;
#else
    *(*stackp)++ = fragment(s, ptrlist_append(e2.out, e1.out), OP_NO);      
    return 1;
#endif
  }
  if(*stackp - stack >= 2){
    if((*stackp)[-1].op == OP_ALTERNATE && 
       (*stackp)[-2].op == OP_NO){
      // Just pop the alternate marker, no need to look at it
      --(*stackp);
      e1 = *--(*stackp);
    
      s = state(0, NFA_SPLIT, e1.start, NULL);

      *(*stackp)++ = fragment(s, 
			      ptrlist_append(e1.out, ptrlist_list1(&s->out1)),
			      OP_NO);
#ifdef END_SPLIT_MARKER
      s->parencount = e1.parencount;
      (*stackp)[-1].parencount = e1.parencount;
#endif
      return 1;

    }
    else if((*stackp)[-1].op == OP_NO && 
	    (*stackp)[-2].op == OP_ALTERNATE){
      e1 = *--(*stackp);
      // Just pop the alternate marker, no need to look at it
      --(*stackp);
#ifdef END_SPLIT_MARKER
      if(e1.parencount > 0){
	s1 = state(0, NFA_EPSILON, NULL, NULL);
	s1->subtype = END_SPLIT;
	s1->parencount = e1.parencount;
	s = state(0, NFA_SPLIT, s1, e1.start);
      	*(*stackp)++ = fragment(s, ptrlist_append(e1.out, 
						  ptrlist_list1(&s1->out0)),
				OP_NO);
	(*stackp)[-1].parencount = e1.parencount;
      }
      else {
	s = state(0, NFA_SPLIT, NULL, e1.start);
	*(*stackp)++ = fragment(s, 
				ptrlist_append(e1.out, ptrlist_list1(&s->out0)),
				OP_NO);
	(*stackp)[-1].parencount = e1.parencount;
	
      }
      return 1;
#else
	s = state(0, NFA_SPLIT, NULL, e1.start);
	*(*stackp)++ = fragment(s, 
				ptrlist_append(e1.out, ptrlist_list1(&s->out0)),
				OP_NO);
      return 1;
#endif
    }
  }
  if(*stackp - stack >= 1){
    // We are not rewriting "||" as "|" as this would change the
    // bit-values generated
    if((*stackp)[-1].op == OP_ALTERNATE){
      // Just pop alternate marker, no need to look at it
      --(*stackp);
      s = state(0, NFA_SPLIT, NULL, NULL);
      *(*stackp)++ = fragment(s, 
			      ptrlist_append(ptrlist_list1(&s->out0),
					     ptrlist_list1(&s->out1)),
			      OP_NO);
      return 1;
    }
  }
  return 0;
}


unsigned int
do_right_paren(struct Fragment **stackp, struct Fragment *stack)
{
  struct State *s;
  struct Fragment e1, e2;
  unsigned int result;

  maybe_concat(stackp, stack);
  result = maybe_alternate(stackp, stack);

  if(*stackp - stack >= 1){
    e1 = *--(*stackp);


    // Put in a epsilon edge if the parenthesis is empty
    if(e1.op == OP_LEFT_PAREN){
      s = state(0, NFA_EPSILON, NULL, NULL);      
      result++;
      *(*stackp)++ = fragment(s, ptrlist_list1(&s->out0), OP_NO);
      return result;
    } 
    else if(e1.op == OP_LEFT_CAPT_PAREN){
#ifdef PAREN_MARKER
	parendepth++;
	s = state(0, NFA_EPSILON, NULL, NULL);
	s->subtype = LEFT_PAREN;
	result++;
	*(*stackp)++ = fragment(s, ptrlist_list1(&s->out0), OP_NO);
#endif
      s = state(0, NFA_EPSILON, NULL, NULL);      
      result++;
      *(*stackp)++ = fragment(s, ptrlist_list1(&s->out0), OP_NO);
#ifdef END_SPLIT_MARKER
      parendepth--;
      // Er det her rigtigt?
      (*stackp)[-1].parencount = parendepth == 0? 1 : 0;
#endif

#ifdef PAREN_MARKER
      maybe_concat(stackp, stack); 
      s = state(0, NFA_EPSILON, NULL, NULL);
      s->subtype = RIGHT_PAREN;
      result++;

      *(*stackp)++ = fragment(s, ptrlist_list1(&s->out0), OP_NO);
      // It is now safe to concatenate the 2 parenthesis markers
      maybe_concat(stackp, stack); 
      maybe_concat(stackp, stack); 
#endif
      return result;
    }

    if(*stackp - stack >= 1){
      e2 = *--(*stackp);

      assert(e2.op == OP_LEFT_PAREN || e2.op == OP_LEFT_CAPT_PAREN);

#ifdef PAREN_MARKER
      if(e2.op == OP_LEFT_CAPT_PAREN){
	parendepth++;
	s = state(0, NFA_EPSILON, NULL, NULL);
	s->subtype = LEFT_PAREN;
	result++;
	*(*stackp)++ = fragment(s, ptrlist_list1(&s->out0), OP_NO);
      }
#endif
      *(*stackp)++ = e1;

#ifdef END_SPLIT_MARKER
      if(e2.op == OP_LEFT_CAPT_PAREN){
	parendepth--;
	(*stackp)[-1].parencount = parendepth == 0? 1 : 0;
      }
#endif
#ifdef PAREN_MARKER
      if(e2.op == OP_LEFT_CAPT_PAREN){
	s = state(0, NFA_EPSILON, NULL, NULL);
	s->subtype = RIGHT_PAREN;
	result++;
	*(*stackp)++ = fragment(s, ptrlist_list1(&s->out0), OP_NO);
	maybe_concat(stackp, stack);
	maybe_concat(stackp, stack);
      }
#endif
      return result;

    } else {
      fprintf(stderr, "Error: Unbalanced parenthesis (%i)\n", __LINE__);
      exit(1);
    }
  } else {
    fprintf(stderr, "Error: Unbalanced parenthesis (%i)\n", __LINE__);
    exit(1);
  }
}


unsigned int
parse_cc_char(const char *re, const unsigned int len, unsigned int *i)
{
  if(*i < len && re[(*i)] == '\\'){
    if(++(*i) < len)
      return re[(*i)++];
    else {
      fprintf(stderr, "Error: Bad escape at position %i (%i)\n", *i, __LINE__);
      exit(1);
    }
  }
  else
    return re[(*i)++];

}


struct Range *
parse_cc_range(const char *re, const unsigned int len, unsigned int *i)
{
  unsigned int lo, hi;

  lo = parse_cc_char(re, len, i);
  if((len-(*i)) >=2 && re[(*i)] == '-' && re[(*i)+1] != ']'){
    (*i)++;
    hi = parse_cc_char(re, len, i);
    if(hi < lo){
      fprintf(stderr, "Error: Bad character range at position %i (%i)\n", *i, 
	      __LINE__);
      exit(1);
    }
    return range(lo, hi);
  }
  else
    return range(lo, lo);
}


struct Fragment
cc2fragment(const char *re, const unsigned int len, unsigned int *i)
{
  enum Boolean is_negated, first;
  struct Range **r;
  struct Fragment e;
  struct State *s;
  
  // First char is a [, no need to see that
  (*i)++;

  if(*i >= len){
    fprintf(stderr, "Error: Missing right bracket in character class (%i)\n",
	    __LINE__);
    exit(1);
  }

  // Is this character class negated?
  if(re[(*i)] == '^'){
    is_negated = true;
    (*i)++;
  } else
    is_negated = false;

  s = state(0, NFA_RANGE, NULL, NULL);
  s->is_negated = is_negated;
  r = &(s->range);
  e = fragment(s, ptrlist_list1(&s->out0), OP_NO);

  first = true;
  while(*i < len){
    if(re[(*i)] != ']' || first) {
      *r = parse_cc_range(re, len, i);
      r = &((*r)->next);
      first = false;
    }
    else 
      return e;
  }
  
  fprintf(stderr, "Error: Missing right bracket in character class (%i)\n", 
	  __LINE__);
  exit(1);
}


struct NFA
finish_up_regex(struct Fragment **stackp, struct Fragment *stack, 
		unsigned int statecount)
{
  struct State *s, *accept;
  struct Fragment e;
  struct NFA nfa;

  accept = state(0, NFA_ACCEPTING, NULL, NULL);
  statecount++;

  if(*stackp - stack == 0){
    nfa.start = accept;
    nfa.statecount = statecount;
    return nfa;
  }

  maybe_concat(stackp, stack);
  maybe_concat(stackp, stack);
  statecount += maybe_alternate(stackp, stack);

  if(*stackp - stack == 1){
    e = *--(*stackp);

    switch(e.op){
    case OP_ALTERNATE:
      s = state(0, NFA_SPLIT, NULL, NULL);
      statecount++;
      e = fragment(s, ptrlist_append(ptrlist_list1(&s->out0),
				     ptrlist_list1(&s->out1)),
		   OP_NO);
      // Fallthrough
    case OP_NO:
      ptrlist_patch(e.out, accept);
      ptrlist_free(e.out);
      break;
    case OP_LEFT_PAREN:
      fprintf(stderr, "Error: Unbalanced parenthesis (%i)\n", __LINE__);
      exit(1);
    case OP_LEFT_CAPT_PAREN:
      fprintf(stderr, "Error: Unbalanced parenthesis (%i)\n", __LINE__);
      exit(1);
    default:
      fprintf(stderr, "Error: Bad regular expression (%i)\n", __LINE__);
      exit(1);
    }

    nfa.start = e.start;
    nfa.statecount = statecount;
    return nfa;
  }
  else {
    fprintf(stderr, "Error: Unbalanced parenthesis (%i)\n", __LINE__);
    exit(1);
  }
}


unsigned int
do_quantifier(struct Fragment **stackp, struct Fragment *stack, 
	      unsigned int quantifier)
{
  struct Fragment e;
  struct State *s;
#ifdef END_REP_MARKER
  struct State *s1;
#endif


  if(*stackp <= stack){
    fprintf(stderr, 
	    "Error: Quantifier follows nothing (%i)\n", 
	    __LINE__);
    exit(1);
  }

  e = *--(*stackp);

  if(e.op != OP_NO){
    fprintf(stderr, 
	    "Error: Quantifier follows nothing (%i)\n", 
	    __LINE__);
    exit(1);
  }
  

  s = state(0, NFA_SPLIT, e.start, NULL);
#ifdef END_REP_MARKER
  s1 = state(0, NFA_EPSILON, s, NULL);
  s1->subtype = END_REPEAT;
#endif

  switch(quantifier){
  case '*':
#ifdef END_REP_MARKER
    ptrlist_patch(e.out, s1);
#else
    ptrlist_patch(e.out, s);
#endif
    ptrlist_free(e.out);
    *(*stackp)++ = fragment(s, ptrlist_list1(&s->out1), OP_NO);
    break;
  case '?':
    *(*stackp)++ = fragment(s, ptrlist_append(e.out, 
					      ptrlist_list1(&s->out1)), 
			    OP_NO);
    break;
  case '+':
#ifdef END_REP_MARKER
    ptrlist_patch(e.out, s1);
#else
    ptrlist_patch(e.out, s);
#endif
    ptrlist_free(e.out);
    *(*stackp)++ = fragment(e.start, ptrlist_list1(&s->out1), OP_NO);
    break;
  }

  return 1;
}


unsigned int
read_paren_type(const char *re, const unsigned int len, unsigned int *i)
{
  if(*i+2 < len && re[*i+1] == '?' && re[*i+2] == ':'){
    *i += 2;
    return PAREN_NON_CAPT;
  }
  else
    return PAREN_CAPT;
}


struct NFA 
re2nfa(const char *re, const unsigned int len)
{
  unsigned int i, statecount;
  struct State *s;
  struct Fragment *stackp;
  struct Fragment stack[len > 1? len : 1];

  stackp = stack;
  statecount = 0;
  #ifdef END_SPLIT_MARKER
  parendepth = 0;
  #endif

  for(i = 0; i < len; i++){
    switch(re[i]){
    case '*':
      // FALLTHROUGH      
    case '?':
      // FALLTHROUGH
    case '+':
      do_quantifier(&stackp, stack, re[i]);
      break;

    case '|':
      maybe_concat(&stackp, stack);
      statecount += maybe_alternate(&stackp, stack);
      // Push new alternate operator onto stack
      *stackp++ = fragment(NULL, NULL, OP_ALTERNATE);
      break;

    case '(':
      maybe_concat(&stackp, stack);
      
      if(read_paren_type(re, len, &i) == PAREN_CAPT){
	*stackp++ = fragment(NULL, NULL, OP_LEFT_CAPT_PAREN);
      }
      else
	*stackp++ = fragment(NULL, NULL, OP_LEFT_PAREN);
      break;

    case ')':
      statecount += do_right_paren(&stackp, stack);
      break;

    case '[':
      maybe_concat(&stackp, stack);
      statecount++;
      *stackp++ = cc2fragment(re, len, &i);
      break;
      
    case '.':
      maybe_concat(&stackp, stack);
      
      s = state(0, NFA_RANGE, NULL, NULL);
      *stackp++ = fragment(s, ptrlist_list1(&s->out0), OP_NO);
      s->range = range(0, 255);
      break;

    case '\\':
      i++;
      if(i >= len){
	fprintf(stderr, "Error: Bad escape at position %i (%i)\n", 
		i, __LINE__);
	exit(1);
      }

      // FALLTHROUGH

    default:
      maybe_concat(&stackp, stack);
      s = state(re[i], NFA_LITERAL, NULL, NULL);
      statecount++;
      *stackp++ = fragment(s, ptrlist_list1(&s->out0), OP_NO);
      break;
    }
  }

  // Finish up the regex and splice in an accepting state
  return finish_up_regex(&stackp, stack, statecount);
}
