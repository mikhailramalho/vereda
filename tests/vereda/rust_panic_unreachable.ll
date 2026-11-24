; A dead panic call does not make a safe path fail.
; EXPECT: ^VERIFICATION SUCCESSFUL$
declare void @_RNvNtCs8uuN1nrGffB_4core9panicking5panic() noreturn
define i32 @main() {
  br i1 false, label %panic, label %done
panic:
  call void @_RNvNtCs8uuN1nrGffB_4core9panicking5panic()
  unreachable
done:
  ret i32 0
}
