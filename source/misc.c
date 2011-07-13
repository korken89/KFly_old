#include "../include/misc.h"

int cmpString(char* str1, char* str2)
{
	while (*str1)
	{
		if (*(str1++) != *(str2++))
			return 0;
	}
	
	return 1;
}

void memset(int* dst, int c, int size)
{
	while(size--)
		*(dst++) = c;
}

void memcpy(int* dst, int* src, int size)
{
	while(size--)
		*(dst++) = *(src++);
}

char* itoa(int val, int base)
{
	static char buf[31] = {0};
	if (val == 0)
	{
		buf[0] = '0';
		return buf;
	}
	
	int i;
	for(i = 30; (val && i); --i, val /= base)
		buf[i] = "0123456789abcdef"[val % base];
	
	return &buf[i+1];
}

int atoi(char* s)
{
	int num = 0;
	int flag = 0;

	for(int i = 0; i < strlen(s); i++)
	{
		if(s[i] >= '0' && s[i] <= '9')
			num = num * 10 + s[i] -'0';
		else if(s[0] == '-' && i == 0) 
			flag = 1;
		else 
			break;
	}
	
	if(flag == 1)
		num = -num;
		
	return num;
}

int strlen(char* str)
{
	int i = 0;
	while (*str != '\0' && *str != '\n' && *str != '\r')
	{
		str++;
		i++;
	}
		
	return i;
}
