#include <assert.h>
#include <stddef.h>
struct desc
{
  void (*init)(int *);
  int n;
};
static void init_fn(int *v) { *v = 1; }
const struct desc the_desc = {init_fn, 4};

int big(const struct desc *d)
{
  int sum = 0;
  for (int i = 0; i < 1000; i++)
    sum += d->n;
  return sum;
}

int unpack(const struct desc *d)
{
  __VERIFIER_assume(d == &the_desc);
  int v = 0;
  if (d->init != NULL)
    d->init(&v);
  else
    v = big(d);
  return v;
}

int main(void)
{
  const struct desc *d;
  assert(unpack(d) == 2);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_8517_assume_pins_pointer_fail
// RUN: --unwind 2
// EXPECT: ^VERIFICATION FAILED$
