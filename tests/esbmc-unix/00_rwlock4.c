#include <pthread.h>
#include <assert.h>
#include <errno.h>

pthread_rwlock_t lock;

void *reader_thread(void *arg)
{
    int ret;
    ret = pthread_rwlock_tryrdlock(&lock);
    assert(ret == 0 || ret == EBUSY);
    pthread_rwlock_unlock(&lock);
    return NULL;
}

void *writer_thread(void *arg)
{
    int ret;
    ret = pthread_rwlock_trywrlock(&lock);
    assert(ret == 0 || ret == EBUSY);
    ret = pthread_rwlock_trywrlock(&lock);
    assert(ret == EBUSY);
    ret = pthread_rwlock_tryrdlock(&lock);
    pthread_rwlock_unlock(&lock);
    return NULL;
}

int main()
{
    pthread_rwlock_init(&lock, NULL);
    pthread_t t1, t2;
    pthread_create(&t1, NULL, reader_thread, NULL);
    pthread_create(&t2, NULL, writer_thread, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}
// Imported from ESBMC regression/esbmc-unix/00_rwlock4
// Needs: --no-por --context-bound 3
// RUN: --no-por --context-bound 3
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
