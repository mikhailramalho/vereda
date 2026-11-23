#include <assert.h>

extern int __VERIFIER_nondet_int(void);

int N;

/* Stores through the VLA parameter are encoded as byte updates; their nondet
 * values must still be kept by the slicer and emitted in testcase.xml. */
void init(int x[N])
{
  for (int i = 0; i < N; i++)
    x[i] = __VERIFIER_nondet_int();
}

int main()
{
  N = __VERIFIER_nondet_int();
  if (N == 2)
  {
    int x[N];
    init(x);
    if (x[1] == 5)
      assert(0);
  }
  return 0;
}
// Imported from ESBMC regression/witnesses/test_case_generation/testcomp_gen_vla_param
// Needs: --generate-testcase CHECK_FILE testcase.xml contains <testcase coversError="true">\n<input>2</input>\n<input>-?[0-9]+</input>\n<input>5</input>\n</testcase>
// RUN: --generate-testcase --unwind 3 --no-unwinding-assertions
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
