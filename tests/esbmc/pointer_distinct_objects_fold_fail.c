/* The fold must leave alone any pointer whose object is not fixed: a
 * pointer chosen between two objects may equal either, and a pointer from
 * a dereference may alias anything, including another object's address. */
struct S
{
  int x;
};
struct S s;
int a[4], b[4];
int *q;
int __VERIFIER_nondet_int(void);

int main(void)
{
  int *p = __VERIFIER_nondet_int() ? &a[0] : &b[0];
  q = __VERIFIER_nondet_int() ? &a[2] : &b[2];
  __VERIFIER_assert(p != &a[0]); // a chosen pointer may equal either object
  __VERIFIER_assert(&q[-2] != &b[0]); // a dereference-rooted address may too
  struct S *r = &s;
  __VERIFIER_assert(&r->x != &s.x); // a member through a pointer may too
  return 0;
}
// Imported from ESBMC regression/esbmc/pointer_distinct_objects_fold_fail
// RUN: --multi-property
// EXPECT: ^\s*FAILED\s+\[main\.assertion\.1\]
// EXPECT: ^\s*FAILED\s+\[main\.assertion\.2\]
// EXPECT: ^\s*FAILED\s+\[main\.assertion\.3\]
// EXPECT: ^VERIFICATION FAILED$
