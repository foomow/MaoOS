#include "asm.c"
#include "boot_text.c"
#include "time.c"
#include "boot_disk_ata_lba28.c"
#include "process.c"

void main()
{
	asm("mov ax,0x0010");
	asm("mov ds,ax");
	asm("mov es,ax");
	asm("mov fs,ax");
	asm("mov gs,ax");
	asm("mov ss,ax");
	asm("mov esp,0x1FFFF");
	cls();
	
	char str1[20]="Drive M sector:";
	char str2[20]="Drive S sector:";
	char str3[20]="Memory:";
	char str4[43]="Base             Length           Type";

	get_cmos_date();
	print_time();
	print_cr();
	
	uint32 diskcap=get_total_sector(DRV_MASTER);
	prints(str1);
	print_u32(diskcap);
	print_cr();

	diskcap=get_total_sector(DRV_SLAVE);
	prints(str2);
	print_u32(diskcap);
	print_cr();

	uint8 *mem_info_addr=(uint8 *)0x8000;
	uint16 memory_count=(uint16)(*mem_info_addr);
	prints(str3);
	print_cr();
	prints(str4);
	print_cr();
	
	uint64 *base;
	uint64 *length;
	uint32 *type;

	for(int i=0;i<memory_count;i++)
	{
		base=(uint64*)(mem_info_addr+4+i*24);
		print_hex_64(*base);
		printc(':');
		length=(uint64*)(mem_info_addr+4+i*24+8);
		print_hex_64(*length);
		printc(':');
		type=(uint32*)(mem_info_addr+4+i*24+16);
		print_hex_32(*type);
		print_cr();
	}
	
	char kfile[7]="kernel";
	launch(kfile);

	asm("hlt");
}
