#include <assert.h>
#include <stddef.h>

extern void __VERIFIER_nondet_memory(void *ptr, size_t size);
extern int __VERIFIER_nondet_int(void);

struct pair
{
  int x;
  short y;
};

int main()
{
  /* An interior pointer takes the model's per-byte loop, whose
   * __VERIFIER_nondet_uchar() calls are the harness inputs. */
  struct pair p = {0, 0};
  __VERIFIER_nondet_memory(&p.y, sizeof(p.y));
  int k = __VERIFIER_nondet_int();
  if (p.y == 0x1234 && k == 9)
    assert(0);
  return 0;
}
// Imported from ESBMC regression/witnesses/test_case_generation/testcomp_gen_nondet_memory_partial
// Needs: --generate-testcase CHECK_FILE testcase.xml contains <testcase coversError="true">\n<input>52</input>\n<input>18</input>\n<input>9</input>\n</testcase>
// RUN: --generate-testcase
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
