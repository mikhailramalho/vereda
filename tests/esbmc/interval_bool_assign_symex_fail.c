_Bool b;

int main()
{
  b = __VERIFIER_nondet_bool();
  __VERIFIER_assume(b);
  b = __VERIFIER_nondet_bool();

  int i = 0;
  do
  {
    i++;
    if (i > 1)
      break;
  } while (b);

  __VERIFIER_assert(i == 2); // i is 1 when the second nondet bool is false
}
// Imported from ESBMC regression/esbmc/interval_bool_assign_symex_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
