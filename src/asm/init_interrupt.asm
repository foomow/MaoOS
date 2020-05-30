_interrupt_handlers:

.20:
    call [0x2908]
    mov dword [apic+APIC_EOI],0
    iretd

.21:
    push ax
    in al,0x60
    call [0x2900]
    pop ax
    mov dword [apic+APIC_EOI],0
    iretd

.2f:  
    
    mov dword [apic+APIC_EOI],0
    iretd

