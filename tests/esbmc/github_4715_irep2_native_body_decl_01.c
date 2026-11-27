// Exercises the IREP2-native code_decl2t dispatcher (W1-loc
// spike Phase C, esbmc/esbmc#4715): compute()'s body declares trivial-type
// locals with side-effect-free initializers, so goto_convert consumes each
// code_decl2t natively (DECL + side-effect-free ASSIGN, no legacy round-trip)
// and the block emits their scope-exit DEAD instructions by managing the
// destructor stack exactly as convert_block does; main() (a call + assert) falls
// back to goto_convert_rec. The declared values must flow correctly and the
// verdict/GOTO must match a run without the flag.
#include <assert.h>

int g;

void compute(int p)
{
  int y = p + 1;
  int z = y * 2;
  g = z;
}

int main(void)
{
  compute(5);
  assert(g == 12);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_4715_irep2_native_body_decl_01
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
