
void welcome()
{
	cls();
	prints("  __       __                       ______    ______  \n");	
	prints(" /  \\     /  |                     /      \\  /      \\ \n");	
	prints(" MM  \\   /MM |  ______    ______  /MMMMMM  |/MMMMMM  |\n");
	prints(" MMM  \\ /MMM | /      \\  /      \\ MM |  MM |MM \\__MM/ \n");
	prints(" MMMM  /MMMM | MMMMMM  |/MMMMMM  |MM |  MM |MM      \\ \n");
	prints(" MM MM MM/MM | /    MM |MM |  MM |MM |  MM | MMMMMM  |\n");
	prints(" MM |MMM/ MM |/MMMMMMM |MM \\__MM |MM \\__MM |/  \\__MM |\n");
	prints(" MM | M/  MM |MM    MM |MM    MM/ MM    MM/ MM    MM/ \n");
	prints(" MM/      MM/  MMMMMMM/  MMMMMM/   MMMMMM/   MMMMMM/  \n");
	

	print_cr();
	prints("MaoOS v1.01 - By Mao 2020\n");
	get_cmos_date();
	print_time();
	print_cr();	

	print_cr();
	uint32 disk_space=get_total_sector(DRV_MASTER);
	prints("DISK SPACE:");
	print_u32(disk_space/2);
	prints(" k\n");
	init_mem();
	print_cr();
		

}
