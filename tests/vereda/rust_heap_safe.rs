// Box and vec! allocate through the allocator shims; a failed allocation
// aborts in handle_alloc_error, which is no panic.
// EXPECT: ^VERIFICATION SUCCESSFUL$
fn main() {
    let b = Box::new(5);
    let v = vec![1, 2, 3];
    assert!(*b + v[0] + v[2] == 9);
}
