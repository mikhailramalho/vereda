#ifndef uint16_t
#define uint16_t unsigned short
#endif
int main()
{
  uint16_t a = 0xFFFF;
  uint16_t b = a << 16;
  return 0;
}
// Imported from ESBMC regression/esbmc/github_381
// RUN: --overflow-check
// EXPECT: github_381\.c line 7 column 3 function main$
// Vereda: unmet EXPECT: github_381\.c line 7 column 3 function main$
// MODE: FUTURE
