// Pairs with github_7858: the assumption for the named local IS emitted on the
// same run. A fix for #7858 must add d->devnum without losing this.
#include <stdlib.h>

extern int __VERIFIER_nondet_int(void);

struct dev
{
  int devnum;
  int other;
};

static int create_pipe(struct dev *d)
{
  return d->devnum << 8;
}

int main(void)
{
  struct dev *d = malloc(sizeof(struct dev));
  if (!d)
    return 0;

  int local = __VERIFIER_nondet_int();
  if (local == 7)
    return create_pipe(d);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_7858-named-local
// Needs: --witness-output-graphml
// RUN: -m64 --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check --witness-output-graphml -
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: key="assumption">local == 7;
// EXPECT: (?s)\A(?!.*key="assumption">[^<]*(dynamic_|symex_|&amp;))
// MODE: FUTURE
// Vereda: verdict none
