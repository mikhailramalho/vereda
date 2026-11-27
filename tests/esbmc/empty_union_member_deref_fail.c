/* The byte at offset sizeof(struct s) is past the end: the empty union member
 * contributes no storage, so the access must be reported out of bounds rather
 * than aborting the zero-size skip (#5393). */
union e
{
};

struct s
{
  union e u;
  long a;
};

int main(void)
{
  struct s t;
  t.a = 0;
  char *c = (char *)&t;
  c[sizeof(struct s)] = 1;
  return 0;
}
// Imported from ESBMC regression/esbmc/empty_union_member_deref_fail
// RUN: -m64 --no-incremental-bmc
// EXPECT: ^\s*FAILED\s+\[.*\]\s+line\s+[0-9]+\s+dereference failure: Access to object out of bounds$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^\s*FAILED\s+\[.*\]\s+line\s+[0-9]+\s+dereference failure: Access to object out of bounds$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
