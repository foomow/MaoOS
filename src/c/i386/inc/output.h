#define V_BASE 0xB8000
char hex_char[16]="0123456789ABCDEF";
byte cl_pos=0;
char cl_cache[256];
void init_output();
void enable_cur();
void disable_cur();

void update_cur();

void cls();
void scroll();
void prints(const char* str);
void printc(char c);
void show_cmd();
void cmd_input(char c);
void print_cr();
void print_byte_hex(byte v);
void print_word_hex(uint16 v);
void print_dword_hex(uint32 v);

void print_u32(uint32 val);
