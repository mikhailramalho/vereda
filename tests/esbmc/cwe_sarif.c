#include <stdlib.h>

int main(void)
{
  int *p = NULL;
  return *p;
}
// Imported from ESBMC regression/esbmc/cwe_sarif
// Needs: --sarif-output -
// RUN: --incremental-bmc --sarif-output -
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: "version": "2.1.0"
// EXPECT: "ruleId": "null-pointer-dereference"
// EXPECT: "name": "CWE"
// EXPECT: "version": "4.20"
// EXPECT: "id": "476"
// Vereda: verdict none
