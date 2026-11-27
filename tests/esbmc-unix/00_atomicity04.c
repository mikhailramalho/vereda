/* A simple race */

#include <pthread.h>
#include <unistd.h>

static int shared;

static void *th(void *v)
{
	shared++;

	return 0;
}

int main()
{
	pthread_t a, b;

	pthread_create(&a, NULL, th, NULL);	
	sleep(1);		/* force ordering */
	pthread_create(&b, NULL, th, NULL);

	pthread_join(a, NULL);
	pthread_join(b, NULL);

	return 0;
}
// Imported from ESBMC regression/esbmc-unix/00_atomicity04
// Needs: --atomicity-check --context-bound 3
// RUN: --atomicity-check --context-bound 3 --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
