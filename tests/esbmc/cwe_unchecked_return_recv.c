#include <sys/socket.h>

int main(void)
{
  char buf[64];
  long n = recv(0, buf, sizeof(buf), 0);
  return (int)(n + 1);
}
// Imported from ESBMC regression/esbmc/cwe_unchecked_return_recv
// Needs: --unchecked-return-value-check
// RUN: --unchecked-return-value-check --incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^  unchecked return value of recv: n$
// EXPECT: ^  CWE: CWE-252$
// Vereda: verdict none
