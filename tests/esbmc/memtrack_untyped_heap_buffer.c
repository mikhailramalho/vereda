#include <stdlib.h>

struct s
{
  int datum;
  struct s *next;
};

struct s *list;

/* The result is not cast at the allocation site, so ESBMC cannot type the
 * object and models it as a flat byte array. */
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
  list->next->next = safe_malloc(sizeof(struct s));
  list->next->next->next = NULL;
  return 0;
}
// Imported from ESBMC regression/esbmc/memtrack_untyped_heap_buffer
// Needs: --memory-leak-check --no-reachable-memory-leak --no-abnormal-memory-leak
// RUN: --memory-leak-check --no-reachable-memory-leak --no-abnormal-memory-leak --force-malloc-success --unwind 2
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
