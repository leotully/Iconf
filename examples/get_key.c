#include <stdio.h>

#include <iconf.h>
#include "util.h" /* dump_file, write_sample */

int main(void)
{
	struct Iconf sample;
	int loaderr = 1;
		
	write_sample("sample.ini");
	
	sample = iconf_load("sample.ini");
	
	/* check load error */
	if (sample.errval == 1)
		printf("file access problem\n");
	else if (sample.errval == 2)
		printf("memory allocation problem\n");
	else
		loaderr = 0;
	
	if (loaderr)
		return 1;
		
	if (dump_file("sample.ini", DF_TITLE | DF_FOOTER) != 0)
		return 1;
	
	/* get key value within section */
	printf("%s\n", iconf_get_key(sample, "section1", "first"));
	
	/* get first key value regardless of section */
	printf("%s\n", iconf_get_key(sample, NULL, "first"));
	
	/* parsing with whitespace is same */
	printf("\nParsing with whitespace\n");
	printf("-----------------------\n");
	printf("key 'one'  : %s\n", iconf_get_key(sample, "white space", "one"));
	printf("key 'two'  : %s\n", iconf_get_key(sample, "white space", "two"));
	printf("key 'three': %s\n", iconf_get_key(sample, "white space", "three"));
	
	iconf_free(sample);		
	
	return 0;
}
