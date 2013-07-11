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

FILE* openBox(char* boxcode, char* fileName)
{
	FILE* fp;
	char fileboxcode[10000];
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
		char commandBuf[10000];
		if (g_verbose) fprintf(stderr, "opening %s\n", fileName);
		sprintf(commandBuf, "gzcat %s", fileName); // cool trick
		fp = popen(commandBuf, "r");
		return fp;
	}
	return 0;
}

bool  putStream(FILE* dest, FILE* source) {
    size_t size;
    char buf[BUFSIZ];
    while ((size = fread(buf, 1, BUFSIZ, source))) {
        fwrite(buf, 1, size, dest);
    }
    if (ferror(dest) || ferror(source))
        return false;
    else
        return true;
}

bool markIncomplete(char const* fileName) {
    char fileNameI[10000];
    strcpy(fileNameI,fileName);
    strcat(fileNameI, ".incomplete");
    return (rename(fileName, fileNameI) == 0);
}

bool processTree(FILE* fp, FILE* out, bool printTree, bool printHoles, char* boxcode)
{
	int boxdepth = strlen(boxcode);
	char buf[10000];
    char fileName[10000];
    bool print = (printTree || printHoles);
	int depth = 0;
	while (fgets(buf, sizeof(buf), fp)) {
		bool filledHole = false;
        // Open HOLE file if exists. If no printing is set, we don't need to traverse HOLEs
		if (print && buf[0] == 'H' && g_recursive) {
			FILE* fpH = openBox(boxcode, fileName);
			if (fpH) {
                FILE* outH = tmpfile();
                if (outH) {
                    bool success = processTree(fpH, outH, printTree, printHoles, boxcode);
                    fclose(fpH);
                    if (!success) {
                        fclose(outH);
                        // The tree is incomplete, so we rename the boxfile to mark as incomplete
                        // TODO: Not sure if treecat should have the power to rename files
                        if (!markIncomplete(fileName)) 
                            fprintf(stderr, "failed to mark %s as incomplete\n", fileName);
                    } else {
                        // If the HOLE subtree is complete, print it to the output stream
                        rewind(outH);
                        bool success = putStream(out,outH);
                        fclose(outH);
                        if (!success) {
                            fprintf(stderr, "failed to print HOLE %s subtree\n", boxcode);
                            break;
                        } else {
                            filledHole = true;
                        }
                    }
                }
			}
            if (printHoles && !filledHole) 
                fprintf(out, "%s\n", boxcode); // Print the missing boxcode to stdout
		}
		if (printTree && !filledHole) {
			fputs(buf, out); // Print the buffer if we are printing out the filled tree
        }
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
				return true;
			}
		}
	}
    
    // If we get to this point, the tree is incomplete
    // TODO: This may be unnecssary code as we rename incomplete trees
    if (printHoles) {
        // Print the box we "should" be at as missing
        fprintf(out, "%s\n", boxcode); // Print the missing boxcode to stdout
    
        // We list all other missing boxes
        for (int i = depth; i > 0; --i) {
            if (boxcode[boxdepth + i-1] != '1') {
                boxcode[boxdepth + i-1] = '1'; // Jump from left to right node
                boxcode[boxdepth + i] = '\0'; // Truncate to keep box current
                fprintf(out, "%s\n", boxcode); // Print the missing boxcode to stdout
            } else {
                boxcode[boxdepth + i] = '\0'; // Truncate to keep box current
            } 
        } 
    }           

    return false; 
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

    bool printTree = true;
    bool printHoles = false;
	if (argc > 1 && strcmp(argv[1], "--holes") == 0) {
        printTree = false;
        printHoles = true;
		++argv;
		--argc;
	}

	if (argc != 3) {
		fprintf(stderr, "Usage: treecat [-v] [-r] [--holes]  treeLocation boxcode\n");
		exit(1);
	}
    
    // The fullboxcode parameter can specify the fileName and sequetial boxcode
    // A boxcode is just a sequence of zeros and ones giving a posiiton in a binary tree depth-first traversal
    // The treeFile will also be in pre-order depth-first
    char fullboxcode[10000];
	char fileboxcode[10000];
	strncpy(fullboxcode, argv[2], 10000);
	strncpy(fileboxcode, argv[2], 10000);
	g_treeLocation = argv[1];
    
    // See if a file with the tree for a prefix of the box exists
	FILE* fp = 0;
	int fileBoxLength;
    char fileName[10000];
	for (fileBoxLength = strlen(fullboxcode); fileBoxLength >= 0; --fileBoxLength) {
		fileboxcode[fileBoxLength] = '\0';
		fp = openBox(fileboxcode, fileName);
		if (fp)
			break;
	}

    if (!fp) exit(1);
    
    char * boxcode_const = (char *)calloc(10000, sizeof(char));
    char * boxcode = boxcode_const;
	strncpy(boxcode, fullboxcode+fileBoxLength, 10000);
    
	char buf[10000];
    // If the boxcode is still not empty, we traverse down the tree and print only
    // once we get to the proper node. We terminate early if the node does not exist
	while (*boxcode && fgets(buf, sizeof(buf), fp)) {
		if (buf[0] != 'X') { // If not a splitting, print the test failed by the truncated box
			*boxcode = '\0';
			fprintf(stderr, "terminal box = %s\n", boxcode_const);
			fputs(buf, stdout);
            free(boxcode_const);
            fclose(fp);
			exit(0);
		}
		if (*boxcode == '1') { // Actually have to process the tree is we go right at any point in the boxcode
		    int success = processTree(fp, NULL, false, false, boxcode);
            if (!success) exit(1); // Incomplete tree or boxcode not found
        }
		++boxcode; // Keeps going left in the tree as *boxcode == 0
	}
    free(boxcode_const);
    FILE* out = tmpfile();
    if (!out) exit(1);

	bool success = processTree(fp, out, printTree, printHoles, fullboxcode);
    fclose(fp);
 
   if (!success) {
        fclose(out);
        // The tree is incomplete, so we rename the boxfile to mark as incomplete
        // TODO: Not sure if treecat should have the power to rename files
        if (!markIncomplete(fileName))
            fprintf(stderr, "failed to mark %s as incomplete\n", fileName);
        exit(1);
    } else {
        rewind(out);
        bool putSuccess = putStream(stdout, out);
        fclose(out);
        if (!putSuccess) exit(1);
        else exit(0); 
    }
}
