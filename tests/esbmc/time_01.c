#include <time.h>

int main()
{
	time(NULL);
	return 0;
}

// Imported from ESBMC regression/esbmc/time_01
// EXPECT: ^VERIFICATION SUCCESSFUL$
