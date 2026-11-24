; Rust's checked addition is a no-overflow property violation.
; RUN: --property no-overflow
; EXPECT: arithmetic overflow
; EXPECT: ^FALSE_OVERFLOW$
declare void @_ZN4core9panicking11panic_const24panic_const_add_overflow17h1234567890abcdefE() noreturn
define i32 @main() {
  call void @_ZN4core9panicking11panic_const24panic_const_add_overflow17h1234567890abcdefE()
  unreachable
}
