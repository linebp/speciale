#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include "util.h"
#include "nfa.h"

void
display_usage(void)
{
  puts("main - match regular expression with text generating mixed bit-values");
  puts("usage: main [regex] [text] [more options]");
  puts("OPTIONS:");
  puts("These are the long option names, any unique abbreviation is also accepted.");
  puts("--regular-expression=regex");
  puts("\tThe regular expression.");
  puts("--text=text");
  puts("\tThe text to be matched.");
  puts("--debug-file=file");
  puts("\tOptional, this is the file where debug output is dumped."); 
  puts("\tThe debug output consists of a graph of the NFA in pdf format.");
  puts("--output-stream=file");
  puts("\tOptional, if present output will be written to file. Default is stdout.");
  puts("--regular-expression-file=file");
  puts("\tRead regular expression from file.");
  puts("--text-file=file");
  puts("\tRead text to be matched from file.");
  puts("--help");
  puts("\tWill print this message");

  exit(1);
}


int 
main(const int argc, char *const argv[]) 
{
  int c, regexlen, textlen, option_index;
  struct NFA nfa;
  char *regex = NULL;
  char *text = NULL;
  char *debugfile = NULL;
  FILE *outstream = stdout;
  FILE *instream = stdin;
  char *outbuf;
  char *inbuf;

  static struct option long_options[] = {
    {"help",                          no_argument, NULL, 'h'},
    {"regular-expression",      required_argument, NULL, 'r'},
    {"regular-expression-file", required_argument, NULL, 'a'},
    {"text-file",               required_argument, NULL, 'b'},
    {"debug-file",              required_argument, NULL, 'd'},
    {"output-stream",           required_argument, NULL, 'o'},
    {0, 0, 0, 0}
  };

  while(true){
    c = getopt_long(argc, argv, "a:b:hr:t:d:o:", long_options, &option_index);
    if(c == -1)
      break;

    switch(c){
    case 'a':
      regexlen = read_file(optarg, &regex);
      break;

    case 'b':
      if((instream = fopen(optarg, "r")) == NULL){
	perror("Can not open file for reading\n");
	exit(1);
      }
      break;

    case 'h':
      display_usage();
      break;
      
    case 'r':
      regex = optarg;
      regexlen = strlen(regex);
      break;

    case 't':
      text = optarg;
      textlen = strlen(text);
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

    default:
       break;
    }
  }

  if(optind < argc) {
    regex = argv[optind++];
    regexlen = strlen(regex);
  }

  if(regex == NULL)
    display_usage();

  outbuf = init_stream(BUFSIZE, outstream);
  inbuf = init_stream(BUFSIZE, instream);

  nfa = re2nfa(regex, regexlen);
  if(debugfile != NULL)
    print_nfa(debugfile, nfa.start);
  match(nfa, instream, outstream);
  nfa_free(nfa.start);

  close_stream(outbuf, outstream);
  close_stream(inbuf, instream);
  return 0;
}
