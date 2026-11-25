// EXPECT: ^VERIFICATION FAILED$
// EXPECT: Rust panic
fn main() {
    let x: Result<i32, i32> = Err(7);
    x.unwrap();
}
