#include <stdlib.h>

extern unsigned __VERIFIER_nondet_uint(void);

int main(void)
{
  unsigned n = __VERIFIER_nondet_uint();
  char *p = malloc(n);
  free(p);
  return 0;
}
// Imported from ESBMC regression/esbmc/cwe_excessive_alloc_sarif
// Needs: --excessive-alloc-check --sarif-output -
// RUN: --excessive-alloc-check --incremental-bmc --sarif-output -
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: "ruleId": "excessive-allocation"
// EXPECT: "external/cwe/cwe-789"
// EXPECT: "id": "789"
// EXPECT: "name": "CWE"
// EXPECT: "version": "4.20"
// Vereda: verdict none
