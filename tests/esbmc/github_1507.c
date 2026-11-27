/* #1507: ESBMC crashed parsing this. d.f has no elements, so d.f->b accesses
 * one, which C17 6.7.2.1p18 leaves undefined (#5393). */
struct a {
  long b
};
struct c d;
struct c {
  short e;
  struct a f[]
} main() {
  d.f->b;
}
// Imported from ESBMC regression/esbmc/github_1507
// RUN: --no-incremental-bmc
// EXPECT: ^\s*FAILED\s+\[.*\]\s+line\s+[0-9]+\s+array bounds violated: array upper bound$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^\s*FAILED\s+\[.*\]\s+line\s+[0-9]+\s+array bounds violated: array upper bound$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
