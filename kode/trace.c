#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include "util.h"

struct Channel {
  int cnum;
  int nnum;

  char *bits;
  unsigned int bits_size;
  unsigned int bits_count;
};

void
channel_write_bit(struct Channel *chan, char bit)
{
  if(chan->bits_size <= chan->bits_count){
    chan->bits_size = chan->bits_size * 2;
    if ((chan->bits = (char *) realloc(chan->bits, chan->bits_size)) == NULL ) {
      perror("Error reallocating memory for bit values\n");
      exit(1);
    }
  }
  chan->bits[chan->bits_count++] = bit;  
}

int
trace(char *mbv, unsigned int len, char **buf)
{
  int chan_cur = 0;
  unsigned int i, j, split_count = 0;
  enum Boolean first = true, even;
  struct Channel match = {0, 0, NULL, 0, 0};

  for(i = 0; i < len; i++){
    //printf("char: %c, i: %i, cur: %i, match.cnum: %i, match.nnum: %i, split_count: %i\n", mbv[i], i, chan_cur, match.cnum, match.nnum, split_count);
    even = true;
    for(j = i+1; j < len; j++){
      if(mbv[j] == '\\')
	even = !even;
      else
	break;
    }
    if(!even){
      if(i+1 < len && mbv[i+1] == '\\'){
	if(match.bits != NULL){
	  if(chan_cur == match.cnum){
	    channel_write_bit(&match, mbv[i]);
	    channel_write_bit(&match, '\\');
	  }
	  else if((chan_cur - match.cnum) <= split_count){
	    channel_write_bit(&match, mbv[i]);
	    channel_write_bit(&match, '\\');
	  }
	}
	i++;
	continue;
      }
    }

    switch(mbv[i]){
    case '|':
      match.cnum = match.nnum;
      split_count = 0;
      first = false;
      chan_cur = 0;
      break;

    case ':':
      split_count = 0;
      chan_cur++;
      break;

    case '=':
      if(match.bits != NULL){
	if(((chan_cur - match.cnum) <= split_count) || 
	   (match.cnum >= chan_cur) || (match.cnum == -1)){
	  match.nnum--;
	}
	if(match.cnum <= chan_cur)
	  split_count++;
      }
      break;

    case 't':
      assert(match.bits == NULL);
      match.bits_size = 1024;
      if((match.bits = malloc(match.bits_size)) == NULL){
	perror("Can not allocate memory for bits\n");
	exit(1);
      }
      match.cnum = chan_cur;
      match.nnum += chan_cur;
      break;

    case 'b':
      if(!first) {
	if(match.bits != NULL){
	  if(match.cnum >= chan_cur){
	    match.cnum++;
	    match.nnum++;
	  }
	}
      }
      break;

    case '0':
      // Fallthrough

    case '1':
      if(match.bits != NULL){
	if(chan_cur == match.cnum){
	  channel_write_bit(&match, mbv[i]);
	}
	else if((chan_cur - match.cnum) <= split_count){
	  channel_write_bit(&match, mbv[i]);
	  match.cnum = chan_cur;
	}
      }
      break;

    default:
      break;
    }
  }  

  *buf = match.bits;
  return match.bits_count;
}


unsigned int
read_mbv(char **mbv, FILE *instream)
{
  char c;
  int size = 1024;
  int i = 0;

  if ((*mbv = (char *) malloc(size)) == NULL ) {
    perror("Error allocating memory for mixed bit values\n");    
    exit(1);
  }
  
  while ( (c = read_bit(instream)) != EOF ){
    if(i >= size){
      size = size*2;
      if ((*mbv = (char *) realloc(*mbv, size)) == NULL ) {
	perror("Error reallocating memory for mixed bit values\n");
	exit(1);
      }
    }
    (*mbv)[i++] = c;
  }

  return i;
}


void
reverse(char *str, unsigned int len)
{
  char tmp;
  unsigned int i;
  
  for(i = 0; i < len / 2; ++i){
    tmp = str[i];
    str[i] = str[len-i-1];
    str[len-i-1] = tmp;
  }
}


void
display_usage(void)
{
  puts("trace - ");
  puts("usage: trace [options]");
  puts("OPTIONS:");
  puts("These are the long option names, any unique abbreviation is also accepted.");
  puts("--output-stream=file");
  puts("\tOptional, if present output will be written to file. Default is stdout.");
  puts("--input-stream=file");
  puts("\tOptional, if present output will be read from file. Default is stdin.");
  puts("--help");
  puts("\tWill print this message");

  exit(1);
}


int 
main(const int argc, char *const argv[]) 
{
  int c, option_index;
  unsigned int mbvlen, matchlen;
  char *mbv, *match;
  FILE *outstream = stdout;
  FILE *instream = stdin;
  char *outbuf;
  char *inbuf;
  unsigned int i;

  static struct option long_options[] = {
    {"help",                no_argument, NULL, 'h'},
    {"output-stream", required_argument, NULL, 'o'},
    {"input-stream",  required_argument, NULL, 'i'},
    {0, 0, 0, 0}
  };

  while(true){
    c = getopt_long(argc, argv, "ho:i:", long_options, &option_index);
    if(c == -1)
      break;

    switch(c){
    case 'h':
      display_usage();
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

  outbuf = init_stream(BUFSIZE, outstream);
  inbuf = init_stream(BUFSIZE, instream);
  
  mbvlen = read_mbv(&mbv, instream);
  reverse(mbv, mbvlen);
  matchlen = trace(mbv, mbvlen, &match);
  if(match != NULL){
    reverse(match, matchlen);
    for(i = 0; i < matchlen; i++){
      write_bit(match[i], outstream);
    }
  } 
  free(mbv);
  free(match);

  close_stream(outbuf, outstream);
  close_stream(inbuf, instream);
  return 0;
}
