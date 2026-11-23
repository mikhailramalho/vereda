/* The three-argument form: envp'/envp_size' come from the same side effect. */
int main(int argc, char **argv, char **envp)
{
  return argc > 0 ? 0 : (int)envp[0][0];
}
// Imported from ESBMC regression/esbmc/irep2_only_argc_argv_envp
// Needs: --clang-c-irep2-adjust-only --symbol-table-only REQUIRES signed_char_host
// RUN: --clang-c-irep2-adjust-only --symbol-table-only
// MODE: FUTURE
// EXPECT: ^Symbol\.+: envp_size'$
// EXPECT: ^Symbol\.+: envp'$
// EXPECT: ^Type\.+: signed char \* \[envp_size\]$
// Vereda: verdict none
