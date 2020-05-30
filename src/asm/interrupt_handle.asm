interrupt_handle:
    .21:
    push eax   
    in al,60h   
    mov byte [col],0
    mov byte [row],0
    mov bp, message
    mov di,0

    call _output.puts

    mov al,20h
    out 20h,al  ;; acknowledge the interrupt to the PIC
    pop eax     ;; restore state
    iret

    .20:
    push eax    ;; make sure you don't damage current state
    in al,60h   ;; read information from the keyboard
    mov byte [col],0
    mov byte [row],1
    mov bp, message
    mov di,0
    call _output.puts
    mov al,20h
    out 20h,al  ;; acknowledge the interrupt to the PIC
    pop eax     ;; restore state
    iret

    message db 'MaoOS',0
