// Writing one past the three ints the calloc paid for. The flexible array
// member carries no length of its own, so the violation is only visible against
// the heap object's size. #133
#include <stdlib.h>

struct A
{
  char alloc;
  int b[];
};

int main()
{
  struct A *e = calloc(1, sizeof(struct A) + 3 * sizeof(int));
  if (!e)
    return 0;

  e->b[3] = 1;

  free(e);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_133_flex_array_fail
// EXPECT: ^  dereference failure: array bounds violated: heap object$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^  dereference failure: array bounds violated: heap object$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
