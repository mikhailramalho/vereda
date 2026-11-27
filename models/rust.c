// Models of Rust's global allocator shims, which rustc's allocator shim
// crate defines and the standard library calls; ir renames their mangled
// __rustc:: symbols to these names. They map onto malloc, calloc, realloc
// and free, so heap objects get the walker's model: an allocation may fail
// unless --force-malloc-success, and Rust then calls handle_alloc_error,
// which aborts as std::process::abort does. Objects are 16-byte aligned,
// as malloc's; a layout asking for more is not honoured.
#include <stddef.h>

void *malloc(size_t);
void *calloc(size_t, size_t);
void *realloc(void *, size_t);
void free(void *);

void *__rust_alloc(size_t size, size_t align) { return malloc(size); }
void *__rust_alloc_zeroed(size_t size, size_t align) { return calloc(1, size); }
void *__rust_realloc(void *p, size_t old, size_t align, size_t size) { return realloc(p, size); }
void __rust_dealloc(void *p, size_t size, size_t align) { free(p); }
