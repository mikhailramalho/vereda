#include <unistd.h>
#include <assert.h>

int main(void) {
    // Call _exit() directly
    _exit(42);

    // Unreachable: program terminates before this line
    assert(0 && "_exit did not terminate as expected");

    return 0;
}

// Imported from ESBMC regression/esbmc-unix/exit2
// EXPECT: ^VERIFICATION SUCCESSFUL$
