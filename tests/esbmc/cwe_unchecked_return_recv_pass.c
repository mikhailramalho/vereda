#include <sys/socket.h>

int main(void)
{
  char buf[64];
  long n = recv(0, buf, sizeof(buf), 0);
  if (n < 0)
    return 1;
  return (int)(n & 0x7f);
}
// Imported from ESBMC regression/esbmc/cwe_unchecked_return_recv_pass
// Needs: --unchecked-return-value-check
// RUN: --unchecked-return-value-check --incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
