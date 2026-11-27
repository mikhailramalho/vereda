#include <assert.h>
#include <stddef.h>

extern void __VERIFIER_nondet_memory(void *ptr, size_t size);
extern int __VERIFIER_nondet_int(void);

struct pair
{
  int x, y;
};

int main()
{
  /* ESBMC havocs the whole struct in one step; the test harness reads one
   * __VERIFIER_nondet_uchar() per byte, so the test case lists its 8 bytes
   * before k. */
  struct pair p = {0, 0};
  __VERIFIER_nondet_memory(&p, sizeof(p));
  int k = __VERIFIER_nondet_int();
  if (p.x == 0x01020304 && p.y == -1 && k == 9)
    assert(0);
  return 0;
}
// Imported from ESBMC regression/witnesses/test_case_generation/testcomp_gen_nondet_memory
// Needs: --generate-testcase CHECK_FILE testcase.xml contains <testcase coversError="true">\n<input>4</input>\n<input>3</input>\n<input>2</input>\n<input>1</input>\n<input>255</input>\n<input>255</input>\n<input>255</input>\n<input>255</input>\n<input>9</input>\n</testcase>
// RUN: --generate-testcase
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
