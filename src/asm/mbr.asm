[BITS 16]
[ORG 0x7c00]

mov eax, 0x0
cpuid

mov dword [vendor_id],ebx
mov dword [vendor_id+4],edx
mov dword [vendor_id+8],ecx


mov bh,0
mov bl,0x0f
mov cx,12
mov dh,0
mov dl,0
mov bp,vendor_id
mov ah,0x13
mov al,00000001b
int 0x10

mov esi, vendor_id
mov edi, allow_id
mov ecx, 12
cld
repe  cmpsb
jecxz  go_init

mov bh,0
mov bl,0x0e
mov cx,40
mov dh,1
mov dl,0
mov bp,err_msg
mov ah,0x13
mov al,00000001b
int 0x10

jmp $

go_init:

mov ah, 0x00
mov al, 0x03  ; text mode 80x25 16 colours
int 0x10

mov ah,0x02
mov al,18;add up to 19 sectors for system.the 1st sector is this code(mbr);and the 20th sector is diskinfo
mov ch,0
mov cl,2
mov dh,0
mov dl,0x80
mov bx,0x050
mov es,bx
mov bx,0
int 0x13

jmp 0x50:0

vendor_id times 12 db 0
allow_id db 'GenuineIntel'
err_msg db 'MaoOS only run on GenuineIntel processor'
times 510-($-$$) db 0
dw 0xaa55
