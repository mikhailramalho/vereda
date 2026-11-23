// The frame exceeds --stack-limit, which fails a claim of its own in the run
// that looks for a recurrent state. That failure shows no repeated state.
int main(void)
{
  volatile char big[4096];
  big[0] = 0;
  unsigned x = 1;
  while (x != 0)
    x <<= 8;
  return big[0];
}
// Imported from ESBMC regression/termination/recurrent_state_other_claim
// Needs: --termination --max-inductive-step 3 --no-assertions --stack-limit 1024
// RUN: --termination --k-step 2 --max-k-step 12 --max-inductive-step 3 --no-pointer-check --no-bounds-check --no-assertions --stack-limit 1024
// MODE: FUTURE
// EXPECT: ^VERIFICATION UNKNOWN$
