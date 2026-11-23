#include <assert.h>
#include <stddef.h>

/* clang lays a long double out with its own width; where it is double
 * (AArch64 Darwin, Windows) that is 8 bytes, not the data model's 16. */
struct S
{
  long double ld;
  char e;
};

int main(void)
{
  struct S s;
  assert(offsetof(struct S, e) == sizeof(long double));
  assert((char *)&s.e - (char *)&s == offsetof(struct S, e));
  struct S *back = (struct S *)((char *)&s.e - offsetof(struct S, e));
  assert(back != &s);
  return 0;
}
// Imported from ESBMC regression/esbmc/long_double_member_layout_fail
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: line 18 +assertion
// Vereda: unmet EXPECT: line 18 +assertion
// MODE: FUTURE
// Vereda: verdict FAILED
