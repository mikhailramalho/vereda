/* Padding is part of the layout, not a spelling: the symbol table's type is
   what ESBMC sizes objects and computes member offsets from. */
struct s
{
  int a;
  char c;
};

struct t
{
  char c;
  int a;
};

extern struct s g;
extern struct t h;

int main(void)
{
  return g.a + h.a;
}
// Imported from ESBMC regression/esbmc/irep2_only_struct_padding
// Needs: --goto-functions-only --clang-c-irep2-adjust-only REQUIRES signed_char_host
// RUN: --goto-functions-only --clang-c-irep2-adjust-only
// MODE: FUTURE
// EXPECT: struct s \{ signed int a; signed char c; unsigned _ExtInt\(24\) anon_pad#2; \}
// EXPECT: struct t \{ signed char c; unsigned _ExtInt\(24\) anon_pad#1; signed int a; \}
// Vereda: verdict none
