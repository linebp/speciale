#ifndef __groupings_h
#define __groupings_h


struct Channel {
  unsigned int parendepth;
  unsigned int id;
  struct State *s;
  struct Channel *prev;
  struct Channel *next;
  
#ifdef END_REP_MARKER
  enum Boolean end_rep_marker;
  unsigned int suspend_output;
#endif
};


struct Channel_list {
  struct Channel *first;
  struct Channel *cur;
};


unsigned int channel_id;

struct Channel_list clist = {NULL, NULL};

#endif
