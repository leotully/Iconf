/*-----------------------------------------------------------------------------
Copyright (c) 2022 Leo Tully - All Rights Reserved

You may use, copy, modify, merge, publish, etc. this software under the terms
of the MIT License.
-----------------------------------------------------------------------------*/
#ifndef _ICONF_H
#define _ICONF_H

#define BUF_LEN 1025

struct Iconf {
	char **strings;
	int count;
	int errval;
};

/* loads configuration file strings.
 * sets string count, including all comments and blank lines.
 * sets error reporting variable 'Iconf.errval'.
 * 
 * Iconf.errval: ok = 0, can't open file = 1;
 */
struct Iconf iconf_load(const char *filename);

/* iconf_get_key
 * if section is set to NULL iconf_get_key will return the first matching key.
 * 
 * return value: key value string. if return is NULL check the Iconf.errval.
 * Iconf.errval: ok = 0, key_not_found = 1, allocation problem 2
 */
char *iconf_get_key(struct Iconf iconf, char *section, char *keyname);

int iconf_get_int(struct Iconf iconf, char *section, char *keyname);
long iconf_get_long(struct Iconf iconf, char *section, char *keyname);

/* iconf_set_key
 * key must exist. if section is set to NULL iconf_set_key will update the
 * first matching key.
 *
 * return value: ok - 0, key not found = 1, allocation problem = 2
 */
int iconf_set_key(struct Iconf iconf, char *section, char *keyname, char *newvalue);

int iconf_write(struct Iconf iconf, char *filename);
void iconf_free(struct Iconf ini);

#endif /* _ICONF_H */

