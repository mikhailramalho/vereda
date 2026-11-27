
typedef struct {
    int i;
} s;

typedef s *s_t;

int main()
{
  s_t p;

  p = (s_t)malloc(sizeof(s_t));
  free(p);
  // this should fail, as the object is not alive anymore
  p->i = __VERIFIER_nondet_int();
}
// Imported from ESBMC regression/cbmc/01_cbmc_Malloc4
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
