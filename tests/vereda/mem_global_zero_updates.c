// Zeroed globals remain independent through writes, joins and whole copies.
// Padding and null pointers are zero, while partial zero initializers must
// preserve the enclosing object's nonzero fields and pointer provenance.
// RUN: -m32
// RUN: -m64
// RUN: -m64 --ub-uninit-check
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <string.h>
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);

unsigned char a[32], b[32];
int value = 17;
struct Packet {
  unsigned char tag;
  unsigned number;
  int *pointer;
  unsigned char tail[3];
} mixed = {7, 123, &value, {0, 5, 0}}, zero;

__attribute__((noinline)) unsigned char read_at(unsigned char *p, unsigned i) {
  return p[i];
}

__attribute__((noinline)) void write_at(unsigned char *p, unsigned i) {
  p[i] = 9;
}

int main(void) {
  unsigned i = __VERIFIER_nondet_uint(), j = __VERIFIER_nondet_uint();
  unsigned update = __VERIFIER_nondet_uint();
  __VERIFIER_assume(i < sizeof a && j < sizeof a);
  if (update)
    write_at(a, i);
  __VERIFIER_assert(read_at(a, j) == (update && i == j ? 9 : 0));
  __VERIFIER_assert(read_at(b, j) == 0);
  memcpy(b, a, sizeof a);
  __VERIFIER_assert(read_at(b, j) == (update && i == j ? 9 : 0));
  __VERIFIER_assert(zero.pointer == 0 && zero.number == 0 && zero.tag == 0);
  zero.pointer = &value;
  __VERIFIER_assert(*zero.pointer == 17);
  __VERIFIER_assert(mixed.tag == 7 && mixed.number == 123 && *mixed.pointer == 17);
  __VERIFIER_assert(mixed.tail[0] == 0 && mixed.tail[1] == 5 && mixed.tail[2] == 0);
  __VERIFIER_assert(read_at((unsigned char *)&mixed, 1) == 0);
  __VERIFIER_assert(read_at((unsigned char *)&mixed, sizeof mixed - 1) == 0);
}
