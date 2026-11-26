#include <pthread.h>

void * run(void *);

int main()
{
	pthread_t t1;
	pthread_create(&t1, NULL, run, NULL);
	pthread_detach(t1);
}
// Imported from ESBMC regression/mathsat/github_1197
// RUN:
// EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict UNKNOWN
