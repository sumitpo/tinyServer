#include <stdio.h>
#include <sys/sysinfo.h>
#include "color.h"
int main(int argc, char *argv[]){
  int numCores = get_nprocs();
  printf("%d cores\n", numCores);
  printf(ANSI_COLOR_BLUE "hello world" ANSI_COLOR_RESET);
  printf(ANSI_COLOR_CYAN "hello world" ANSI_COLOR_RESET);
  printf(ANSI_COLOR_GREEN "hello world" ANSI_COLOR_RESET);
  printf(ANSI_COLOR_MAGENTA "hello world" ANSI_COLOR_RESET);
  printf(ANSI_COLOR_RED "hello world" ANSI_COLOR_RESET);
  printf(ANSI_COLOR_YELLOW "hello world" ANSI_COLOR_RESET);
  printf(ANSI_COLOR_GRAY "hello world" ANSI_COLOR_RESET);
  printf("\n");
  return 0;
}
