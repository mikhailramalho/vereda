#include <assert.h>
#include <stddef.h>

extern void __VERIFIER_nondet_memory(void *ptr, size_t size);

union word
{
  int i;
  float f;
  char c[6];
};

int main()
{
  /* The model gives every member as a view of the same bytes; c, the widest,
   * covers them all. */
  union word w;
  __VERIFIER_nondet_memory(&w, sizeof(w));
  if (w.i == 0x01020304 && w.c[5] == 9)
    assert(0);
  return 0;
}
// Imported from ESBMC regression/witnesses/test_case_generation/testcomp_gen_nondet_memory_union
// Needs: --generate-testcase CHECK_FILE testcase.xml contains <testcase coversError="true">\n<input>4</input>\n<input>3</input>\n<input>2</input>\n<input>1</input>\n<input>[0-9]+</input>\n<input>9</input>\n<input>[0-9]+</input>\n<input>[0-9]+</input>\n</testcase>
// RUN: --generate-testcase
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
