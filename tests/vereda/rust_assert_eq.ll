; assert_eq! can call assert_failed, even when that function has a body.
; EXPECT: Rust panic
; EXPECT: \[main\.assertion\.1\]
; EXPECT: ^VERIFICATION FAILED$
define void @_ZN4core9panicking13assert_failed17h1234567890abcdefE() {
  ret void
}
define i32 @main() {
  call void @_ZN4core9panicking13assert_failed17h1234567890abcdefE()
  ret i32 0
}
