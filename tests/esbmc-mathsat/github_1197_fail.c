#include <pthread.h>

void * run(void *);

int main()
{
	pthread_t t1;
	pthread_create(&t1, NULL, run, NULL);
	pthread_detach(t1);
	pthread_detach(t1);   /* genuine double detach: MUST be reported */
}
// Imported from ESBMC regression/mathsat/github_1197_fail
// RUN:
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: already detached
// MODE: FUTURE
// Vereda: verdict UNKNOWN
