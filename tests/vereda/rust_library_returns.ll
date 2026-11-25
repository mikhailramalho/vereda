; Returning declarations and defined non-panic library functions are not panics.
; EXPECT: ^VERIFICATION SUCCESSFUL$
; EXPECT-NOT: Rust panic
declare i32 @_ZN4core6option5value17h1234567890abcdefE()
define void @_ZN5alloc6helper4stop17h1234567890abcdefE() noreturn {
  call void @exit(i32 0)
  unreachable
}
declare void @exit(i32) noreturn
define i32 @main() {
  %x = call i32 @_ZN4core6option5value17h1234567890abcdefE()
  call void @_ZN5alloc6helper4stop17h1234567890abcdefE()
  unreachable
}
