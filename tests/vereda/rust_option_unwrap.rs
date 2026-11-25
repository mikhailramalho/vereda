// EXPECT: ^VERIFICATION FAILED$
// EXPECT: Rust panic
fn main() {
    let x: Option<i32> = None;
    x.unwrap();
}
