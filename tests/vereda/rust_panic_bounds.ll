; Rust's indexing panic is an assertion with a bounds diagnostic.
; EXPECT: array bounds violated
; EXPECT: ^VERIFICATION FAILED$
declare void @_ZN4core9panicking18panic_bounds_check17h1234567890abcdefE() noreturn
define i32 @main() {
  call void @_ZN4core9panicking18panic_bounds_check17h1234567890abcdefE()
  unreachable
}
