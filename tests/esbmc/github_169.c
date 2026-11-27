a;
void h();
#define c(d, g) strcmp(d, 0)
main(argc, argv) char **argv;
{
  char *b[argc];
  char e;
  for (; e;)
    *b = a;
  h(b);
}
void h(f) { c(f, ); }
// Imported from ESBMC regression/esbmc/github_169
// RUN: --ub-pointer-check=false --ub-div-by-zero-check=false --incremental-bmc --max-k-step 10
// EXPECT: ^VERIFICATION UNKNOWN$
// Vereda: unmet EXPECT: ^VERIFICATION UNKNOWN$
// MODE: FUTURE
// Vereda: verdict none
