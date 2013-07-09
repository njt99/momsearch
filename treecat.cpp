#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>

// Takes a tree from an sstream and writes to disk

bool g_recursive = false;
bool g_verbose = false;
char* g_treeLocation;

FILE* openBox(char* boxcode)
{
	FILE* fp;
	char fileName[1000];
	char fileboxcode[1000];
	strcpy(fileboxcode, boxcode);
    // Open the root file if empty
	if (strcmp(fileboxcode, "") == 0)
		strcpy(fileboxcode, "root");
	sprintf(fileName, "%s/%s.out", g_treeLocation, fileboxcode);
	struct stat sb;
	if (0 == stat(fileName, &sb)) {
		if (g_verbose) fprintf(stderr, "opening %s\n", fileName);
		fp = fopen(fileName, "r");
		return fp;
	}
    // Look for a gzipped file
	sprintf(fileName, "%s/%s.out.gz", g_treeLocation, fileboxcode);
	if (0 == stat(fileName, &sb)) {
		char commandBuf[1000];
		if (g_verbose) fprintf(stderr, "opening %s\n", fileName);
		sprintf(commandBuf, "gzcat %s", fileName); // cool trick
		fp = popen(commandBuf, "r");
		return fp;
	}
	return 0;
}

int processTree(FILE* fp, bool print, char* boxcode)
{
	int boxdepth = strlen(boxcode);
	char buf[10000];
	int depth = 0;
	while (fgets(buf, sizeof(buf), fp)) {
		bool filledHole = false;
        // Open HOLE file is exist, mark as missing otherwise
		if (print && buf[0] == 'H' && g_recursive) {
			FILE* fpH = openBox(boxcode);
			if (fpH) {
				int success = processTree(fpH, print, boxcode);
				fclose(fpH);
				filledHole = true;
                if (!success) return 0;
			} else {
				fprintf(stderr, "missing %s\n", boxcode);
			}
		}
		if (print && !filledHole)
			fputs(buf, stdout); // Print the buffer if we are printing out the filled tree
		if (buf[0] == 'X') {
			boxcode[boxdepth + depth] = '0'; // Descend via left branch
			++depth;
			boxcode[boxdepth + depth] = '\0';
		} else {
            // Go up as many nodes as necessary
			for (; depth > 0 && boxcode[boxdepth + depth-1] == '1'; --depth) {}
			if (depth > 0) {
				boxcode[boxdepth + depth-1] = '1'; // Jump from left to right node
				boxcode[boxdepth + depth] = '\0'; // Truncate to keep box current
			} else {
				boxcode[boxdepth] = '\0'; // Truncate to keep box current
				return 1;
			}
		}
	}
    fprintf(stderr, "premature EOF at %s\n", boxcode);
    return 0; 
}
		
int main(int argc, char** argv)
{
	if (argc > 1 && strcmp(argv[1], "-v") == 0) {
		g_verbose = true;
		++argv;
		--argc;
	}

	if (argc > 1 && strcmp(argv[1], "-r") == 0) {
		g_recursive = true;
		++argv;
		--argc;
	}

	if (argc != 3) {
		fprintf(stderr, "Usage: treecat [-v] [-r]  treeLocation boxcode\n");
		exit(1);
	}
    
    // The fullboxcode parameter can specify the filename and sequetial boxcode
    // A boxcode is just a sequence of zeros and ones giving a posiiton in a binary tree depth-first traversal
    // The treeFile will also be in pre-order depth-first
    char fullboxcode[1000];
	char fileboxcode[1000];
	strncpy(fullboxcode, argv[2], 1000);
	strncpy(fileboxcode, argv[2], 1000);
	g_treeLocation = argv[1];
    
    // See if a file with the tree for a prefix of the box exists
	FILE* fp = 0;
	int fileBoxLength;
	for (fileBoxLength = strlen(fullboxcode); fileBoxLength >= 0; --fileBoxLength) {
		fileboxcode[fileBoxLength] = '\0';
		fp = openBox(fileboxcode);
		if (fp)
			break;
	}

    if (!fp) exit(1);
    
    char * boxcode_const = (char *)calloc(1000, sizeof(char));
    char * boxcode = boxcode_const;
	strncpy(boxcode, fullboxcode+fileBoxLength, 1000);
    
	char buf[10000];
	while (*boxcode && fgets(buf, sizeof(buf), fp)) {
		if (buf[0] != 'X') { // If not a splitting, print the test failed by the truncated box
			*boxcode = '\0';
			fprintf(stderr, "box = %s\n", boxcode_const);
			fputs(buf, stdout);
            free(boxcode_const);
            fclose(fp);
			exit(0);
		}
		if (*boxcode == '1') { // Actually have to process the tree is we go right at any point in the boxcode
		    int success = processTree(fp, false, boxcode);
            if (!success) exit(1); // Incomplete tree!
        }
		++boxcode; // Keeps going left in the tree as *boxcode == 0
	}
    free(boxcode_const);
	int success = processTree(fp, true, fullboxcode);
    fclose(fp);
    if (!success) exit(1);
    else exit(0);
}
