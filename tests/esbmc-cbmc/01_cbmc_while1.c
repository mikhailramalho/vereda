void cmp_str(char* s1, char* s2, int n) {

  while ((n > 0) && (*s1 == *s2)) {
    s1++;
    s2++;
    n--;
  }

  assert ((n <= 0) || (*s1 != *s2));
}

#define MAX 3

int main () {
  char a[MAX];
  char b[MAX];
  a[0] = 2;
  b[0] = 2;
  
  for (int i = 1; i < MAX; i++) {
    a[i] = __VERIFIER_nondet_char();
    b[i] = __VERIFIER_nondet_char();
  }

  cmp_str(a, b, MAX);
}
// Imported from ESBMC regression/cbmc/01_cbmc_while1
// EXPECT: ^VERIFICATION SUCCESSFUL$
