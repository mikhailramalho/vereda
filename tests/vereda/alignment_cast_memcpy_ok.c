// Copying unaligned bytes with memcpy or memmove is how C reads them; a
// struct copied through a cast at an aligned offset, a packed struct copied
// through a cast at any offset, packed members, bitfields of a packed struct
// and members of packed unions in an array are aligned too, although clang
// gives each an alignment below its IR type's.
// RUN: --ub-alignment-check
// RUN: --ub-alignment-check -m32
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <stdint.h>
#include <string.h>
unsigned __VERIFIER_nondet_uint(void);
void __VERIFIER_assume(int);
struct N { uint32_t v; };
struct __attribute__((packed)) P { char c; struct N n; };
struct __attribute__((packed)) U { uint32_t v; };
struct __attribute__((packed)) B { char a; unsigned char b : 2, c : 7; };
union __attribute__((packed)) V { int i; char c; };
struct B bits;
int main(void) {
  uint32_t buf[2] = {1, 2};
  unsigned o = __VERIFIER_nondet_uint();
  __VERIFIER_assume(o <= 4);
  struct N a;
  memcpy(&a, (unsigned char *)buf + o, sizeof a);
  uint32_t b;
  memmove(&b, (unsigned char *)buf + o, sizeof b);
  __builtin_memcpy(&b, (unsigned char *)buf + o, sizeof b);
  struct U u = {3};
  *(struct U *)((unsigned char *)buf + o) = u;
  struct P p = {'c', {3}};
  struct N c = p.n;
  p.n = a;
  bits.c = 5;
  union V vs[2] = {{0}, {0}};
  vs[1].i = 4;
  __VERIFIER_assume(o % 4 == 0);
  struct N d = *(struct N *)((unsigned char *)buf + o);
  return a.v + b + c.v + d.v + bits.c + vs[1].i;
}
