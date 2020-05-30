disk_predefine:
    %define IO_BASE 0x1f0
    %define IO_DATA 0x00
    %define IO_ERROR 0x01
    %define IO_FEATURE 0x01
    %define IO_SEC_C 0x02
    %define IO_LBA_L 0x03
    %define IO_SEC_N 0x03
    %define IO_LBA_M 0x04
    %define IO_CYL_L 0x04
    %define IO_LBA_H 0x05
    %define IO_CYL_H 0x05
    %define IO_DRIVEHEAD 0x06
    %define IO_STATUS 0x07
    %define IO_COMMAND 0x07

    %define IO_ALT_STATUS 0x3F6
    %define IO_DEV_CTL 0x3F6
    %define IO_DEV_ADDR 0x3F7

disk_load_kernel:
    xor eax,eax
    mov al,0xA0
    mov dx,IO_BASE+IO_DRIVEHEAD
    out dx,al
    mov al,0x00
    mov dx,IO_BASE+IO_SEC_C
    out dx,al
    mov dx,IO_BASE+IO_LBA_L
    out dx,al
    mov dx,IO_BASE+IO_LBA_M
    out dx,al
    mov dx,IO_BASE+IO_LBA_H
    out dx,al
    
    mov al,0xEC
    mov dx,IO_BASE+IO_COMMAND
    out dx,al
    
    mov dx,IO_BASE+IO_STATUS
    in al,dx

    mov ah,al
    and ah,0xff
    jz protected_code.boot_error
    
    call .poll

    mov ebx,19;19th is disk info
    call .read_sector

    mov ebx,[io_buff+0x2c];
    mov dword [int_timer_addr],ebx
    mov ebx,[io_buff+0x30];
    mov dword [int_key_addr],ebx


    mov ebx,20;
    add ebx,dword [io_buff+36] ;19:36 is fat length,so this line read first root dir sector
    call .read_sector

    mov ebx,dword [io_buff+43]; this is first file in root dir,it must be the kernel
    call .read_sector

    mov esi, kernel_name
    mov edi, io_buff
    mov ecx, 8
    cld
    repe  cmpsb
    jne  protected_code.boot_error

    
    call .find_kernel    
    ret

    .find_kernel:
    mov ecx,dword [io_buff+30];get lenth of kernel file    
    cmp ecx,473
    jg .more_sector

    cld
    mov esi, io_buff+35
    mov edi, 0x4000
    repe movsb
    ret

    .more_sector:
    push ecx
    mov ecx,473
    cld
    mov esi, io_buff+35
    mov edi, 0x4000
    repe movsb
    pop ecx
    sub ecx,473
    
    .next_sector:
    mov eax,508
    cmp ecx,508
    jg .do_read
    mov eax,ecx
    .do_read:
    push ecx
    mov ebx,dword [io_buff+508]
    call .read_sector    
    mov ecx,eax
    mov esi, io_buff
    repe movsb
    pop ecx
    sub ecx,eax
    jnz .next_sector

    ret
    
    .read_sector:
    push eax
    push ebx
    push ecx
    push edx
    push edi

    mov ecx,ebx
    shr ecx,24
    and cl,0x0f
    or cl,0xE0
    mov al,cl    
    mov dx,IO_BASE+IO_DRIVEHEAD
    out dx,al

    mov al,0x01
    mov dx,IO_BASE+IO_SEC_C
    out dx,al

    xor ecx,ecx
    mov ecx,ebx
    and cl,0xff
    mov al,cl
    mov dx,IO_BASE+IO_LBA_L
    out dx,al

    xor ecx,ecx
    mov ecx,ebx
    shr ecx,8
    and cl,0xff
    mov al,cl
    mov dx,IO_BASE+IO_LBA_M
    out dx,al

    xor ecx,ecx
    mov ecx,ebx
    shr ecx,16
    and cl,0xff
    mov al,cl
    mov dx,IO_BASE+IO_LBA_H
    out dx,al

    mov al,0x20
    mov dx,IO_BASE+IO_COMMAND
    out dx,al

    call .poll
    
    ; It is assumed that ES is already set and depends on the environment
    mov dx, IO_BASE+IO_DATA              ; Read words from port 0x1F0 and 0x1F1
    mov edi, io_buff           ; Address of BUFFER
    xor ecx,ecx
    mov ecx, 0x0100                ; Repeat 256 times
    rep insw

    pop edi
    pop edx
    pop ecx
    pop ebx
    pop eax
    ret

    .poll:
    mov dx,IO_BASE+IO_STATUS
    .loop:
    in al,dx
    test al,0x80
    jnz .loop  
    test al,0x08
    jz protected_code.boot_error
    ret

    kernel_name db 'kernel.o',0