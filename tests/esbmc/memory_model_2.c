#include <stdlib.h>
#include <assert.h>

_Bool __VERIFIER_nondet_bool();

typedef unsigned short uint16_t;
typedef unsigned char uint8_t;

struct foo
{
  uint16_t bar[2];
  uint8_t baz;
};

int main()
{
  struct foo *quux = (struct foo *)malloc(sizeof(struct foo));
  if(quux == NULL)
    return -1;
  quux->bar[0] = 1;
  quux->bar[1] = 2;
  quux->baz = 'c';
  uint16_t *fuzz;
  fuzz = &quux->bar[0];
  assert(*fuzz == 1);
}
// Imported from ESBMC regression/esbmc/memory_model_2
// EXPECT: ^VERIFICATION SUCCESSFUL$
