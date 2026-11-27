/* github #10303: printf and fprintf did not check that the format pointer
 * is valid. */
#include <stdio.h>
#include <stdlib.h>

int __VERIFIER_nondet_int(void);

int main()
{
  char *p = malloc(3);
  if (!p)
    return 0;
  p[0] = 'h';
  p[1] = '\n';
  p[2] = '\0';
  int choice = __VERIFIER_nondet_int();
  if (choice)
    free(p);
  printf(p);
  fprintf(stdout, p);
  if (!choice)
    free(p);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10303_fail
// RUN: --multi-property --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: FAILED .* line 19 +dereference failure: invalidated dynamic object$
// EXPECT: FAILED .* line 20 +dereference failure: invalidated dynamic object$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
