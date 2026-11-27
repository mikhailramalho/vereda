#include <assert.h>

int global __attribute__((weak)) = 999;

int main(void) { assert(global == EXPECTED_VALUE); return 0; }
// Imported from ESBMC regression/esbmc/linking-common
// RUN: -DEXPECTED_VALUE=1234
// FILES: ../linking-common/another_def.c
// EXPECT: ^VERIFICATION SUCCESSFUL$
