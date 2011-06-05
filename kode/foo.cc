#include <re2/re2.h>
#include <stdio.h>
#include <string>
#include <iostream>


using namespace re2;

int 
main(int argc, char *argv[]) 
{
  string s;
  
  if(RE2::FullMatch(argv[2], argv[1], &s)) {
    std::cout << s << std::endl;
    return 0;
  }
  return 1;
}
