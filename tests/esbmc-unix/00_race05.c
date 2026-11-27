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
// Imported from ESBMC regression/esbmc-unix/00_race05
// Needs: --ub-data-race-check --context-bound 3
// RUN: --ub-data-race-check --context-bound 3
// EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict none
