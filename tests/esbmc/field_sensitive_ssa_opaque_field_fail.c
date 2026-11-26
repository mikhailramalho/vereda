#include <assert.h>

int __VERIFIER_nondet_int(void);

struct Inner
{
  int a[4];
  int n;
};

struct Outer
{
  struct Inner in;
  int pad;
};

int main(void)
{
  struct Inner tmp;
  tmp.a[0] = 1;
  tmp.a[1] = 2;
  tmp.a[2] = 3;
  tmp.a[3] = 4;
  int x = __VERIFIER_nondet_int();
  tmp.n = x + 1;
  struct Outer o;
  int *p = &o.pad;
  o.in = tmp;
  int i = 0;
  while (i < o.in.a[3])
    i++;
  assert(i == 4);
  assert(o.in.n == x);
  return 0;
}
// Imported from ESBMC regression/esbmc/field_sensitive_ssa_opaque_field_fail
// RUN: --unwind 6
// EXPECT: ^VERIFICATION FAILED$
