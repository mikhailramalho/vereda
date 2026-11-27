typedef struct
{
  char c;
} fam;

typedef struct
{
  short classes[4];
  short e;
  fam f[];
} g;

g h;

int main()
{
  (&h)->classes[h.e];
  return 0;
}
// Imported from ESBMC regression/esbmc/github_3418_minimal
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
