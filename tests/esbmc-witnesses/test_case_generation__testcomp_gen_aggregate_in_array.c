#include <assert.h>

extern int __VERIFIER_nondet_int(void);

struct pair
{
  int a, b;
};

struct pair arr[3];

int main()
{
  /* The element's struct literal sits inside a WITH on the array:
   * `arr == (... WITH [1:={ .a=nondet1, .b=nondet0 }])`. */
  arr[1].b = __VERIFIER_nondet_int();
  arr[1].a = __VERIFIER_nondet_int();
  if (arr[1].a == 4)
    assert(0);
  return 0;
}
// Imported from ESBMC regression/witnesses/test_case_generation/testcomp_gen_aggregate_in_array
// Needs: --generate-testcase CHECK_FILE testcase.xml contains <testcase coversError="true">\n<input>-?[0-9]+</input>\n<input>4</input>\n</testcase>
// RUN: --generate-testcase --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
