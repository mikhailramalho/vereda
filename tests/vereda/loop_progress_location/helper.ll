; A library function with no debug information still has a useful name.
define i32 @_Z10model_loopv() {
entry:
  br label %loop
loop:
  %i = phi i32 [ 0, %entry ], [ %next, %loop ]
  %next = add i32 %i, 1
  %more = icmp ult i32 %next, 3
  br i1 %more, label %loop, label %done
done:
  ret i32 %next
}
