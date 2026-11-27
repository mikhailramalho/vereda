union a {
  int : 5
};

int main() {
  union a a = {};
  return 0;
}
// Imported from ESBMC regression/esbmc/union_bitfield_0
// EXPECT: ^VERIFICATION SUCCESSFUL$
