; ModuleID = 'tests/control_1.c'
source_filename = "tests/control_1.c"

define i32 @main() {
entry:
  %x = alloca i32, align 4
  %i = alloca i32, align 4
  store i32 0, ptr %i, align 4
  store i32 0, ptr %x, align 4
  br label %cond

cond:                                             ; preds = %ifcont, %entry
  %i1 = load i32, ptr %i, align 4
  %compSLT = icmp slt i32 %i1, 10
  %ext = zext i1 %compSLT to i32
  %whilecond = icmp ne i32 %ext, 0
  br i1 %whilecond, label %whilebody, label %after

whilebody:                                        ; preds = %cond
  %i2 = load i32, ptr %i, align 4
  %compEE = icmp eq i32 %i2, 5
  %ext3 = zext i1 %compEE to i32
  %ifcond = icmp ne i32 %ext3, 0
  br i1 %ifcond, label %then, label %ifcont

then:                                             ; preds = %whilebody
  store i32 100, ptr %x, align 4
  br label %ifcont

ifcont:                                           ; preds = %then, %whilebody
  %i4 = load i32, ptr %i, align 4
  %add = add nsw i32 %i4, 1
  store i32 %add, ptr %i, align 4
  br label %cond

after:                                            ; preds = %cond
  %x5 = load i32, ptr %x, align 4
  ret i32 %x5
}
