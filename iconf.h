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
#ifndef _ICONF_H
#define _ICONF_H

#define BUF_LEN 4096

struct Iconf {
	char **strings;
	int count;
	int errval;
};


void iconf_free(struct Iconf ini);
struct Iconf iconf_load(const char *filename);
char *iconf_get_key(struct Iconf iconf, char *section, char *keyname);

/* key must exist. if section == NULL it updates the first matching key */
/* return value: ok - 0, key not found = 1, allocation problem 2 */
int iconf_set_key(struct Iconf iconf, char *section, char *keyname, char *newvalue);

/* if section == NULL key goes at top of list */
/* int iconf_new_key(struct Iconf iconf, char *section, char *keyname); */
int iconf_write(struct Iconf iconf, char *filename);

#endif /* _ICONF_H */
