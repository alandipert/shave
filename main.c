#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include "parse.h"

#define MAX_BUFFER_SIZE 1024

int main(int argc, char *argv[])
{
  if(argc > 1) {
    eval(argv[1]);
  } else {
    int size;
    char buf[MAX_BUFFER_SIZE+1];

    printf("shave.");
    printf("\n> ");
    fflush(stdout);
    while ((size = read(fileno(stdin), buf, MAX_BUFFER_SIZE)) >  0) {
      buf[size]='\0';
      eval(buf);
      printf("\n> ");
      fflush(stdout);
    }
  }

  return 0;
}
