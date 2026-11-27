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
// Imported from ESBMC regression/esbmc/github_9650_witness_argv_line0
// Needs: --witness-output-yaml w.yml CHECK_FILE w.yml contains type: "branching" CHECK_FILE w.yml absent line: 0$ CHECK_FILE w.yml contains line: 8$
// RUN: --witness-output-yaml w.yml --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
