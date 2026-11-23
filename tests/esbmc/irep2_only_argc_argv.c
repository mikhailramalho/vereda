/* clang_c_main looks up argc'/argv' unconditionally when main takes arguments;
   the symbols are a side effect of the adjust pass, so the sole adjuster owes
   them or the lookup dereferences null. */
int main(int argc, char **argv)
{
  return argc > 0 ? 0 : (int)argv[0][0];
}
// Imported from ESBMC regression/esbmc/irep2_only_argc_argv
// Needs: --clang-c-irep2-adjust-only --symbol-table-only REQUIRES signed_char_host
// RUN: --clang-c-irep2-adjust-only --symbol-table-only
// MODE: FUTURE
// EXPECT: ^Symbol\.+: argc'$
// EXPECT: ^Symbol\.+: argv'$
// EXPECT: ^Type\.+: signed char \* \[argc \+ 1\]$
// Vereda: verdict none
