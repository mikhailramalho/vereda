// esbmc/esbmc#7858: d->devnum is read from an object the program never wrote,
// so no trace step carries it and the witness cannot constrain it.
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
// Imported from ESBMC regression/esbmc/github_7858
// Needs: --witness-output-graphml -
// RUN: --64 --overflow-check --witness-output-graphml -
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: key="assumption">d-&gt;devnum == -?\d+;
