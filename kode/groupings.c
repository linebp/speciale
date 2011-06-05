#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include "nfa.h"
#include "util.h"
#include "match.h"
#include "groupings.h"


struct Channel *
channel(struct State *s){
  struct Channel *new;

  if ((new = (struct Channel *) malloc(sizeof(struct Channel))) == NULL ) {
    fprintf(stderr, "Error allocating memory for channel\n");    
    exit(1);
  }
  
  new->s = s;
  new->id = channel_id++;
  new->next = NULL;
  new->prev = NULL;
  new->parendepth = 0;
#ifdef END_REP_MARKER
  new->end_rep_marker = false;
  new->suspend_output = 0;
#endif
  return new;
}

struct Channel *
channel_copy(struct Channel *old)
{
  struct Channel *new;

  if ((new = (struct Channel *) malloc(sizeof(struct Channel))) == NULL ) {
    fprintf(stderr, "Error allocating memory for channel\n");    
    exit(1);
  }

  new->s = old->s;
  new->id = channel_id++;
  new->next = old->next;
  new->prev = old;
  new->parendepth = old->parendepth;
#ifdef END_REP_MARKER
  new->end_rep_marker = old->end_rep_marker;
  new->suspend_output = old->suspend_output;
#endif
  return new;
}


void
channel_free(struct Channel *ch)
{
  free(ch);
}


void
channel_remove(struct Channel *ch)
{
  struct Channel *prev, *next;

  next = ch->next;
  prev = ch->prev;

  if(ch->next == NULL){
    if(ch->prev != NULL)
      ch->prev->next = NULL;
  }
  else if(ch->prev == NULL){
    if(ch->next != NULL)
      ch->next->prev = NULL;
  }
  else {
    ch->next->prev = ch->prev;
    ch->prev->next = ch->next;
  }

  if(ch == clist.first)
    clist.first = ch->next;

  channel_free(ch);  
}


void
follow_epsilon(struct Channel *cur, FILE *outstream)
{
  unsigned int i;

  assert(cur != NULL);

  while(true){
    assert(cur->s != NULL);
    switch(cur->s->type){
    case NFA_EPSILON:
      switch(cur->s->subtype){

      case LEFT_PAREN:    
	if(cur->parendepth == 0)
	  write_bit('1', outstream);
#ifdef END_REP_MARKER
      if(!cur->suspend_output)
#endif
	cur->parendepth++;
      break;

      case RIGHT_PAREN:
#ifdef END_REP_MARKER
      if(!cur->suspend_output)
#endif
	cur->parendepth--;
	break;

      case END_SPLIT:
	for(i = 0; i < cur->s->parencount; i++){
	  write_bit('0', outstream);
	}
	break;

#ifdef END_REP_MARKER
      case END_REPEAT:
	cur->end_rep_marker = true;
#endif
      }
      // Fallthrough

    case NFA_LITERAL:
      cur->s = cur->s->out0;
      break;

    default:
      return;
    }
  }
}


void
do_end(char c, FILE *outstream)
{
  struct Channel *tmp;

  if(clist.cur == NULL){
    fprintf(stderr, "Error: Channel corruption (%i)\n", __LINE__);
    exit(0);
  } 

  write_bit(c, outstream);
  tmp = clist.cur;
  clist.cur = clist.cur->next;
  channel_remove(tmp);
}


void
do_split(FILE *outstream)
{
  struct Channel *tmp;

  if(clist.cur == NULL){
    fprintf(stderr, "Error: Channel corruption (%i)\n", __LINE__);
    exit(0);
  } 
  write_bit('=', outstream);
  
  tmp = channel_copy(clist.cur);
  if(tmp->next != NULL)
    tmp->next->prev = tmp;
  clist.cur->next = tmp;
}


void
do_one(FILE *outstream)
{
  unsigned int i;

  if(clist.cur == NULL){
    fprintf(stderr, "Error: Channel corruption (%i)\n", __LINE__);
    exit(0);
  } 
  assert(clist.cur->s->type == NFA_SPLIT);

#ifdef END_REP_MARKER
  if(clist.cur->end_rep_marker){
    clist.cur->suspend_output++;
    clist.cur->end_rep_marker = false;	
  }
#endif

  // Make transition on out1-arrow
  for(i = 0; i < clist.cur->s->parencount; i++){
    write_bit('0', outstream);
  }
  if(clist.cur->parendepth)
    write_bit('1', outstream);
  clist.cur->s = clist.cur->s->out1;
  follow_epsilon(clist.cur, outstream);
}


void
do_zero(FILE *outstream)
{
  if(clist.cur == NULL){
    fprintf(stderr, "Error: Channel corruption (%i)\n", __LINE__);
    exit(0);
  } 

  assert(clist.cur->s->type == NFA_SPLIT);

#ifdef END_REP_MARKER
  if(clist.cur->end_rep_marker){
    clist.cur->suspend_output--;
    clist.cur->end_rep_marker = false;	
  }
#endif

  if(clist.cur->parendepth)
    write_bit('0', outstream);
  clist.cur->s = clist.cur->s->out0;
  follow_epsilon(clist.cur, outstream);
}


