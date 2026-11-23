// --no-slice --incremental-bmc

typedef struct {
  int b
} d;
union {} f[];
d *a;
void main() 
  { a->b; }
// Imported from ESBMC regression/esbmc/github_873
// RUN: --no-unwinding-assertions --unwind 1
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
