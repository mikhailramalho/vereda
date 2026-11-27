// As global_promote_loops, but the last loop reads a[500], which is 0: the
// summarised loops still find the failure.
// EXPECT: ^\[INFO\] Summarised 3 array loop\(s\)$
// EXPECT: ^VERIFICATION FAILED$
extern void __VERIFIER_assert(int);
int a[1000];
int i;
int main(void) {
  for (i = 0; i < 1000; i++)
    a[i] = 0;
  for (i = 0; i < 500; i++)
    a[i] = 10;
  for (i = 0; i < 501; i++)
    __VERIFIER_assert(a[i] == 10);
  return 0;
}
