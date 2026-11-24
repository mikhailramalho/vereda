// An __int128 that fits 64 bits gets a constant, though the ABI returns it
// as { i64, i64 }; one that does not has no C constant and gets no
// waypoint.
// RUN: --witness -
// EXPECT: value: "\\\\result == -3"(\n.*){3}\n +line: 14\n
// EXPECT-NOT: line: 15\n
// EXPECT: VERIFICATION FAILED
extern void __assert_fail(const char *, const char *, unsigned, const char *);
void reach_error(void) { __assert_fail("0", "witness_int128.c", 3, "reach_error"); }
extern __int128 __VERIFIER_nondet_int128(void);
extern unsigned __int128 __VERIFIER_nondet_uint128(void);
int main(void) {
  // Each call has a line of its own so that its waypoint shows by its line.
  __int128 i = __VERIFIER_nondet_int128();
  unsigned __int128 u = __VERIFIER_nondet_uint128();
  if (i == -3 && u == (unsigned __int128)1 << 100)
    reach_error();
  return 0;
}
