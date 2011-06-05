#include <stdio.h>
#include <graphviz/gvc.h>
#include "util.h"
#include "nfa.h"

#define LABEL_BUF_SIZE 11


int print_counter;
char label_buf[LABEL_BUF_SIZE];
//char *red = "red";
//char *black = "black";
//char *green = "green";

void
_print_range(struct Range *r, enum Boolean is_negated, char *buf, int size)
{
  int i;
  
  i = 0;
  if(i >= (size - 1))
    goto end;
  buf[i++] = '[';
  
  if(i >= (size - 1))
    goto end;
  if(is_negated)
    buf[i++] = '^';

  while(r != NULL){
    if(r->lo == r->hi){
      if(i >= (size - 1))
	goto end;
      buf[i++] = r->lo;
    }
    else{
      if(i >= (size - 4))
	goto end;
      buf[i++] = r->lo;
      buf[i++] = '-';
      buf[i++] = r->hi;
    }
    r = r->next;
  }

  if(i >= (size - 1))
    goto end;
  buf[i++] = ']';
 end:
  buf[i] = 0;
}

void 
_print_nfa(struct State *s, graph_t *g, Agnode_t *prev, char *label, 
	   char *color) 
{
  Agnode_t *n2;
  Agedge_t *e;
  Agsym_t *a;
  char temp[100];

  assert(s != NULL);

  if(s->n != NULL) {
    e = agedge(g, prev, s->n);
    a = agedgeattr(g, "label", "");
    agxset(e, a->index, label);

    a = agedgeattr(g, "color", "");
    agxset(e, a->index, color);

    return;
  } 

#if defined(END_SPLIT_MARKER) || defined(PAREN_MARKER) || defined(END_REP_MARKER)
  switch(s->type){
  case NFA_SPLIT:
    sprintf(temp, "%i\n%i", print_counter, s->parencount);
    break;
  case NFA_EPSILON:
    if(s->subtype == END_SPLIT)
      sprintf(temp, "%i\n%i", print_counter, s->parencount);
    else
      sprintf(temp, "%i", print_counter);
    break;
  default:
    sprintf(temp, "%i", print_counter);
  }
#else
  sprintf(temp, "%i", print_counter);
#endif

  s->id = print_counter;
  print_counter++;
  n2 = agnode(g, temp);
  a = agnodeattr(g, "shape", "");
  agxset(n2, a->index, "circle");

  s->n = n2;

  e = agedge(g, prev, n2);
  a = agedgeattr(g, "label", "");
  agxset(e, a->index, label);

  a = agedgeattr(g, "color", "");
  agxset(e, a->index, color);
  
  switch(s->type){
  case NFA_SPLIT:
    _print_nfa(s->out0, g, n2, "&#949;_0", "green");
    _print_nfa(s->out1, g, n2, "&#949;_1", "red");
    break;
  case NFA_ACCEPTING:
    a = agnodeattr(g, "shape", "");
    agxset(n2, a->index, "doublecircle");
    break;
  case NFA_RANGE:
    _print_range(s->range, s->is_negated, label_buf, LABEL_BUF_SIZE);
    _print_nfa(s->out0, g, n2, label_buf, "black");
    break;
  case NFA_EPSILON:
#if defined(END_SPLIT_MARKER) || defined(PAREN_MARKER) || defined(END_REP_MARKER)
    switch(s->subtype){
    case LEFT_PAREN:
      _print_nfa(s->out0, g, n2, "&#949;_(", "black");
      break;
    case RIGHT_PAREN:
      _print_nfa(s->out0, g, n2, "&#949;_)", "black");
      break;
    default:
      _print_nfa(s->out0, g, n2, "&#949;", "black");
    }
#else
    _print_nfa(s->out0, g, n2, "&#949;", "black");
#endif
    break;
  default:
    label_buf[0] = s->c;
    label_buf[1] = 0;
    _print_nfa(s->out0, g, n2, label_buf, "black");    
    break;
  }
}

void 
print_nfa(char *filename, struct State *s) 
{

  GVC_t *gvc; 
  graph_t *g; 
  Agnode_t *n;
  Agsym_t *a;
  char temp[10];
  FILE *file;


  if ((file = fopen(filename, "w")) == NULL) {
    printf("Could not open %s for writing\n", filename);
    return;
  }

  gvc = gvContext(); 

  g = agopen("NFA",AGDIGRAPH); 

  agraphattr(g, "fontname", "Palatino");
  agraphattr(g, "fontsize", "11");
  agraphattr(g, "rankdir", "LR");
  agraphattr(g, "margin", "0");
  //agraphattr(g, "size", "5.4,8.2");

  agnodeattr(g, "fontname", "Palatino");
  agnodeattr(g, "fontsize", "11");
  agnodeattr(g, "width", "0");
  agnodeattr(g, "height", "0");

  agedgeattr(g, "fontname", "Palatino");
  agedgeattr(g, "fontsize", "11");

  print_counter = 0;

  sprintf(temp, "%i", print_counter);
  print_counter++;

  n = agnode(g, temp);
  a = agnodeattr(g, "shape", "");
  agxset(n, a->index, "point");

  _print_nfa(s, g, n, "", "black");

  gvLayout(gvc, g, "dot"); 
  gvRender(gvc, g, "pdf", file); 
  gvFreeLayout(gvc, g); 
  agclose(g); 
  gvFreeContext(gvc); 
  fclose(file);
} 

