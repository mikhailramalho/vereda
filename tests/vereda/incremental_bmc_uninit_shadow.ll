; %x is read from an uninitialised alloca before a loop that ends the prefix
; in an immediate checkpoint; after resuming, its store and the read in
; consume must still see it uninitialised, as --unwind 8 does.
; RUN: --ub-uninit-check --no-summarize-array-loops --incremental-bmc --max-k-step 8
; EXPECT: ^VERIFICATION FAILED$
target triple = "x86_64-unknown-linux-gnu"
define i32 @main() {
entry:
  %a = alloca i32, align 4
  %b = alloca i32, align 4
  %x = load i32, ptr %a, align 4
  br label %loop
loop:
  %i = phi i32 [ 0, %entry ], [ %next, %loop ]
  %next = add i32 %i, 1
  %more = icmp ult i32 %next, 6
  br i1 %more, label %loop, label %exit
exit:
  store i32 %x, ptr %b, align 4
  %y = call i32 @consume(ptr %b)
  ret i32 %y
}

define i32 @consume(ptr %p) noinline {
  %value = load i32, ptr %p, align 4
  ret i32 %value
}
