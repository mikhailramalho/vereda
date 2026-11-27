/* #5565: fclose() on a standard stream must not report "invalid pointer
 * freed".  stdin/stdout/stderr are not heap objects, but the fclose model
 * called free(stream) unconditionally (outside SV-COMP), so closing stdout --
 * exactly what coreutils' close_stdout atexit handler does -- raised a
 * spurious "invalid pointer freed".  fclose(stdout) is well-defined C. */
#include <stdio.h>

int main(void)
{
  fclose(stdout);
  fclose(stderr);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_5565_fclose_stdout
// Needs: --memory-leak-check --no-reachable-memory-leak
// RUN: --memory-leak-check --no-reachable-memory-leak --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
