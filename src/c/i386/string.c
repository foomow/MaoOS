
//0--same,1--diff
byte cmps(char *s1,char *s2)
{
	while(*s1!=0)
	{
		if(*s1!=*s2)
		{
			return 1;
		}
		s1++;
		s2++;
	}
	if(*s2==0)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

byte set_str(char *str,char *content)
{
	while(*content!=0)
	{
		*str++=*content++;
	}
}

uint16 get_len(char *s)
{
	uint16 ret=0;
	while(s[ret]!=0)
	{
		ret++;
	}
	return ret;
}

void get_filename_dir(char* filename,char* name)
{
	uint8 hasdir=0;
	uint8 pos=0;
	uint8 idx=0;
	while(filename[idx]!=0)
	{
		if(filename[idx]=='/')
		{
			hasdir=1;
			pos=idx;
		}
		name[idx]=filename[idx];
		idx++;
	}
	name[pos]=0;
	if(hasdir==0)name[0]=0;
}

void get_filename_file(char* filename,char* name)
{
	uint8 pos=0;
	uint8 idx=0;
	while(filename[idx]!=0)
	{
		if(filename[idx]=='/')
		{
			pos=0;
			name[pos]=0;
			idx++;	
		}
		else
		{
			name[pos++]=filename[idx++];
		}
	}
	name[pos]=0;
}
