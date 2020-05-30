typedef struct _statusKey{
	byte cap:1;
	byte shift:1;
	byte ctrl:1;
	byte alt:1;
} STATUS_KEY;

static STATUS_KEY status_key;

void _KEYPRESS();

void _KEYPRESS();
void receivekey(uint8 scan_code);
char getkey();
void init_key();

byte cur_key_read=0;
byte cur_key_write=0;
char keybuff[256];
char key[256]={
		0,0,'1','2',
		'3','4','5','6',
		'7','8','9','0',
		'-','=',0,0,
		'q','w','e','r',
		't','y','u','i',
		'o','p','[',']',
		0,0,'a','s',
		'd','f','g','h',
		'j','k','l',';',
		0x27,'`',0,0x5c,
		'z','x','c','v',
		'b','n','m',',',
		'.','/',0,'*',
		0,' ',0,0
	};
