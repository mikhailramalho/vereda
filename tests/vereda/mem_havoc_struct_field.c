// EXPECT: ^VERIFICATION SUCCESSFUL$
// make() returns { ptr, i64 } with the i64 set only where malloc succeeds,
// so the joined struct is ite(ok, constant, concat(nondet, null)). Its
// pointer field is a choice of the heap object and null, not a pointer read
// from memory: the call without a body reaches the second allocation through
// v, but not the slot holding b, which g exposed (#499).
#include <stdlib.h>
struct r { int *p; long n; };
static struct r make(void) {
  struct r x;
  x.p = malloc(sizeof(int));
  if (x.p)
    x.n = 4;
  return x;
}
void touch(int **q);
int **g;
void keep(int **q) { g = q; }
int main(void) {
  int *b = make().p;
  keep(&b);
  int *v = make().p;
  touch(&v);
  free(b);
  return 0;
}
