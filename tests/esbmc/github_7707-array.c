#include <stdint.h>

/* #7707 for an element of an array of packed structs: the array's base is
 * unconstrained, so the load really can be misaligned. */

struct __attribute__((packed)) S
{
  char a;
  char pad[7];
  uint64_t b;
};

struct S arr[4];

int main(void)
{
  uint64_t *p = (uint64_t *)&arr[1].b;
  uint64_t z = *p;
  (void)z;
  return 0;
}
// Imported from ESBMC regression/esbmc/github_7707-array
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^.*dereference failure: Incorrect alignment when accessing data object.*$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
