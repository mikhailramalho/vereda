#include <assert.h>
#include <string.h>
int main()
{
  int z, *q;
  {
    int *p = &(int){0};
    memcpy(p, &z, sizeof(z));
    assert(*p == z);
    q = p;
  }
  assert(*q == z);
}
// Imported from ESBMC regression/esbmc/github_591-3
// EXPECT: dereference failure: accessed expired variable pointer
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: dereference failure: accessed expired variable pointer
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
