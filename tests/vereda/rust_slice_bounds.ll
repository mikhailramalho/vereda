; Real rustc 1.100.0 output (LLVM 23.1.5), compiled with:
; rustc --edition=2024 --crate-type=bin -C panic=abort -C opt-level=0 -g
; --emit=llvm-bc --remap-path-prefix=/tmp=. rust_slice_bounds.rs
; Source:
; #![no_main]
; #[unsafe(no_mangle)]
; pub extern "C" fn main() -> i32 {
;     let a = [1, 2]; let n = 3; a[..n].len() as i32
; }
; FILES: rust_slice_bounds.bc
; EXPECT: ^VERIFICATION FAILED$
; EXPECT: array bounds violated
; EXPECT: \[[^\]]+\.array-bounds-violated\.1\]
