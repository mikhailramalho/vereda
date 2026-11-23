struct tickets
{
  unsigned short head, tail;
};
union ticketpair
{
  unsigned int head_tail;
  struct tickets t;
};
struct raw_spinlock
{
  union ticketpair raw_lock;
  unsigned int magic;
  void *owner;
};
union spinlock
{
  struct raw_spinlock rlock;
};
struct port
{
  union spinlock lock;
  unsigned int rx, tx;
  unsigned long mapbase;
};
struct dev
{
  struct port port;
  unsigned long base;
};
static struct dev devs[2];
int main(void)
{
  int i = __VERIFIER_nondet_int();
  __VERIFIER_assume(i >= 0 && i < 2);
  unsigned long p = (unsigned long)devs + (unsigned long)i * sizeof(struct dev);
  *(unsigned long *)(p + __builtin_offsetof(struct dev, port.mapbase)) = 1;
  *(unsigned int *)(p + __builtin_offsetof(struct dev, port.tx)) = 7;
  __VERIFIER_assert(devs[i].port.tx == 7); // tx stored
  __VERIFIER_assert(devs[i].port.lock.rlock.magic == 7); // lock written
  return 0;
}
// Imported from ESBMC regression/esbmc/intptr_struct_store_union_z3_fail
// Needs: --timeout 60s
// RUN: --z3 --no-pointer-check --no-bounds-check --timeout 60s
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
