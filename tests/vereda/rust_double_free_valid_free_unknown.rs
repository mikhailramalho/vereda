// RUN: --property valid-free
// EXPECT: ^VERIFICATION UNKNOWN: unsupported: free under Rust's allocator is not checked
// EXPECT: ^UNKNOWN$
// EXPECT-NOT: ^TRUE$
// Frees under Rust's allocator are not checked (#357), so a double free in
// unsafe Rust must not leave valid-free with nothing to decide and answer TRUE.
fn main() {
    let p = Box::into_raw(Box::new(7i32));
    unsafe {
        std::alloc::dealloc(p.cast(), std::alloc::Layout::new::<i32>());
        std::alloc::dealloc(p.cast(), std::alloc::Layout::new::<i32>());
    }
}
