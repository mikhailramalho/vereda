#include <assert.h>

int main() {
  float i=0x0102;
  char *p=(char *)&i;
//  printf("Bytes of i: %d, %d, %d, %d\n",
//         p[0], p[1], p[2], p[3]);

  printf("Bytes of i: %d, %d\n", p[0], p[1]);
  assert(p[0]==0);
  assert(p[1]==0);
//  assert(0);
} 
// Imported from ESBMC regression/esbmc/00_little_endian_04
// RUN: -Wno-error=implicit-function-declaration
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
