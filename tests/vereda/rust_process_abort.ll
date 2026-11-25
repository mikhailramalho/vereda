; Normal Rust termination is not a panic.
; EXPECT: ^VERIFICATION SUCCESSFUL$
; EXPECT-NOT: Rust panic
declare void @_ZN3std7process5abort17h1234567890abcdefE() noreturn
define i32 @main() {
  call void @_ZN3std7process5abort17h1234567890abcdefE()
  unreachable
}
