
uint8 parse_command_line()
{
	for(int i=0;i<256;i++)
	command_line[i]=0;
	uint8 ret=0;
	uint8 i=0;
	uint8 idx=0;
	while(cl_cache[i]!=0)
	{		
		if(cl_cache[i]==' '||(cl_cache[i]<0x20&&cl_cache[i]>0x7e))
		{
			if(idx!=0&&command_line[idx-1]!=0)
			{
				command_line[idx]=0;
				idx++;
			}
		}
		else
		{
			if(idx==0||command_line[idx-1]==0)
				ret++;
			command_line[idx++]=cl_cache[i];
		}
		i++;		
	}
	command_line[idx]=0;
	return ret;	
}
char* get_arg(uint8 idx)
{
	uint8 i=0;
	while(i<256)
	{
		if(idx==0)
		{
			return command_line+i;
		}
		if(command_line[i]==0)idx--;
		i++;
	}
	return 0;
}
void cmd()
{
	char key=getkey();	
	if(key!=0)
	{
		if(key=='R')
		{
			print_cr();
			if(cl_pos!=0)
			{
				uint8 arg_count=parse_command_line();
				if(arg_count>0)
				{	
					char* cli_cmd=get_arg(0);
					byte syntax_error=1;
					if(cmps(cli_cmd,"time")==0)
					{
						syntax_error=0;
						get_cmos_date();
						print_time();
					}				
					if(cmps(cli_cmd,"disk")==0)
					{
						syntax_error=0;
						show_disk_data(DRV_MASTER,0);
					}
					if(cmps(cli_cmd,"cls")==0)
					{
						syntax_error=0;
						cls();
					}
					if(cmps(cli_cmd,"ver")==0)
					{
						syntax_error=0;
						prints("MaoOS v1.01");
					}
					if(cmps(cli_cmd,"d")==0)
					{
						syntax_error=0;
						if(arg_count==1)
							list_dir(current_path);
						else
							list_dir(get_arg(1));
					}
					if(cmps(cli_cmd,"chakan")==0)
					{					
						if(arg_count>0)
						{
							syntax_error=0;
							byte buff[128];
							uint16 offset=0;			
							uint16 readcount=readfile(get_arg(1),offset,buff,128);
							while(readcount!=0)
							{
								for(int i=0;i<readcount;i++)
								{
									//if(buff[i]=='\r')
									printc(buff[i]);
								}
								offset+=readcount;
								readcount=readfile(get_arg(1),offset,buff,128);
							}
						}
					}
					if(cmps(cli_cmd,"yd")==0)//yidong
					{
						if(arg_count==3)
						{
							syntax_error=0;	
							movefile(get_arg(1),get_arg(2));
						}
					}
					if(cmps(cli_cmd,"s")==0)//yidong
					{
						if(arg_count==2)
						{
							syntax_error=0;
							char str[256];
							get_filename_dir(get_arg(1),str);
							prints(str);
							print_cr();
							uint32 dir_sec=get_dir_sector(str);
							print_dword_hex(dir_sec);
							print_cr();
							
							get_filename_file(get_arg(1),str);
							prints(str);
							print_cr();
							print_dword_hex(get_file_sector(dir_sec,str));
							print_cr();
							print_byte_hex(get_attr(get_file_sector(dir_sec,str)));
						}
					}
					if(syntax_error!=0)
					{
						font_color=FONT_COLOR_RED;
						prints("Syntax error!!");
						font_color=FONT_COLOR_WHITE;
					}
				}
				print_cr();
				for(int i=0;i<256;i++)
					cl_cache[i]=0;
				cl_pos=0;
								
			}
			show_cmd();
			return;
		}
		if(key=='B')
		{
			if(cl_pos>0)
			{
				byte pos=cl_pos-1;
				while(cl_cache[pos+1]!=0)
				{
					cl_cache[pos]=cl_cache[pos+1];
					pos++;
				}
				cl_cache[pos]=0;
				cl_pos--;
				cur_pos--;
			}
			cur_pos=cur_pos-cur_pos%80;
			show_cmd();
			prints(cl_cache);
			update_cur();
			return;
		}
		cmd_input(key);
	}
}

