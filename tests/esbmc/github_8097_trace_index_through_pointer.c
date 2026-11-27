/* #8097: building the counterexample asked the solver for the index
 * s->a[i], which still holds a dereference, and aborted. */
typedef struct
{
  unsigned a[2];
  unsigned b[2];
} S;

unsigned __VERIFIER_nondet_uint(void);

void f(S *s)
{
  for (int i = 0; i < 2; i++)
  {
    s->a[i] = __VERIFIER_nondet_uint() % 2;
    s->b[s->a[i]] = i + 1;
  }
}

int main(void)
{
  S s = {{0, 0}, {0, 0}};
  f(&s);
  __VERIFIER_assert(s.b[0] + s.b[1] <= 3); // b sums to at most 3
  return 0;
}
// Imported from ESBMC regression/esbmc/github_8097_trace_index_through_pointer
// EXPECT: ^VERIFICATION SUCCESSFUL$
