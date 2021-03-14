#include <stdio.h>
#include "util.h"
#include "color.h"
void coloredPerror(const char *msg){
  puts(ANSI_COLOR_RED);
  perror(msg);
  puts(ANSI_COLOR_RESET);
}
