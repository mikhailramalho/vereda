// Closing each kind of stream once is no error: a stream from fopen
// (fclose gives 0 or EOF), from fmemopen and from open_memstream,
// and the three standard streams, which are not freed.
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <stdio.h>
extern char __VERIFIER_nondet_char(void);
extern void __VERIFIER_assert(int);
int main(void) {
  FILE *f = fopen("x", "r");
  if (f) {
    int r = fclose(f);
    __VERIFIER_assert(r == 0 || r == EOF);
  }
  char buf[8];
  FILE *m = fmemopen(buf, sizeof buf, "w");
  if (m) {
    fputc(__VERIFIER_nondet_char(), m);
    fclose(m);
  }
  char *p = 0;
  size_t n = 0;
  FILE *o = open_memstream(&p, &n);
  if (o)
    fclose(o);
  fclose(stdin);
  fclose(stdout);
  fclose(stderr);
  return 0;
}