; Unwinding must not be mistaken for panic=abort.
; EXPECT: exception unwinding
; EXPECT: panic=abort
; EXPECT: ^VERIFICATION UNKNOWN:
declare void @may_panic()
declare i32 @__gxx_personality_v0(...)
define i32 @main() personality ptr @__gxx_personality_v0 {
  invoke void @may_panic() to label %done unwind label %cleanup
done:
  ret i32 0
cleanup:
  %lp = landingpad { ptr, i32 } cleanup
  resume { ptr, i32 } %lp
}
