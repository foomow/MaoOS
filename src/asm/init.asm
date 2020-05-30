[ORG 0x0500]
real_code:
    [BITS 16]    
    .main:
    call .check_mem

    cli
    xor ax, ax
    mov ds, ax
    lgdt [.gdt_info]
    xor ax, ax
    mov ds, ax
    lidt [.idt_info]

    mov eax, cr0
    or eax, 1
    mov cr0, eax



    jmp 0x08:protected_code

    memsize equ 0x8000

    .gdt_info: 
        dw .gdt_end-.gtd_start-1
        dd .gtd_start

    .gtd_start:
        .gdt_null:     dq 0x0000000000000000
        .gdt_cs:       dq 0x00cf9a000000ffff
        .gdt_ds:       dq 0x00cf92000000ffff
        .gdt_k_cs:     dq 0x00c09a10000000ff
        .gdt_k_ds:     dq 0x00c09210000000ff
        .gdt_tss:      dq 0x00c089090000000f
    .gdt_end:

    .idt_info:
        dw .idt_end-.itd_start-1
        dd .itd_start
    .itd_start:
        %rep 32
		dq 0x0000000000000000
        %endrep
        .20 dw _interrupt_handlers.20
            dw 0x0008
            db 0
            db 0x8e
            dw 0
        .21 dw _interrupt_handlers.21
            dw 0x0008
            db 0
            db 0x8e
            dw 0
        %rep 13
		dq 0x0000000000000000
        %endrep
        .2f dw _interrupt_handlers.2f
            dw 0x0008
            db 0
            db 0x8e
            dw 0
        %rep 208
		dq 0x0000000000000000
        %endrep
    .idt_end:


    .check_mem:
    mov di, memsize+4          ; Set di to 0x8004. Otherwise this code will get stuck in `int 0x15` after some entries are fetched 
	xor ebx, ebx		; ebx must be 0 to start
	xor bp, bp		; keep an entry count in bp
	mov edx, 0x0534D4150	; Place "SMAP" into edx
	mov eax, 0xe820
	mov [es:di + 20], dword 1	; force a valid ACPI 3.X entry
	mov ecx, 24		; ask for 24 bytes
	int 0x15
	jc short .error	; carry set on first call means "unsupported function"
	mov edx, 0x0534D4150	; Some BIOSes apparently trash this register?
	cmp eax, edx		; on success, eax must have been reset to "SMAP"
	jne short .error
	test ebx, ebx		; ebx = 0 implies list is only 1 entry long (worthless)
	je short .error
	jmp short .check_ent

    .read_next:
	mov eax, 0xe820		; eax, ecx get trashed on every int 0x15 call
	mov [es:di + 20], dword 1	; force a valid ACPI 3.X entry
	mov ecx, 24		; ask for 24 bytes again
	int 0x15
	jc short .count_got		; carry set means "end of list already reached"
	mov edx, 0x0534D4150	; repair potentially trashed register

    .check_ent:
	jcxz .skipempty		; skip any 0 length entries
	cmp cl, 20		; got a 24 byte ACPI 3.X response?
	jbe short .empty_got
	test byte [es:di + 20], 1	; if so: is the "ignore this data" bit clear?
	je short .skipempty

    .empty_got:
	mov ecx, [es:di + 8]	; get lower uint32_t of memory region length
	or ecx, [es:di + 12]	; "or" it with upper uint32_t to test for zero
	jz .skipempty		; if length uint64_t is 0, skip entry
	inc bp			; got a good entry: ++count, move to next storage spot
	add di, 24

    .skipempty:
	test ebx, ebx		; if ebx resets to 0, list is complete
	jne short .read_next

    .count_got:
	mov [memsize], bp	; store the entry count
	clc			; there is "jc" on end of list to this point, so the carry must be cleared
	ret

    .error:
	stc
	ret


protected_code:
    [BITS 32]
    .main:
    mov ax,0x0010
    mov ds,ax
    mov es,ax
    mov fs,ax
    mov gs,ax
    mov ss,ax
    mov esp,0x1FFFF

    mov byte [col],0
    mov byte [row],0
    mov bp, init_msg
    mov di,0
    call _output.puts

    call _output.disable_cursor

    mov eax,0x01
    cpuid

    test edx,1<<9
    jnz .init_apic

    mov byte [col],0
    mov byte [row],0
    mov byte [cur_attr],0x0e
    mov bp, err_msg
    mov di,0

    call _output.puts
    mov byte [cur_attr],0x0f

.init_apic:
    call _apic_map
    
.load_kernel:
    call disk_load_kernel

.init_done:
    call _output.cls
    mov byte [col],0
    mov byte [row],0
    mov bp, done
    mov di,0
    call _output.puts



	sti
    jmp 0x4000

.boot_error:
    call _output.cls
    mov byte [col],0
    mov byte [row],0
    mov bp, disk_err_msg
    mov di,0
    call _output.puts
    jmp $

%include "init_apic.asm"
%include "init_pic.asm"
%include "init_interrupt.asm"
%include "init_output.asm"
%include "disk.asm"

init_msg db 'MaoOS is initiallizing',0
err_msg db 'MaoOS need APCI supported',0
disk_err_msg db 'FileSystem fault',0
done db 'Launch kernel...',0

;start form 0x500 add up 18 sectors (0x2400Bytes)
;so int address start from 0x2900
;each int entry occupy 8 bytes
;256 ints need 0x800 bytes
;so int address table should end at 0x2900+0x800=0x3700
.addr_table:
    int_timer_addr     equ 0x00002900
    int_key_addr       equ 0x00002908

    io_buff equ 0x00003700;0x200byte for a sector,so buff will end up to 0x3900
    kb_buff equ 0x00003900;kb buff start at there,and last byte(0x39ff)is the key_cursor
    ;0x3a00-0x3fff is reserved for feather more function
    ;and then kernel code begin at 0x4000
