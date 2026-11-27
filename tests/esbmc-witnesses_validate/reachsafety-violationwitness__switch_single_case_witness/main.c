extern int __VERIFIER_nondet_int(void);
extern void reach_error(void);
int main()
{
  int v = __VERIFIER_nondet_int();
  int r = 0;
  switch (v)
  {
  case 2:
    r = 2;
  }
  if (r == 2)
    reach_error();
  return 0;
}
// Imported from ESBMC regression/witnesses_validate/reachsafety-violationwitness/switch_single_case_witness
// Needs: --validate-violation-witness --witness witness.yml
// RUN: --validate-violation-witness --witness witness.yml
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
