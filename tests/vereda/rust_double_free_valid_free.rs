// RUN: --property valid-free
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: dereference failure: invalidated dynamic object freed
// EXPECT: ^FALSE_FREE$
// Frees under Rust's allocator are checked (#357): a double free in unsafe
// Rust is reported.
fn main() {
    let p = Box::into_raw(Box::new(7i32));
    unsafe {
        std::alloc::dealloc(p.cast(), std::alloc::Layout::new::<i32>());
        std::alloc::dealloc(p.cast(), std::alloc::Layout::new::<i32>());
    }
}
