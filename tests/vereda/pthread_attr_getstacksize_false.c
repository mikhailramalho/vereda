// EXPECT: ^VERIFICATION FAILED$
// pthread_attr_getstacksize writes sz, which every pthread function was
// taken not to do (#112).
#include <assert.h>
#include <pthread.h>
int main(void) {
  pthread_attr_t a;
  size_t sz = 0;
  pthread_attr_init(&a);
  pthread_attr_getstacksize(&a, &sz);
  assert(sz == 0);
  return 0;
}
