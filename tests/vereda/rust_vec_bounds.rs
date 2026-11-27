// EXPECT: ^VERIFICATION FAILED$
// EXPECT: array bounds violated
fn main() {
    let v = vec![1, 2, 3];
    let i = v.len();
    let _ = v[i];
}
