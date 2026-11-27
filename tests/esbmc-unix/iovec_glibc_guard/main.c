#include <sys/socket.h>
#include <fcntl.h>

int main(void)
{
    return 0;
}
// Imported from ESBMC regression/esbmc-unix/iovec_glibc_guard
// Needs: -I wrap
// RUN: -I wrap -D_GNU_SOURCE
// EXPECT: ^VERIFICATION SUCCESSFUL$
