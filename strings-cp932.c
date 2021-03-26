#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// needs to be at least 2
#define BUFLEN 4096

typedef unsigned long Rune;
#include "_cp932tab.c"

static inline void
eputchar(int c)
{
	if(putchar(c) == EOF) {
		fputs("putchar: write error\n", stderr);
		exit(EXIT_FAILURE);
	}
}

int
strings(FILE *f, char offfmt)
{
	char fmt[] = {'%', offfmt, ' ', '\0'};
	unsigned char buf[BUFLEN], *c = buf, *cend;
	size_t n;
	unsigned long long offset = 0;
	bool waschar = 0;
	while(!feof(f)) {
		if((n = fread(c, 1, BUFLEN-(c-buf), f)) != BUFLEN-(c-buf) && ferror(f)) {
			fprintf(stderr, "read error at %ld\n", ftell(f));
			return EXIT_FAILURE;
		}
		cend = c+n;
		c = buf;

		while(c < cend) {
			if((*c < 0x7f && *c >= 0x20) || (*c >= 0xa1 && *c <=0xdf)) {
				if(!waschar) {
					eputchar('\n');
					waschar = 1;
					if(offfmt)
						printf(fmt, offset);
				}
				eputchar(*c++);
				offset++;
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
					if(!waschar) {
						eputchar('\n');
						waschar = 1;
						if(offfmt)
							printf(fmt, offset);
					}
					eputchar(*c++);
					eputchar(*c++);
					offset += 2;
					continue;
				}
			}

			waschar = 0;
			c++;
			offset++;
		}

		if(c == cend)
			c = buf;
	}
	eputchar('\n');

	return EXIT_SUCCESS;
}

enum opt {
	OPT_NONE,
	OPT_OFFFMT,
};

struct opts {
	char offfmt;
};

void
argparse(int *argc, char **argv, struct opts *opts)
{
	enum opt nextopt = OPT_NONE;
	for(int i = 1; i < *argc; i++) {
		if(argv[i][0] == '-') {
			switch(argv[i][1]) {
			case 'a':
				break;
			case 't':
				nextopt = OPT_OFFFMT;
				break;
			default:
				fprintf(stderr, "unrecognized option %s\n", argv[i]);
			}
			if(i+1 < *argc) {
				memmove(&argv[i], &argv[i+1], (*argc-i) * (sizeof argc));
				i--;
			}
			(*argc)--;
		} else if(nextopt != OPT_NONE) {
			switch(nextopt) {
			case OPT_OFFFMT:
				if(argv[i][0] != '\0' && argv[i][1] != '\0') {
					fprintf(stderr, "unrecognized offset format %s\n", argv[i]);
					break;
				} else if(argv[i][0] != 'd' && argv[i][0] != 'x' && argv[i][0] != 'o') {
					fprintf(stderr, "unrecognized offset format %c\n", argv[i][0]);
					break;
				}
				opts->offfmt = argv[i][0];
				break;
			}
			nextopt = OPT_NONE;
			if(i+1 < *argc) {
				memmove(&argv[i], &argv[i+1], (*argc-i) * (sizeof argc));
				i--;
			}
			(*argc)--;
		}
	}

	if(nextopt != OPT_NONE)
		fprintf(stderr, "no argument given for -%c, ignoring\n", "\0t"[nextopt]);
}

int
main(int argc, char **argv)
{
	struct opts opts = {
		.offfmt = '\0',
	};
	argparse(&argc, argv, &opts);

	if(argc < 2)
		return strings(stdin, opts.offfmt);

	for(int i = 1; i < argc; i++) {
		FILE *f = fopen(argv[i], "rb");
		if(f == NULL) {
			fputs("failed to open ", stderr);
			perror(argv[i]);
			continue;
		}
		if(strings(f, opts.offfmt) != EXIT_SUCCESS)
			return EXIT_FAILURE;
		fclose(f);
	}

	return EXIT_SUCCESS;
}
