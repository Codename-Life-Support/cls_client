public AsmDetour0

EXTERN player_content_check_ptr:QWORD
EXTERN ReportPlayersHaveContent:PROC

.code 

AsmDetour0 PROC
    ; store volatile registers
        push rax
        mov  rax, rdx
        push rcx
        push rdx
        push r8
        push r9
        push r10
        push r11
    ; call defined function
        sub rsp, 32
        mov rcx, rax
        call ReportPlayersHaveContent
        add rsp, 32
    ; restore volatile registers
        pop r11
        pop r10
        pop r9
        pop r8
        pop rdx
        pop rcx
        pop rax
    ; jump back into where we came from
        mov     rax, qword ptr [player_content_check_ptr] ; 
        jmp     rax
AsmDetour0 ENDP


END