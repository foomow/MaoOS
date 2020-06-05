void _KEYPRESS()
{
	char keycode=0;
	asm("mov %0,al":"=a"(keycode));
	receivekey(keycode);
}

void receivekey(uint8 scan_code)
{	
	char ret=0;
	switch(scan_code)
		{
			case 0x1c:
				ret='R';
				break;
			case 0x0e:
				ret='B';
				break;
			default:
				ret=key[scan_code];
				break;
		}
	if(ret==0)return;
	if(cur_key_write==255&&cur_key_read==0)
	{
		keybuff[cur_key_write]=ret;
		cur_key_write=0;
		cur_key_read++;
		return;
	}
	if(cur_key_write==254&&cur_key_read==255)
	{
		keybuff[cur_key_write++]=ret;
		cur_key_read=0;
		return;
	}
	if(cur_key_write==255)
	{
		keybuff[cur_key_write]=ret;
		cur_key_write=0;
		return;
	}
	if(cur_key_write+1==cur_key_read)
	{
		keybuff[cur_key_write++]=ret;
		cur_key_read++;
		return;
	}
	keybuff[cur_key_write++]=ret;
}

char getkey()
{	
	char ret=0;	
	if(cur_key_read!=cur_key_write)
	{
		if(cur_key_read==255)
		{
			cur_key_read=0;
			ret= keybuff[255];
		}
		else
		{	
			ret= keybuff[cur_key_read++];			
		}
		
	}
	return ret;
}
