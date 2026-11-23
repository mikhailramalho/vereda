/* The literal must be non-zero: is_recursively_zero returns before the member
   walk reaches the padding test. `g` is extern so the dump prints the type. */
struct s
{
  char a;
  int c;
};

extern struct s g;

int main(void)
{
  struct s v = {1, 2};
  return v.c + g.a;
}
// Imported from ESBMC regression/esbmc/irep2_only_struct_pad_not_printed
// Needs: --goto-functions-only --clang-c-irep2-adjust-only REQUIRES signed_char_host
// RUN: --goto-functions-only --clang-c-irep2-adjust-only
// MODE: FUTURE
// EXPECT: struct s \{ signed char a; unsigned _ExtInt\(24\) anon_pad#1; signed int c; \}
// EXPECT: ASSIGN v=\{ \.a=1, \.c=2 \};
// Vereda: verdict none
