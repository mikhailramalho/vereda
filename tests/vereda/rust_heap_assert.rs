// The heap keeps what was stored: a wrong assertion on it fails.
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: Rust panic
fn main() {
    let b = Box::new(5);
    let v = vec![1, 2, 3];
    assert!(*b + v[2] == 7);
}
