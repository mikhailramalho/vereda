/* The argv setup ESBMC adds for this main has no source location. */
#include <assert.h>
void reach_error(void) { assert(0); }
extern int __VERIFIER_nondet_int(void);
int main(int argc, char *argv[])
{
  int x = __VERIFIER_nondet_int();
  if (x == 3)
    reach_error();
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9677_graphml_argv_line0
// Needs: --witness-output-graphml w.graphml CHECK_FILE w.graphml absent <data key="startline">0</data> CHECK_FILE w.graphml contains <data key="startline">7</data> CHECK_FILE w.graphml contains <data key="startline">3</data>
// RUN: --witness-output-graphml w.graphml --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
