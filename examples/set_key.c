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
		
	if (dump_file("sample.ini", DF_FOOTER) != 0)
	{
		printf("problem dumping file\n");
		return 1;
	}

	printf("Setting keys and writing file. zeros = success... ");

	/* set key value within section */
	printf("%d, ", iconf_set_key(sample, "section1", "first", "1st"));
	
	/* set first key value regardless of section */
	printf("%d, ", iconf_set_key(sample, NULL, "first", "1st"));

	/* set keys with whitespace, retains formatting */
	printf("%d, ", iconf_set_key(sample, "white space", "one", "one"));
	printf("%d, ", iconf_set_key(sample, "white space", "two", "two"));
	printf("%d, ", iconf_set_key(sample, "white space", "three", "three"));

	/* write updated keys file */
	printf("%d\n", iconf_write(sample, "sample.ini"));

	if (dump_file("sample.ini", DF_HEADER) != 0)
	{
		printf("problem dumping file\n");
		return 1;
	}

	iconf_free(sample);		
	
	return 0;
}