long a;

struct b {
   long c;
   char bytes[];
};

int main() {
   __VERIFIER_assume(a < ((18446744073709551615UL) - 1 - sizeof(struct b)));
   __VERIFIER_assume(a > 0);
   struct b *d = malloc(sizeof(struct b) + a + 1);
   d->bytes[a] = '\0';
   assert(d->bytes[a] == 0);
}
// Imported from ESBMC regression/esbmc/github_28
// RUN: --force-malloc-success --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
