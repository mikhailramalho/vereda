/* declare_argc_argv reads main's type off the IREP2 side, so on the default
   path -- where clang_c_convert wrote that type legacy -- the symbol's lazy
   forward migration has to preserve the argument types (esbmc/esbmc#4715). */
int main(int argc, char **argv)
{
  return argc > 0 ? 0 : (int)argv[0][0];
}
// Imported from ESBMC regression/esbmc/argc_argv_type
// Needs: --symbol-table-only REQUIRES signed_char_host
// RUN: --symbol-table-only
// MODE: FUTURE
// EXPECT: ^Symbol\.+: argc'$
// EXPECT: ^Symbol\.+: argv'$
// EXPECT: ^Type\.+: signed char \* \[argc \+ 1\]$
// Vereda: verdict none
