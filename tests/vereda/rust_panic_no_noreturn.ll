; A bodyless Rust library call followed by unreachable is a panic without noreturn.
; EXPECT: ^VERIFICATION FAILED$
; EXPECT: Rust panic
declare void @_ZN4core6option13expect_failed17h1234567890abcdefE()
define i32 @main() {
  call void @_ZN4core6option13expect_failed17h1234567890abcdefE()
  unreachable
}
