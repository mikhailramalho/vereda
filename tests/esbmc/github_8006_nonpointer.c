// #8006
void *malloc(unsigned long);
void free(long);
int main()
{
  int *p = malloc(sizeof(int));
  if (!p) return 0;
  long x = (long)p;
  *p = 1;
  int a = *p + 1;
  free(x);
  int b = *p + 1;
  return a + b;
}
// Imported from ESBMC regression/esbmc/github_8006_nonpointer
// Needs: --gcse --goto-functions-only
// RUN: --gcse --goto-functions-only
// MODE: FUTURE
// EXPECT: ^\s*OTHER FREE\(x\);$
// EXPECT: ^\s*ASSIGN b=\*p \+ 1;$
// Vereda: verdict none
