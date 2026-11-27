// Bounds 1 and 3 cut the loops, whose unwinding assertions fold to true:
// they ask the resumed solver nothing, so that its first query, at bound 7,
// gets Bitwuzla's full preprocessing, which proves the sums equal in 0.1 s.
// Asking it about the constant checks first took 40 s at bound 7
// (reducercommutativity/sum05-2 in SV-COMP).
// RUN: -m32 --incremental-bmc --property unreach-call
// EXPECT: \[PROGRESS\] Resuming main
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern int __VERIFIER_nondet_int(void);
void reach_error(void);
#define N 5
int sum(int x[N]) {
  long long ret = 0;
  for (int i = 0; i < N; i++)
    ret = ret + x[i];
  return ret;
}
int main(void) {
  int x[N];
  for (int i = 0; i < N; i++)
    x[i] = __VERIFIER_nondet_int();
  int ret = sum(x);
  int temp = x[0];
  x[0] = x[1];
  x[1] = temp;
  int ret2 = sum(x);
  temp = x[0];
  for (int i = 0; i < N - 1; i++)
    x[i] = x[i + 1];
  x[N - 1] = temp;
  int ret5 = sum(x);
  if (ret != ret2 || ret != ret5)
    reach_error();
  return 0;
}