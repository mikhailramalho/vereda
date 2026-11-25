// EXPECT: ^VERIFICATION FAILED$
// EXPECT: array bounds violated
fn main() {
    let a = [1, 2];
    let n = 3;
    let _ = &a[..n];
}
