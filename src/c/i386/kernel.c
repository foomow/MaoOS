#include "inc/asm.h"
#include "inc/output.h"
#include "inc/string.h"
#include "inc/disk_ata_lba28.h"
#include "inc/time.h"
#include "inc/key.h"
#include "inc/cmd.h"
#include "inc/process.h"
#include "inc/welcome.h"








void main()
{
	//init_output();
	//init_time();
	//init_key();
	current_path="/";
	welcome();
	enable_cur();
	show_cmd();

	while(true)
	{		
		cmd();
	}
}

#include "asm.c"
#include "output.c"
#include "string.c"
#include "disk_ata_lba28.c"
#include "time.c"
#include "key.c"
#include "cmd.c"
#include "welcome.c"
#include "process.c"