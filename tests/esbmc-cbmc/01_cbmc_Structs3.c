int main() {
  struct
  {
    int a, b;
  } s, q;

  s=q;

  assert(s.a==q.a);
}
// Imported from ESBMC regression/cbmc/01_cbmc_Structs3
// EXPECT: ^VERIFICATION SUCCESSFUL$
