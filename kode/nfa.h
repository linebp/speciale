#ifndef __nfa_h
#define __nfa_h

#include "util.h"

/******************************
 * NFA states
 ******************************/

#define NFA_SPLIT 256
#define NFA_ACCEPTING 257
#define NFA_LITERAL 258
#define NFA_RANGE 259
#define NFA_EPSILON 260

/******************************
 * Subtypes
 ******************************/

#define NONE 0
#define LEFT_PAREN 1
#define RIGHT_PAREN 2
#define END_SPLIT 3
#define END_REPEAT 4
#define REPEAT 5
#define ALTERNATE 6

#define PAREN_CAPT 0
#define PAREN_NON_CAPT 1

struct Range 
{
  unsigned int lo;
  unsigned int hi;

  struct Range *next;
};


enum Color
{
  white, gray, black
};

struct State 
{
  // The type of the node: split, accepting, literal, epsilon or range
  unsigned int type;
  // If type is set to literal, this contains the value of the literal
  unsigned int c;
  // For freeing the nfa
  enum Boolean is_seen;
#if defined(PAREN_MARKER) || defined(END_SPLIT_MARKER) || defined (END_REP_MARKER)
  // The subtype of the transition: end_split
  int subtype;
  // If type is set to split or epsilon->end_split, 
  // this contains the parenthesis count
  unsigned int parencount;
#endif
  // If type is set to range, this will show if the range is negated
  enum Boolean is_negated;
  // If the type is set to range, this contains the pointer to the
  // Range structure
  struct Range *range;
  // If the type is set to split, literal or range, this contains a
  // pointer to a following state
  struct State *out0;
  // If the type is set to split, this contains a pointer to a
  // following state
  struct State *out1;
  // Flags to ensure we only add each state once to the next list in
  // the simulation for each step
  unsigned int laststep;

  // For debugging
  Agnode_t *n;
  unsigned int id;
};


struct Range *range(unsigned int lo, unsigned int hi);

struct State *state(unsigned int c, unsigned int type,
		    struct State *s0, struct State *s1);

void nfa_free(struct State *s);


/******************************
 * NFA
 ******************************/
struct NFA {
  struct State *start;
  unsigned int statecount;
};

struct NFA re2nfa(const char *re, const unsigned int len);

#endif
