// EXPECT: ^VERIFICATION FAILED$
// EXPECT: arithmetic overflow
unsafe extern "C" {
    safe fn __VERIFIER_nondet_int() -> i32;
    safe fn __VERIFIER_assume(cond: i32);
}
fn main() {
    let x = __VERIFIER_nondet_int();
    __VERIFIER_assume((x == i32::MAX) as i32);
    let _ = x + 1;
}
