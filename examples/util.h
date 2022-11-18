#ifndef _DUMPFILE_H
#define _DUMPFILE_H

#include <stdio.h>
#include <stdlib.h>

#define DF_NONE   0x00
#define DF_TITLE  0x01
#define DF_HEADER 0x02
#define DF_FOOTER 0x04

int dump_file(char *filename, int decor)
{
	FILE *fp;
	char *s;
	int i;
	int fsize;
		
	fp = fopen(filename, "r");
	
	if (fp == NULL)
	{
		printf("can't open '%s'\n", filename);
		return 1;
	}
	
	fseek(fp, 0L, SEEK_END);
	fsize = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	
	s = calloc(fsize + 1, sizeof(char));
	
	if (decor & DF_TITLE)
	{
		for (i = 0; i < 79; i++)
			fputc('-', stdout);
		
		printf("\nfile: %s\n", filename);
		
		for (i = 0; i < 79; i++)
			fputc('-', stdout);
		
		puts("");
	}
	
	if (decor & DF_HEADER)
	{
		for (i = 0; i < 79; i++)
			fputc('-', stdout);
		
		puts("");
	}
	
	fread(s, fsize, 1, fp);
	
	printf("%s\n", s);
	fclose(fp);	
	free(s);
	
	if (decor & DF_FOOTER)
	{
		for (i = 0; i < 79; i++)
			fputc('-', stdout);
		
		puts("");
	}	
	
	return 0;
}

void write_sample(char *filename)
{
	FILE *fp = fopen(filename, "w");
	
	fprintf(fp,
"; keys not within sections can be parsed by passing NULL to section parameter\n\
first=first not within section\n\
\n\
[section1]\n\
first=first in section1\n\
\n\
   [  white space  ]\n\
   one   = 1\n\
   two   = 2\n\
   three = 3");
   
	fclose(fp);
}

#endif /* _DUMPFILE_H */

