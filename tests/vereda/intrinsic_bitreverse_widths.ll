; llvm.bitreverse and llvm.ctpop on non-standard integer widths (i1, i3, i5, i128).
; EXPECT: ^VERIFICATION SUCCESSFUL$
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

declare i1 @llvm.bitreverse.i1(i1)
declare i3 @llvm.bitreverse.i3(i3)
declare i5 @llvm.bitreverse.i5(i5)
declare i128 @llvm.bitreverse.i128(i128)
declare i1 @llvm.ctpop.i1(i1)
declare i3 @llvm.ctpop.i3(i3)
declare i5 @llvm.ctpop.i5(i5)
declare i128 @llvm.ctpop.i128(i128)
declare void @__VERIFIER_assert(i32)
declare i32 @__VERIFIER_nondet_uint()
declare i64 @__VERIFIER_nondet_ulong()

define i32 @main() {
entry:
  %u = call i32 @__VERIFIER_nondet_uint()

  ; i1: identity for both bitreverse and ctpop.
  %b = trunc i32 %u to i1
  %rb = call i1 @llvm.bitreverse.i1(i1 %b)
  %cb = call i1 @llvm.ctpop.i1(i1 %b)
  %eq_rb = icmp eq i1 %rb, %b
  %z_rb = zext i1 %eq_rb to i32
  call void @__VERIFIER_assert(i32 %z_rb)
  %eq_cb = icmp eq i1 %cb, %b
  %z_cb = zext i1 %eq_cb to i32
  call void @__VERIFIER_assert(i32 %z_cb)

  ; i3: odd width (exercises leftover term in ctpop adder tree).
  %s3 = trunc i32 %u to i3
  %r3 = call i3 @llvm.bitreverse.i3(i3 %s3)
  %rr3 = call i3 @llvm.bitreverse.i3(i3 %r3)
  %eq_r3 = icmp eq i3 %rr3, %s3
  %z_r3 = zext i1 %eq_r3 to i32
  call void @__VERIFIER_assert(i32 %z_r3)
  %c3 = call i3 @llvm.ctpop.i3(i3 %s3)
  %ns3 = xor i3 %s3, -1
  %nc3 = call i3 @llvm.ctpop.i3(i3 %ns3)
  %sum3 = add i3 %c3, %nc3
  %eq_c3 = icmp eq i3 %sum3, 3
  %z_c3 = zext i1 %eq_c3 to i32
  call void @__VERIFIER_assert(i32 %z_c3)

  ; i5: odd width > 3.
  %s5 = trunc i32 %u to i5
  %r5 = call i5 @llvm.bitreverse.i5(i5 %s5)
  %rr5 = call i5 @llvm.bitreverse.i5(i5 %r5)
  %eq_r5 = icmp eq i5 %rr5, %s5
  %z_r5 = zext i1 %eq_r5 to i32
  call void @__VERIFIER_assert(i32 %z_r5)
  %c5 = call i5 @llvm.ctpop.i5(i5 %s5)
  %ns5 = xor i5 %s5, -1
  %nc5 = call i5 @llvm.ctpop.i5(i5 %ns5)
  %sum5 = add i5 %c5, %nc5
  %eq_c5 = icmp eq i5 %sum5, 5
  %z_c5 = zext i1 %eq_c5 to i32
  call void @__VERIFIER_assert(i32 %z_c5)

  ; i128: wide integer.
  %lo = call i64 @__VERIFIER_nondet_ulong()
  %hi = call i64 @__VERIFIER_nondet_ulong()
  %lo128 = zext i64 %lo to i128
  %hi128 = zext i64 %hi to i128
  %hi_sh = shl i128 %hi128, 64
  %s128 = or i128 %hi_sh, %lo128
  %r128 = call i128 @llvm.bitreverse.i128(i128 %s128)
  %rr128 = call i128 @llvm.bitreverse.i128(i128 %r128)
  %eq_r128 = icmp eq i128 %rr128, %s128
  %z_r128 = zext i1 %eq_r128 to i32
  call void @__VERIFIER_assert(i32 %z_r128)
  %c128 = call i128 @llvm.ctpop.i128(i128 %s128)
  %ns128 = xor i128 %s128, -1
  %nc128 = call i128 @llvm.ctpop.i128(i128 %ns128)
  %sum128 = add i128 %c128, %nc128
  %eq_c128 = icmp eq i128 %sum128, 128
  %z_c128 = zext i1 %eq_c128 to i32
  call void @__VERIFIER_assert(i32 %z_c128)

  ; Constant folding on i128 via path condition.
  %is_one = icmp eq i128 %s128, 1
  br i1 %is_one, label %if_one, label %done

if_one:
  %r_one = call i128 @llvm.bitreverse.i128(i128 %s128)
  %top_bit = shl i128 1, 127
  %eq_top = icmp eq i128 %r_one, %top_bit
  %z_top = zext i1 %eq_top to i32
  call void @__VERIFIER_assert(i32 %z_top)
  %c_one = call i128 @llvm.ctpop.i128(i128 %s128)
  %eq_cone = icmp eq i128 %c_one, 1
  %z_cone = zext i1 %eq_cone to i32
  call void @__VERIFIER_assert(i32 %z_cone)
  br label %done

done:
  ret i32 0
}
