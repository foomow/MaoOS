#include "inc/kernel.h"

void main()
{		
	set_str(current_path,"/");
	welcome();	
	enable_cur();
	show_cmd();	
	while(true)
	{		
		cmd();
	}
}

#include "file.c"
#include "mem.c"
#include "asm.c"
#include "output.c"
#include "string.c"
#include "disk_ata_lba28.c"
#include "time.c"
#include "key.c"
#include "cmd.c"
#include "welcome.c"
#include "process.c"
#include "elf.c"