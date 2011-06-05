#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include "nfa.h"
#include "util.h"

void
follow(struct State **s, FILE *outstream)
{
  while(true){
    assert((*s) != NULL);
    switch((*s)->type){
    case NFA_EPSILON:
      (*s) = (*s)->out0;
      break;

    case NFA_LITERAL:
      write_bit((*s)->c, outstream);
      (*s) = (*s)->out0;
      break;
    
    default:
      return;
    }
  }
}


void
read_bv(struct NFA nfa, FILE *instream, FILE *outstream)
{
  char c;
  struct State *cur;

  cur = nfa.start;

  while ( (c = read_bit(instream)) != EOF ){
    //printf("received char: %c\n", c);
    assert(cur != NULL);
    switch(c){
    case '0':
      cur = cur->out0;
      follow(&cur, outstream);
      break;

    case '1':
      cur = cur->out1;
      follow(&cur, outstream);
      break;

    case '\\':
      c = read_bit(instream);
      if(c == EOF){
	fprintf(stderr, "Error: Bad escape (%i)\n", __LINE__);
      }
      write_bit(c, outstream);
      cur = cur->out0;
      follow(&cur, outstream);
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
  puts("serialize - ");
  puts("usage: main [regex] [more options]");
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
  puts("\tOptional, if present input will be read from file. Default is stdin.");
  puts("--regular-expression-file=file");
  puts("\tRead regular expression from file.");
  puts("--help");
  puts("\tWill print this message");

  exit(1);
}


int
main(const int argc, char* const argv[])
{
  int c, regexlen, option_index;
  struct NFA nfa;
  char *debugfile = NULL;
  char *regex;
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
  read_bv(nfa, instream, outstream);
  
  nfa_free(nfa.start);

  close_stream(outbuf, outstream);
  close_stream(inbuf, instream);
  return 0;
}
