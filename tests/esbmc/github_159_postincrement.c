struct a {
 union {
   struct a *c;
 } d;
};


struct a Q[3];
struct a *b = &Q + 2;
int main() {
  (b++)->d.c;
}
// Imported from ESBMC regression/esbmc/github_159_postincrement
// EXPECT: ^VERIFICATION SUCCESSFUL$
