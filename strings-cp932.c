#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// needs to be at least 2
#define BUFLEN 4096

typedef unsigned long Rune;
#include "_cp932tab.c"

int
strings(FILE *f)
{
	unsigned char buf[BUFLEN], *c = buf, *cend;
	size_t n;
	_Bool waschar = 0;
	while(!feof(f)) {
		if((n = fread(c, 1, BUFLEN-(c-buf), f)) != BUFLEN-(c-buf) && ferror(f)) {
				fprintf(stderr, "read error at %ld\n", ftell(f));
				return EXIT_FAILURE;
		}
		cend = c+n;
		c = buf;

		while(c < cend) {
			if((*c < 0x7f && *c >= 0x20) || (*c >= 0xa1 && *c <=0xdf)) {
				if(!waschar) { putchar('\n'); waschar = 1; }
				putchar(*c++);
				continue;
			}

			if(
				(*c >= 0x81 && *c <= 0x84) ||
				(*c >= 0x87 && *c <= 0x9f) ||
				(*c >= 0xe0 && *c <= 0xea) || // sjis
				(*c >= 0xed && *c <= 0xee) ||
				(*c >= 0xfa && *c <= 0xfc) // cp932 extensions
			) {
				if(cend-c < 2) {
					memmove(buf, c, cend-c);
					c = buf + (cend-c);
					break;
				}

				if(cp932tab[c[1] | (c[0] << 8)] != 0) {
					if(!waschar) { putchar('\n'); waschar = 1; }
					putchar(*c++);
					putchar(*c++);
					continue;
				}
			}

			waschar = 0;
			c++;
		}

		if(c == cend)
			c = buf;
	}
	putchar('\n');

	return EXIT_SUCCESS;
}

int
main(int argc, char **argv)
{
	if(argc < 2)
		return strings(stdin);
	return strings(fopen(argv[1], "rb"));
}
