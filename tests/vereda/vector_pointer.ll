; A vector of pointers crosses a call: the callee packs two pointers, the
; caller takes them out, compares them with the originals and loads
; through them.
; RUN:
; EXPECT: ^VERIFICATION SUCCESSFUL$
declare i32 @__VERIFIER_nondet_int()
declare void @__VERIFIER_assert(i32)

define <2 x ptr> @pair(ptr %a, ptr %b) noinline {
  %v0 = insertelement <2 x ptr> poison, ptr %a, i32 0
  %v1 = insertelement <2 x ptr> %v0, ptr %b, i32 1
  ret <2 x ptr> %v1
}

define i32 @main() {
  %p = alloca i32
  %q = alloca i32
  %x = call i32 @__VERIFIER_nondet_int()
  store i32 %x, ptr %p
  store i32 7, ptr %q
  %v = call <2 x ptr> @pair(ptr %p, ptr %q)
  %a = extractelement <2 x ptr> %v, i32 0
  %b = extractelement <2 x ptr> %v, i32 1
  %same = icmp eq ptr %a, %p
  %la = load i32, ptr %a
  %lb = load i32, ptr %b
  %ok1 = icmp eq i32 %la, %x
  %ok2 = icmp eq i32 %lb, 7
  %ok12 = and i1 %ok1, %ok2
  %ok = and i1 %ok12, %same
  %z = zext i1 %ok to i32
  call void @__VERIFIER_assert(i32 %z)
  ret i32 0
}
