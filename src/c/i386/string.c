
//0--same,1--diff
byte cmps(char *s1,char *s2)
{
	while(*s1!=0)
	{
		if(*s1!=*s2)return 1;
		s1++;
		s2++;
	}
	if(*s2==0)
		return 0;
	else
		return 1;
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
