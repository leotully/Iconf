/*------------------------------------------------------------------------------
MIT License

Copyright (c) 2022 Leo Tully

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <iconf.h>

/* options for trimws */
typedef enum { ALL, LEFT, RIGHT } trim_type;

static char *trimws(char *s, trim_type tt)
{
	char *end;
	
	if (*s == '\0')
		return s;

	if (tt == ALL || tt == LEFT)
	{
		while (isspace(*s))
			s++;
	}
	
	if (tt == LEFT)
		return s;

	end = s + strlen(s) - 1;
	
	while (end > s && isspace(*end))
		end--;
	
	end[1] = '\0';

	return s;
}

static int strnicmp_p(const char *s1, const char *s2, size_t n)
{	
	for (;;)
	{
		int x = tolower((unsigned char)*s1) - tolower((unsigned char)*s2);
		
		n--;
		
		if(x != 0 || !*s1 || n == 0)
			return x;

		s1++;
		s2++;
	}
}

static int stricmp_p(const char *s1, const char *s2)
{
	int x;

	for (;;)
	{
		x = tolower((unsigned char)*s1) - tolower((unsigned char)*s2);
		
		if (x != 0 || !*s1)
			return x;

		s1++;
		s2++;
	}
}

static int strpos(const char *s1, const char *s2, int index)
{
	const char *p = strstr(s1+index, s2);
    
    if (p == NULL)
		return -1; 
    
	return p - s1;
}

void iconf_free(struct Iconf ini)
{
	int i;

	for (i = 0; i < ini.count; i++)
		free(ini.strings[i]);
	
	free(ini.strings);
}

/* errval: can't open file = 1, allocation problem 2 */
struct Iconf iconf_load(const char *filename)
{
	struct Iconf ini;
	FILE *fp;
	int arsize = 100;
	char *s;
	char buffer[BUF_LEN];
	int count = 0;
	
	ini.errval = 0;
	ini.count = 0;
	
	fp = fopen(filename, "r");
	
	if (fp == NULL)
	{
		ini.errval = 1;
		return ini;
	}	
	
	ini.strings = (char**)malloc(arsize * sizeof(char*));

	if (ini.strings == NULL)
	{
		fclose(fp);
		ini.errval = 2;
		return ini;
	}

	while (fgets(buffer, BUF_LEN, fp) != NULL)
	{
		if (count == arsize)
		{
			char **newptr;

			arsize += 100;

			newptr = (char**)realloc(ini.strings, arsize * sizeof(char*));
			
			if (newptr == NULL)
			{
				fclose(fp);
				iconf_free(ini);
				ini.errval = 2;
				return ini;		
			}

			ini.strings = newptr;
		}

		if (strchr(buffer, '\n') == NULL)
		{
			char discard;
			
			while ((discard = fgetc(fp)) != '\n' && discard != EOF);
		}

		buffer[strcspn(buffer, "\n")] = '\0';

		s = (char*)malloc(strlen(buffer) + 1);
		
		if (s == NULL)
		{
			fclose(fp);
			iconf_free(ini);
			ini.errval = 2;
			return ini;		
		}
		
		strcpy(s, buffer);

		ini.strings[count] = s;

		count++;
	}

	fclose(fp);

	ini.count = count;
	
	return ini;
}

static int is_section(const char *section, const char *s2)
{
	char *temp;
	int start;
	int end;
	int x = 0;

	/* calloc used to insure null termination with strncpy */
	temp = (char*)calloc(strlen(s2), 1);

	start = strpos(s2, "[", 0);
	end = strpos(s2, "]", 0);

	if (start > -1 && end > -1)
	{
		strncpy(temp, s2 + start + 1, end - start - 1);
		
		if (stricmp_p(section, trimws(temp, ALL)) == 0)
			x = 1;
	}

	free(temp);

	return x;
}

char *iconf_get_key(struct Iconf iconf, char *section, char *keyname)
{
	static char keyValue[BUF_LEN];
	char *s;
	int pos = -1;
	int i = 0;
	int in_section = 0;
	int keyonly = 0;

	if (section == NULL)
		keyonly = 1;
	 
	if ((s = (char*)malloc(1)) == NULL)
	{
		iconf.errval = 2;
		return NULL;
	}
	
	for (i = 0; i < iconf.count; i++)
	{
		char *newptr;

		newptr = (char*)realloc(s, strlen(iconf.strings[i]) + 1);

		if (newptr == NULL)
		{
			free(s);
			return NULL;
		}

		s = newptr;
		
		strcpy(s, trimws(iconf.strings[i], LEFT));

		if (s[0] == ';') continue;
		if (s[0] == '#') continue;

		pos = strpos(s, "=", 0);
	
		if (!keyonly)
		{
			if (in_section && s[0] == '[')
				in_section = 0;
	
			if (is_section(section, s))
				in_section = 1;
		}
		
		if (pos > -1 && strnicmp_p(s, keyname, strlen(keyname)) == 0)
		{
			if (keyonly || in_section)
			{
				strcpy(keyValue, s + pos + 1);
			
				free(s);
				return trimws(keyValue, ALL);
			}
		}
	}

	free(s);
	return NULL; /* key not found */
}

/* return value: ok - 0, key not found = 1, allocation problem 2 */
int iconf_set_key(struct Iconf iconf, char *section, char *keyname, char *newvalue)
{
	char keyBuilder[BUF_LEN] = {0};
	char *keyValue;
	char *s;
	int pos = -1;
	int i = 0;
	int in_section = 0;
	int keyonly = 0;

	if (section == NULL)
		keyonly = 1;
	
	if ((s = (char*)malloc(1)) == NULL) return 2;
	 
	for (i = 0; i < iconf.count; i++)
	{
		char *newptr;

		newptr = (char*)realloc(s, strlen(iconf.strings[i]) + 1);

		if (newptr == NULL)
		{
			free(s);
			return 2;
		}

		s = newptr;

		strcpy(s, trimws(iconf.strings[i], LEFT));

		if (s[0] == ';') continue;
		if (s[0] == '#') continue;

		pos = strpos(s, "=", 0);
	
		if (!keyonly)
		{
			if (in_section && s[0] == '[')
				in_section = 0;
	
			if (is_section(section, s))
				in_section = 1;
		}
		
		if (pos > -1 && strnicmp_p(s, keyname, strlen(keyname)) == 0)
		{
			if (keyonly || in_section)
			{
				int orig_pos = strpos(iconf.strings[i], "=", 0);

				strncpy(keyBuilder, iconf.strings[i], orig_pos + 1);

				orig_pos = strlen(keyBuilder);

				while (isspace(iconf.strings[i][orig_pos]))
				{
					keyBuilder[orig_pos] = iconf.strings[i][orig_pos];
					orig_pos++;
				}

				strcat(keyBuilder, newvalue);


				keyValue = (char*)realloc(iconf.strings[i], strlen(keyBuilder) + 1);
				strcpy(keyValue, keyBuilder);
				iconf.strings[i] = keyValue;

				free(s);
				return 0;
			}
		}
	}

	free(s);
	return 1; /* key not found */
}

int iconf_write(struct Iconf iconf, char *filename)
{
	FILE *fp;
	int i;

	fp = fopen(filename, "w");

	if (fp == NULL)
		return 1;
	
	for (i = 0; i < iconf.count; i++)
	{
		fprintf(fp, "%s", iconf.strings[i]);
		
		if (i < iconf.count - 1)
			fprintf(fp, "\n");
	}

	fclose(fp);

	return 0;
}