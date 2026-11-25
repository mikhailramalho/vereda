; llvm.ptrmask masks the integer address of a pointer while keeping its
; provenance: aligned objects fold the mask onto the offset, unaligned ones
; compute ((base + off) & mask) - base, and null stays null.
; EXPECT: ^VERIFICATION SUCCESSFUL$
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

declare ptr @llvm.ptrmask.p0.i64(ptr, i64)
declare void @__VERIFIER_assert(i32)
declare i32 @__VERIFIER_nondet_int()
declare i64 @__VERIFIER_nondet_ulong()

define i32 @main() {
entry:
  ; 1. Aligned object: GEP +5 then mask -16 lands at offset 0.
  %a = alloca [16 x i8], align 16
  store i32 42, ptr %a, align 16
  %a5 = getelementptr i8, ptr %a, i64 5
  %am = call ptr @llvm.ptrmask.p0.i64(ptr %a5, i64 -16)
  %v1 = load i32, ptr %am, align 4
  %eq1 = icmp eq i32 %v1, 42
  %z1 = zext i1 %eq1 to i32
  call void @__VERIFIER_assert(i32 %z1)

  ; 2. All-ones mask is identity.
  %am_id = call ptr @llvm.ptrmask.p0.i64(ptr %a, i64 -1)
  %v2 = load i32, ptr %am_id, align 4
  %eq2 = icmp eq i32 %v2, 42
  %z2 = zext i1 %eq2 to i32
  call void @__VERIFIER_assert(i32 %z2)

  ; 3. Masking null with any symbolic mask stays null.
  %sym = call i64 @__VERIFIER_nondet_ulong()
  %nm = call ptr @llvm.ptrmask.p0.i64(ptr null, i64 %sym)
  %eq3 = icmp eq ptr %nm, null
  %z3 = zext i1 %eq3 to i32
  call void @__VERIFIER_assert(i32 %z3)

  ; 4. Conditional pointer (split) keeps each branch's provenance.
  %b = alloca [16 x i8], align 16
  store i32 99, ptr %b, align 16
  %c = call i32 @__VERIFIER_nondet_int()
  %cond = icmp ne i32 %c, 0
  %a3 = getelementptr i8, ptr %a, i64 3
  %b7 = getelementptr i8, ptr %b, i64 7
  %sel = select i1 %cond, ptr %a3, ptr %b7
  %sm = call ptr @llvm.ptrmask.p0.i64(ptr %sel, i64 -16)
  %v4 = load i32, ptr %sm, align 4
  %want4 = select i1 %cond, i32 42, i32 99
  %eq4 = icmp eq i32 %v4, %want4
  %z4 = zext i1 %eq4 to i32
  call void @__VERIFIER_assert(i32 %z4)

  ; 5. Unaligned object with mask wider than alignment: address matches addr & mask.
  %u = alloca [32 x i8], align 1
  %u15 = getelementptr i8, ptr %u, i64 15
  %um = call ptr @llvm.ptrmask.p0.i64(ptr %u15, i64 -16)
  %addr_in = ptrtoint ptr %u15 to i64
  %addr_out = ptrtoint ptr %um to i64
  %expected = and i64 %addr_in, -16
  %eq5 = icmp eq i64 %addr_out, %expected
  %z5 = zext i1 %eq5 to i32
  call void @__VERIFIER_assert(i32 %z5)

  ; 6. Symbolic mask on aligned object: address matches addr & sym.
  %as = call ptr @llvm.ptrmask.p0.i64(ptr %a, i64 %sym)
  %a_in = ptrtoint ptr %a to i64
  %a_out = ptrtoint ptr %as to i64
  %a_exp = and i64 %a_in, %sym
  %eq6 = icmp eq i64 %a_out, %a_exp
  %z6 = zext i1 %eq6 to i32
  call void @__VERIFIER_assert(i32 %z6)

  ret i32 0
}
