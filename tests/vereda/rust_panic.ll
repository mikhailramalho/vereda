; A v0 Rust panic call on one nondeterministic path is an assertion failure.
; EXPECT: Rust panic
; EXPECT: \[main\.assertion\.1\]
; EXPECT: ^VERIFICATION FAILED$
declare i32 @__VERIFIER_nondet_int()
declare void @_RNvNtCs8uuN1nrGffB_4core9panicking5panic() noreturn

define i32 @main() {
  %x = call i32 @__VERIFIER_nondet_int()
  %bad = icmp eq i32 %x, 3
  br i1 %bad, label %panic, label %done
panic:
  call void @_RNvNtCs8uuN1nrGffB_4core9panicking5panic()
  unreachable
done:
  ret i32 0
}
