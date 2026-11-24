// A call without a body may return a pointer it was passed (#137).
// EXPECT: ^VERIFICATION FAILED$
extern void __VERIFIER_assert(int);
extern char *find(char *, int);

int main(void) {
  char s[4] = "abc";
  char *p = find(s, 'b');
  if (p)
    *p = 'x';
  __VERIFIER_assert(s[1] == 'b');
  return 0;
}
