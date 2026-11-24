union lock
{
  unsigned int word;
  struct
  {
    unsigned short head, tail;
  } half;
};
struct counts
{
  unsigned int rx, tx;
};
struct port
{
  union lock lock;
  struct counts counts;
  unsigned long base;
};
static struct port ports[2];
int main(void)
{
  int i = __VERIFIER_nondet_int();
  __VERIFIER_assume(i >= 0 && i < 2);
  unsigned long p = (unsigned long)ports + (unsigned long)i * sizeof(struct port);
  *(unsigned short *)(p + __builtin_offsetof(struct port, lock.half.tail)) = 5;
  *(unsigned int *)(p + __builtin_offsetof(struct port, counts.tx)) = 7;
  __VERIFIER_assert(ports[i].lock.half.tail == 5); // tail stored
  __VERIFIER_assert(ports[i].lock.half.head == 0); // head untouched
  __VERIFIER_assert(ports[i].counts.tx == 7); // tx stored
  __VERIFIER_assert(ports[i].counts.rx == 0); // rx untouched
  __VERIFIER_assert(ports[i].lock.half.head == 5); // head written
  return 0;
}
// Imported from ESBMC regression/esbmc/intptr_struct_store_nested_union_fail
// RUN: --no-pointer-check
// EXPECT: ^VERIFICATION FAILED$
