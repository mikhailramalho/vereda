; The standard library's panic entry point is also a violation.
; EXPECT: Rust panic
; EXPECT: ^VERIFICATION FAILED$
declare void @_ZN3std9panicking11begin_panic17h1234567890abcdefE() noreturn
define i32 @main() {
  call void @_ZN3std9panicking11begin_panic17h1234567890abcdefE()
  unreachable
}
