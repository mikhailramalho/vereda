/* An empty union member has no size either, so the zero-size skip in
 * construct_from_const_struct_offset must accept it and address the next
 * member at the same offset (#5393). */
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
  t.a = 7;
  char *c = (char *)&t;
  __VERIFIER_assert(c[0] == 7 || c[0] != 7); // reachable
  return 0;
}
// Imported from ESBMC regression/esbmc/empty_union_member_deref
// RUN: -m64 --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
