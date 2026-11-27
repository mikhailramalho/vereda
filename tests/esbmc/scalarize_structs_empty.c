/* A struct with no members has no field to split into, so
 * --scalarize-structs (#7521) leaves it alone and splits only the other. */
struct empty
{
};

struct pair
{
  int x;
  int y;
};

int main(void)
{
  struct empty a, b;
  a = b;
  struct pair p = {1, 2};
  __VERIFIER_assert(p.x + p.y == 3); // sum
  return 0;
}
// Imported from ESBMC regression/esbmc/scalarize_structs_empty
// RUN: --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^Scalarized 1 struct variable\(s\)$
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict SUCCESSFUL
