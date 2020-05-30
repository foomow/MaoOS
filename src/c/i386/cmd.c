

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

				byte syntax_error=1;
				if(cmps(cl_cache,"time")==0)
				{
					syntax_error=0;
					get_cmos_date();
					print_time();
				}				
				if(cmps(cl_cache,"disk")==0)
				{
					syntax_error=0;
					show_disk_data(DRV_MASTER,0);
				}
				if(cmps(cl_cache,"cls")==0)
				{
					syntax_error=0;
					cls();
				}
				if(cmps(cl_cache,"ver")==0)
				{
					syntax_error=0;
					prints("MaoOS v1.01");
				}
				if(cmps(cl_cache,"d")==0)
				{
					syntax_error=0;
					list_dir(current_path);
				}
				if(cmps(cl_cache,"chakan")==0)
				{
					syntax_error=0;
					byte buff[128];
					uint16 readcount=readfile("build",0,buff,128);
					for(int i=0;i<readcount;i++)
					{
						printc(buff[i]);
					}
				}


				if(syntax_error!=0)
				{
					prints("Syntax error!!");
				}
				for(int i=0;i<256;i++)
					cl_cache[i]=0;
				cl_pos=0;
				print_cr();
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

