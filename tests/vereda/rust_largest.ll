; The test is rust_largest.bc, bitcode from rustc 1.100.0 (LLVM 23.1.5),
; which LLVM 21 could not read ("Invalid record", #213); this file only
; holds its header, an empty module linked with it. It was compiled with
; rustc --edition 2024 --crate-type=bin -C panic=abort -C opt-level=0 -g
; --emit=llvm-bc --remap-path-prefix=$PWD=. largest.rs, from this
; largest.rs, whose largest() returns 0 for three negative readings:
;
; #![no_main]
;
; unsafe extern "C" {
;     safe fn __VERIFIER_nondet_int() -> i32;
;     safe fn __VERIFIER_assert(cond: i32);
; }
;
; /// Returns the largest element of a non-empty slice.
; fn largest(values: &[i32]) -> i32 {
;     let mut best = 0; // bug: should start at values[0]
;     for i in 0..values.len() {
;         if values[i] > best {
;             best = values[i];
;         }
;     }
;     best
; }
;
; #[unsafe(no_mangle)]
; pub extern "C" fn main() -> i32 {
;     let readings = [
;         __VERIFIER_nondet_int(),
;         __VERIFIER_nondet_int(),
;         __VERIFIER_nondet_int(),
;     ];
;     let top = largest(&readings);
;     let found = readings[0] == top || readings[1] == top || readings[2] == top;
;     __VERIFIER_assert(found as i32);
;     0
; }
;
; FILES: rust_largest.bc
; EXPECT: ^VERIFICATION FAILED$
; EXPECT: FAILED +\[main\.assertion\.1\] +line 28 +__VERIFIER_assert
