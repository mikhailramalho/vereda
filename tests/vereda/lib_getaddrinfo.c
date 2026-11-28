// getaddrinfo fails with EAI_NONAME without node and service and with
// EAI_BADFLAGS for AI_CANONNAME without a node; otherwise it gives a list
// of new objects, the first with a canonical name under AI_CANONNAME,
// which freeaddrinfo frees (#503).
// RUN: --unwind 3 --no-unwinding-assertions
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <assert.h>
#include <netdb.h>
#include <string.h>

int main(void) {
  struct addrinfo h, *res;
  memset(&h, 0, sizeof h);
  assert(getaddrinfo(NULL, NULL, NULL, &res) == EAI_NONAME);
  h.ai_flags = AI_CANONNAME;
  assert(getaddrinfo(NULL, "80", &h, &res) == EAI_BADFLAGS);
  int r = getaddrinfo("localhost", "80", &h, &res);
  if (r) {
    assert(r < 0);
    return 0;
  }
  assert(res->ai_canonname && res->ai_addrlen <= sizeof(struct sockaddr_storage));
  if (res->ai_next)
    assert(!res->ai_next->ai_canonname);
  freeaddrinfo(res);
  return 0;
}
