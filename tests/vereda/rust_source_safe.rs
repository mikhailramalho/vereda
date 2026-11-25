// EXPECT: ^VERIFICATION SUCCESSFUL$
fn main() {
    let x = Some(4);
    let a = [x.unwrap(), 5];
    assert_eq!(a[..1][0] + 1, 5);
}
