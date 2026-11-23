#include <stdlib.h>

struct s
{
  int datum;
  struct s *next;
};

struct s *list;

void *safe_malloc(size_t size)
{
  void *p = malloc(size);
  if (p == 0)
    abort();
  return p;
}

int main()
{
  list = safe_malloc(sizeof(struct s));
  list->next = safe_malloc(sizeof(struct s));
  /* The only reference to the second object is dropped: a real leak, which
   * reading the buffer's contents must still report. */
  list->next = NULL;
  return 0;
}
// Imported from ESBMC regression/esbmc/memtrack_untyped_heap_buffer_fail
// Needs: --memory-leak-check --no-reachable-memory-leak --no-abnormal-memory-leak
// RUN: --memory-leak-check --no-reachable-memory-leak --no-abnormal-memory-leak --force-malloc-success --unwind 2
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^.*forgotten memory.*$
// Vereda: verdict none
