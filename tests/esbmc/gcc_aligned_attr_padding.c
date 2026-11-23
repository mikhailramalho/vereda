struct __attribute__((aligned(16))) s
{
  int a;
  char c;
};

extern struct s g;

int main(void)
{
  return g.a;
}
// Imported from ESBMC regression/esbmc/gcc_aligned_attr_padding
// Needs: --goto-functions-only REQUIRES signed_char_host
// RUN: --goto-functions-only
// MODE: FUTURE
// EXPECT: struct s \{ signed int a; signed char c; unsigned _ExtInt\(88\) anon_pad#2; \}
// Vereda: verdict none
