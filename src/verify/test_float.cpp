#include "roundoff.h"
#include <stdio.h>
#include <stdlib.h>

char *double_to_hex(double x)
{
  static char buf[100];
  union {
    double d;
    long l[2];
  } u;
  u.d = x;
  sprintf(buf, "'%0lx', '%0lx' (%.18f)", u.l[0], u.l[1], u.d);
  return buf;
}
  
int main() {
  double small = 1e-200;
  int i;
  initialize_roundoff();
  printf("\n");
  for (i = 0; i < 6; i++)
    printf("pow(2, %d/6) = %s\n", i, double_to_hex(pow(2, i/6.0)));
  
  printf("This program may abort after this line.  This is correct behaviour\n");
  fflush(stdout);
  small = small*small;
  if (!roundoff_ok()) {
    printf("Roundoff detected.\n");
  } else {
    printf("Error --  underflow has occurred, but did not abort!\n");
    printf("small = %g\n", small);
  }
  exit(99);
}

