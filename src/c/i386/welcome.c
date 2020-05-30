
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
	
	print_cr();
	show_cmd();
	prints("MaoOS v1.01 - By Mao 2020\n");
	get_cmos_date();
	show_cmd();
	print_time();
	print_cr();

}
