// freeaddrinfo frees every entry of the list, so freeing the second one
// after it is a double free where the list has more than one (#503).
// RUN: --unwind 3 --no-unwinding-assertions
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^  FAILED +\[main\.invalidated-dynamic-object-freed\.3\]
#include <netdb.h>
#include <stdlib.h>

int main(void) {
  struct addrinfo *res;
  if (getaddrinfo("localhost", "80", NULL, &res))
    return 0;
  struct addrinfo *next = res->ai_next;
  freeaddrinfo(res);
  free(next);
  return 0;
}
