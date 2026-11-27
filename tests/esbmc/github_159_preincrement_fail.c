struct a {
 union {
   struct a *c;
 } d;
};


struct a Q[3];
struct a *b = &Q + 2;
int main() {
  (++b)->d.c;
}
// Imported from ESBMC regression/esbmc/github_159_preincrement_fail
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
