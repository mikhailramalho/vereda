#include <assert.h>
#include <stddef.h>

struct S
{
  int a;
  char b[8];
  int c;
};

enum
{
  v = offsetof(struct S, c)
};

static const size_t offsets[] = {offsetof(struct S, a), offsetof(struct S, c)};

int main(void)
{
  assert(offsets[1] == v);
  assert(v == 8);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_7466_fail
// Needs: --std gnu17
// RUN: --std gnu17
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: line 21 +assertion
// Vereda: verdict none
