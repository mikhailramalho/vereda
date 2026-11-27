int __VERIFIER_nondet_int(void);

struct
{
  int a : 3;
} b = {3};

int main()
{
  // C11 6.5.16.2p3: the addition runs in the promoted type, so the overflow is
  // checked as it is on a plain member; see overflow_plain_member.
  int a = __VERIFIER_nondet_int();
  b.a += a;
}
// Imported from ESBMC regression/esbmc/overflow_bitfield_member
// RUN: --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check
// EXPECT: ^VERIFICATION FAILED$
