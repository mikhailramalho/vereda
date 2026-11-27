#include <assert.h>
#include <stddef.h>

extern void __VERIFIER_nondet_memory(void *ptr, size_t size);

int main()
{
  int a[2];
  __VERIFIER_nondet_memory(a, sizeof(a));
  if (a[0] == 0x01020304 && a[1] == -1)
    assert(0);
  return 0;
}
// Imported from ESBMC regression/witnesses/test_case_generation/testcomp_gen_nondet_memory_array
// Needs: --generate-testcase CHECK_FILE testcase.xml contains <testcase coversError="true">\n<input>4</input>\n<input>3</input>\n<input>2</input>\n<input>1</input>\n<input>255</input>\n<input>255</input>\n<input>255</input>\n<input>255</input>\n</testcase>
// RUN: --generate-testcase --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
