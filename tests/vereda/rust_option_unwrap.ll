; Real rustc 1.100.0 output (LLVM 23.1.5), compiled with:
; rustc --edition=2024 --crate-type=bin -C panic=abort -C opt-level=0 -g
; --emit=llvm-bc --remap-path-prefix=/tmp=. rust_option_unwrap.rs
; Source:
; #![no_main]
; #[unsafe(no_mangle)]
; pub extern "C" fn main() -> i32 {
;     let x: Option<i32> = None; x.unwrap()
; }
; FILES: rust_option_unwrap.bc
; EXPECT: ^VERIFICATION FAILED$
; EXPECT: Rust panic
; EXPECT: \[main\.assertion\.1\]
