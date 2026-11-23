// Reading through NULL crashes on the first iteration, so no state recurs.
// With the pointer check dropped, *p would read any value and repeat one.
int main(void)
{
  int *p = 0;
  int i = 0;
  while (i < 3)
    i = *p;
  return 0;
}
// Imported from ESBMC regression/termination/recurrent_state_null_deref
// Needs: --termination --max-inductive-step 3 --no-assertions
// RUN: --termination --k-step 2 --max-k-step 6 --max-inductive-step 3 --no-pointer-check --no-bounds-check --no-assertions
// MODE: FUTURE
// EXPECT: ^VERIFICATION UNKNOWN$
