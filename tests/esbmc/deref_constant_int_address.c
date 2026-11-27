int main()
{
  // A constant non-null integer cast to a pointer must still be checked; it
  // used to escape the invalid-pointer check (#6544).
  int *p = (int *)65;
  return *p;
}
// Imported from ESBMC regression/esbmc/deref_constant_int_address
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^  dereference failure: invalid pointer$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
