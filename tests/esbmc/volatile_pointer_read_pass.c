struct Flags
{
  volatile int changing;
  int stable;
};

volatile int value;
struct Flags flags = {0, 7};

int main(void)
{
  volatile int *pointer = &value;
  int snapshot = *pointer;
  int copy = snapshot;
  __VERIFIER_assert(pointer == &value); // taking an address preserves the object
  __VERIFIER_assert(snapshot == copy); // a saved value stays stable
  __VERIFIER_assert(flags.stable == 7); // ordinary members stay stable

  volatile struct Flags whole;
  volatile struct Flags *whole_pointer = &whole;
  int member_snapshot = whole_pointer->stable;
  int member_copy = member_snapshot;
  __VERIFIER_assert(member_snapshot == member_copy); // a saved member stays stable

  volatile struct Flags *absent = 0;
  int guarded = absent ? absent->changing : 7;
  __VERIFIER_assert(guarded == 7); // an unevaluated read has no pointer check
  int short_circuit = absent == 0 || absent->changing;
  __VERIFIER_assert(short_circuit); // short circuit guards the volatile access
}
// Imported from ESBMC regression/esbmc/volatile_pointer_read_pass
// Needs: --std c11 --volatile-check
// RUN: --std c11 --volatile-check
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
