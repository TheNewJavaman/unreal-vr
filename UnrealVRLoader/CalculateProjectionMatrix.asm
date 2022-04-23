; https://defuse.ca/online-x86-assembler.htm (use x64)

; PATTERNS
; --------

; if (ViewInfo.bConstrainAspectRatio)
test byte ptr [rcx + 0x30], 0x1

; Jump after if statement
je 0x00004242 ; Address offset in pattern 0x0000....

; End of previous function, start of CalculateProjectionMatrixGivenView
ret
int3
mov rax, rsp

; PATCH
; -----

; Save registers
push rbx
push rsp
push rbp

; Redirect to detour
mov rcx, r9 ; Set first argument of detour as pointer to projection data
add rcx, 0x5C ; Adjust to point to projection matrix
mov r11, 0x0000024242424242 ; Load address of detour, TBD at runtime
call r11 ; Call detour

; Restore registers and return
pop rbp
pop rsp
pop rbx
ret