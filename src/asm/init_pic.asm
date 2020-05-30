;map 8259A PIC Microcontroller to handle keyboard interrupt 
_pic_map:
	mov	al, 0x11
	out	0x20, al
	out	0xa0, al

	mov	al, 0x20
	out	0x21, al
 	mov	al, 0x28
	out	0xa1, al
 
	mov	al, 0x4
	out	0x21, al
	mov	al, 0x2
	out	0xa1, al
 
	mov	al, 0x05 
	out	0x21, al
	mov	al, 0x01
	out	0xa1, al

	;mov	al, 11111101b
	;out	021h, al		

	;mov	al, 11111101b
	;out	0a1h, al

	mov	al, 0xff
	out	021h, al
	out	0a1h, al	

ret
