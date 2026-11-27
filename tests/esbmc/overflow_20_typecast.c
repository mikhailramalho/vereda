//#include "common.h"
#include <stdio.h>

unsigned char aa[] = "asdffeagewaHAFEFaeDsFEawFdsFaefaeerdjgpim23";

unsigned short icrc(unsigned short crc, unsigned char *lin, unsigned int len, short jinit, int jrev)
{
}

//int argc, char *argv[]
int main()
{
  unsigned short i1, i2;
  int n;

  i2 = icrc(i1, aa, 42, (short) -1, 1);
   
  return 0;
  
}
// Imported from ESBMC regression/esbmc/overflow_20_typecast
// RUN: --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