void
do_escape(FILE *instream, FILE *outstream)
{
  char c;

  if(clist.cur == NULL){
    fprintf(stderr, "Error: Channel corruption (%i)\n", __LINE__);
    exit(0);
  } 

  c = read_bit(instream);
  //printf("received char: %c %i\n", c, c);
  if(c == EOF){
    fprintf(stderr, "Error: Bad escape (%i)\n", __LINE__);
  }
  
  // Make transition on out-arrow
  assert(clist.cur->s->type == NFA_RANGE);
  clist.cur->s = clist.cur->s->out0;
  follow_epsilon(clist.cur, outstream);
  if(clist.cur->parendepth){
    write_bit('\\', outstream);
    write_bit(c, outstream);
  }
}


void
read_mbv(struct NFA nfa, FILE *instream, FILE *outstream)
{
  char c;
  int i = 0;
  enum Boolean channel_switch;
  channel_id = 1;

  clist.first = channel(nfa.start);
  clist.cur = clist.first;
  follow_epsilon(clist.cur, outstream);

  while ( (c = read_bit(instream)) != EOF ){
    i++;
    //printf("received char %i: %c %i\n", i, c, c);

    switch(c){
    case '|':
      channel_switch = true;
      write_bit(c, outstream);
      clist.cur = clist.first;
      break;

    case ':':
      if(clist.cur == NULL){
	fprintf(stderr, "Error: Channel corruption (%i)\n", __LINE__);
	exit(0);
      } 
      write_bit(c, outstream);

      if(channel_switch)
	clist.cur = clist.cur->next;
      else
	channel_switch = true;
      break;

    case '=':
      channel_switch = true;
      do_split(outstream);
      break;

    case 't':
      // FALLTRHOUGH
      
    case 'b':
      channel_switch = false;
      do_end(c, outstream);
      break;

    case '0':
      channel_switch = true;
      do_zero(outstream);
      break;

    case '1':
      channel_switch = true;
      do_one(outstream);
      break;

    case '\\':
      channel_switch = true;
      do_escape(instream, outstream);
      break;

    case '*':
      channel_switch = true;
      break;

    default:
      fprintf(stderr, "Error: Bad character: %c %i (%i)\n", c, c, __LINE__);
      exit(0);
    }
  }
}


void
display_usage(void)
{
  puts("main - match regular expression with text generating mixed bit-values");
  puts("usage: main [regex] [text] [more options]");
  puts("OPTIONS:");
  puts("These are the long option names, any unique abbreviation is also accepted.");
  puts("--regular-expression=regex");
  puts("\tThe regular expression.");

  puts("--debug-file=file");
  puts("\tOptional, this is the file where debug output is dumped."); 
  puts("\tThe debug output consists of a graph of the NFA in pdf format.");

  puts("--output-stream=file");
  puts("\tOptional, if present output will be written to file. Default is stdout.");

  puts("--input-stream=file");
  puts("\tOptional, if present output will be read from file. Default is stdin.");

  puts("--regular-expression-file=file");
  puts("\tRead regular expression from file.");

  puts("--help");
  puts("\tWill print this message");

  exit(1);
}


int 
main(const int argc, char* const argv[]) 
{
  struct NFA nfa;
  int c, regexlen, option_index;
  char *regex = NULL;
  char *debugfile = NULL;
  FILE *outstream = stdout;
  FILE *instream = stdin;
  char *outbuf;
  char *inbuf;

  static struct option long_options[] = {
    {"help",                          no_argument, NULL, 'h'},
    {"regular-expression",      required_argument, NULL, 'r'},
    {"regular-expression-file", required_argument, NULL, 'a'},
    {"debug-file",              required_argument, NULL, 'd'},
    {"output-stream",           required_argument, NULL, 'o'},
    {"input-stream",            required_argument, NULL, 'i'},
    {0, 0, 0, 0}
  };

  while(true){
    c = getopt_long(argc, argv, "hr:a:d:o:i:", long_options, &option_index);
    if(c == -1)
      break;

    switch(c){
    case 'a':
      regexlen = read_file(optarg, &regex);
      break;

    case 'h':
      display_usage();
      break;
      
    case 'r':
      regex = optarg;
      regexlen = strlen(regex);
      break;

    case 'd':
      debugfile = optarg;
      break;

    case 'o':
      if((outstream = fopen(optarg, "w")) == NULL){
	perror("Can not open file for writing\n");
	exit(1);
      }
      break;

    case 'i':
      if((instream = fopen(optarg, "r")) == NULL){
	perror("Can not open file for reading\n");
	exit(1);
      }
      break;

    default:
       break;
    }
  }

  if(optind < argc) {
    regex = argv[optind++];
    regexlen = strlen(regex);
  }

  outbuf = init_stream(BUFSIZE, outstream);
  inbuf = init_stream(BUFSIZE, instream);

  nfa = re2nfa(regex, regexlen);
  if(debugfile != NULL)
    print_nfa(debugfile, nfa.start);
  read_mbv(nfa, instream, outstream);
  nfa_free(nfa.start);

  close_stream(outbuf, outstream);
  close_stream(inbuf, instream);
  return 0;
}
