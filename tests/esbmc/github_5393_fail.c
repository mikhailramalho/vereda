/* #5393: a flexible array member adds nothing to the size of its structure
 * (C17 6.7.2.1p18), so the byte at offset sizeof(struct s) lies past the end
 * of a `struct s` local. Sizing the member as a one-element array hides this
 * overflow. */
struct s
{
  void *a;
  long slots[];
};

int main(void)
{
  struct s t;
  t.a = 0;
  char *c = (char *)&t;
  c[sizeof(struct s)] = 1;
  return 0;
}
// Imported from ESBMC regression/esbmc/github_5393_fail
// RUN: -m64 --no-incremental-bmc
// EXPECT: ^\s*FAILED\s+\[.*\]\s+line\s+[0-9]+\s+dereference failure: Access to object out of bounds$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^\s*FAILED\s+\[.*\]\s+line\s+[0-9]+\s+dereference failure: Access to object out of bounds$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
