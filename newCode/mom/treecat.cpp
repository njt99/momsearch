#include <stdio.h>
#include <stdlib.h>
void processTree(bool print)
{
	char buf[10000];
	int depth = 0;
	while (fgets(buf, sizeof(buf), stdin)) {
		if (print)
			fputs(buf, stdout);
		if (buf[0] == 'X') {
			++depth;
		} else {
			if (--depth < 0)
				break;
		}
	}
}
		
int main(int argc, char** argv)
{
	if (argc != 2) {
		fprintf(stderr, "Usage: treecat boxcode\n");
		exit(1);
	}

	char* boxcode = argv[1];
	char buf[10000];
	while (*boxcode && fgets(buf, sizeof(buf), stdin)) {
		if (buf[0] != 'X') {
			*boxcode = '\0';
			fprintf(stderr, "box = %s\n", argv[1]);
			fputs(buf, stdout);
			exit(0);
		}
		if (*boxcode == '1')
			processTree(false);
		++boxcode;
	}
	processTree(true);
}
