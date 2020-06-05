/*
void show_reg(REG reg);
*/
byte cl_pos=0;
char cl_cache[256];
char command_line[256];
void cmd();
uint8 parse_command_line();
char* get_arg(uint8 i);