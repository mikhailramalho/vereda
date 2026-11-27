/* Regression for #7434: the nondet assignment inside nondet.h is on line 29 of
 * that header, past the end of this file. See nondet.h for the full rationale. */
#include "nondet.h"
#include <assert.h>

int main(void)
{
  int a = from_header();
  int b = __VERIFIER_nondet_int();
  assert(a + b != 7);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_7434_yaml_nondet_drop_fail
// Needs: --witness-output-yaml
// RUN: --witness-output-yaml -
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: type: "function_return"
// EXPECT: file_name: "[^"]*main\.c"
// EXPECT: line: 9$
// EXPECT: (?s)\A(?!.*\n *line: 29$)
// Vereda: verdict none
