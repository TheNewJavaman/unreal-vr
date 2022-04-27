; https://defuse.ca/online-x86-assembler.htm (use x64)

; if (ViewInfo.bConstrainAspectRatio)
test byte ptr [rcx + 0x30], 0x1

; Jump after if statement
je 0x00004242 ; Address offset in pattern 0x0000....

; End of previous function (probably CalculateProjectionMatrix), start of CalculateProjectionMatrixGivenView
ret
int3
mov rax, rsp