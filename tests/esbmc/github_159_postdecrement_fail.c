struct a {
 union {
   struct a *c;
 } d;
};


struct a Q[3];
struct a *b = &Q;
int main() {
  (b--)->d.c;
}
// Imported from ESBMC regression/esbmc/github_159_postdecrement_fail
// MODE: KNOWNBUG
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict SUCCESSFUL
