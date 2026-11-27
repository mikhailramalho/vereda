struct a {
 union {
   struct a *c;
 } d;
};


struct a Q[3];
struct a *b = &Q + 3;
int main() {
  (b++)->d.c;
}
// Imported from ESBMC regression/esbmc/github_159_postincrement_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
