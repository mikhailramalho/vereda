; An assertion panic is not an overflow violation.
; RUN: --property no-overflow
; EXPECT: ^VERIFICATION SUCCESSFUL$
; EXPECT: ^TRUE$
declare void @_RNvNtCs8uuN1nrGffB_4core9panicking5panic() noreturn
define i32 @main() {
  call void @_RNvNtCs8uuN1nrGffB_4core9panicking5panic()
  unreachable
}
