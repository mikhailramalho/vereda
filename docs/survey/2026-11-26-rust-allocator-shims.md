# Rust allocator shims

Recorded 2026-11-26.

With rustc 1.100.0 at `-C opt-level=0 -C panic=abort`, `Box::new` and a
`vec!` literal allocate through `__rust_alloc`, `__rust_alloc_zeroed`,
`__rust_realloc` and `__rust_dealloc`, which rustc emits as v0 symbols in a
`__rustc` crate whose hash depends on the compiler build
(`_RNvCs5WqGp7zLRnA_7___rustc12___rust_alloc`). Without a body the call
returned any pointer, null included, so the standard library reached
`handle_alloc_error`, a bodyless noreturn alloc call that the walker counted
as a panic: `fn main() { let b = Box::new(5); assert!(*b == 5); }` gave
VERIFICATION FAILED.

ir now renames every declared `__rustc::__rust_*` function to its plain
name, and `models/rust.c` defines the four allocator shims over malloc,
calloc, realloc and free. Rust heap objects so get the same model as C's:
16-byte aligned, and an allocation may fail unless `--force-malloc-success`,
as ESBMC's malloc. Rust's default allocation error handler prints a message
and aborts, so the walker treats `alloc::alloc::handle_alloc_error` as it
treats `std::process::abort`: the path ends without a violation. Making
Rust allocation never fail was not adopted: it would differ from C's
malloc for no gain, since a failed allocation is no property violation
either way. A layout aligned to more than 16 bytes is not honoured.

The `exits` test matched on the name before its `::h` hash split, which cuts
`alloc::alloc::handle_alloc_error` at `::handle`; the new entry matches the
whole demangled name.

Before and after, with the vec! literal, Box and nondet-index probes:
`vec![1, 2, 3]` with `assert!(v[0] == 1)` and `Box::new(5)` went from FAILED
(Rust panic in box_new_uninit) to SUCCESSFUL; indexing that vector at a
nondet index stays FAILED, now with array bounds violated in the slice index
instead of the false allocation panic.

Not fixed here: `Vec::push`, `String::from` and `vec![0; n]` call
`RawVecInner::grow_amortized` or `RawVecInner::try_allocate_in`, which are
compiled into the precompiled alloc crate and have no body in the program's
IR. `grow_amortized` returns a struct, so the result is VERIFICATION
UNKNOWN; `try_allocate_in` returns nondet values and gives a false panic.
Both need the standard library's bodies or models of those internals, not
allocator models.

Follow-up issues: #334
