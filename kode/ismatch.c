#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include "util.h"


void
display_usage(void)
{
  puts("ismatch - filter to determine if a match has occurred");
  puts("usage: ismatch [options]");
  puts("OPTIONS:");
  puts("These are the long option names, any unique abbreviation is also accepted.");
  puts("--output-stream=file");
  puts("\tOptional, if present output will be written to file. Default is stdout.");
  puts("--input-stream=file");
  puts("\tOptional, if present input will be read from file. Default is stdin.");
  puts("--help");
  puts("\tWill print this message");

  exit(1);
}


void
read_mbv(FILE *instream, FILE *outstream){
  char c;
  enum Boolean is_escaped = false;
  enum Boolean empty = true;

  while ( (c = read_bit(instream)) != EOF ){
    empty = false;
    if(is_escaped == true){
      is_escaped = false;
      continue;
    }

    switch(c){
    case 't':
      write_bit('t', outstream);
      return;

    case '\\':
      is_escaped = true;
      break;

    default:
      is_escaped = false;
      break;
    }
  }

  if(!empty)
    write_bit('b', outstream);
  else
    fprintf(stderr, "No output to read\n");
}


int 
main(const int argc, char *const argv[]) 
{
  int c, option_index;
  FILE *outstream = stdout;
  FILE *instream = stdin;
  //int i;

  static struct option long_options[] = {
    {"help",                no_argument, NULL, 'h'},
    {"output-stream", required_argument, NULL, 'o'},
    {"input-stream",  required_argument, NULL, 'i'},
    {0, 0, 0, 0}
  };

  while(true){
    c = getopt_long(argc, argv, "hi:o:", long_options, &option_index);
    if(c == -1)
      break;

    switch(c){
    case 'o':
      if((outstream = fopen(optarg, "w")) == NULL){
	perror("Can not open file for writing\n");
	exit(1);
      }
      break;

    case 'i':
      if((instream = fopen(optarg, "r")) == NULL){
	perror("Can not open file for writing\n");
	exit(1);
      }
      break;

    default:
       break;
    }
  }

  //for(i = 0; i < RUNS; i++){
  read_mbv(instream, outstream);
  //printf("%c", EOF);
  //}
  fclose(instream);
  fclose(outstream);
}
