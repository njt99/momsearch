#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <vector>
#include <string>

// Takes a tree from an sstream and writes to disk

bool g_recursive = false;
bool g_verbose = false;
bool g_start_is_root = false;
char* g_treeLocation;

FILE* openBox(char* boxcode)
{
	FILE* fp;
	char fileboxcode[10000];
    char fileName[10000];
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

bool processTree(FILE* fp, bool printTree, char* boxcode)
{
    bool success = true;
	int boxdepth = strlen(boxcode);
	char buf[10000];
    char fileName[10000];
	int depth = 0;
	while (fgets(buf, sizeof(buf), fp)) {
        // Open HOLE file if exists. If no printing is set, we don't need to traverse HOLEs
		if (buf[0] == 'H') {
		    if (g_recursive && depth > 0) {
                FILE* fpH = openBox(boxcode);
                if (fpH) {
                    success = processTree(fpH, printTree, boxcode);
                    fclose(fpH);
                    if (!success) {
                        fprintf(stderr, "Fatal! Hole file %s incomplete\n", boxcode);
                        // The tree is incomplete, so we rename the boxfile to mark as incomplete
                        break;
                    }
                }
            }
		} else if (printTree) {
            fprintf(stdout, "%s", buf); 
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
    return false; 
}
		
int main(int argc, char** argv)
{
    bool printTree = true;

	if (argc > 1 && strcmp(argv[1], "-s") == 0) {
        printTree = false;
		++argv;
		--argc;
	}

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
		fprintf(stderr, "Usage: simple_tree_cat [-s] [-v] [-r] treeLocation boxcode\n");
		fprintf(stderr, "s : silent, don't print trees or holes\n");
		fprintf(stderr, "v : verbose\n");
		fprintf(stderr, "r : recur over all subtree files to prince full subtree of boxcode\n");
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
  
	int fileBoxLength = strlen(fullboxcode);

    if ((fileBoxLength == 0) || (strncmp(fullboxcode, "root", fileBoxLength) == 0)) {
        g_start_is_root = true;
    }

    // See if a file with the tree for a prefix of the box exists
	FILE* fp = 0;
	while (fileBoxLength >= 0) {
		fileboxcode[fileBoxLength] = '\0';
		fp = openBox(fileboxcode);
		if (fp) { break; }
        --fileBoxLength;
	}

    if (!fp) { 
        fprintf(stderr, "Fatal! Failed to open boxfile\n");
        exit(2);
    }
    
    char * boxcode_const = (char *)calloc(10000, sizeof(char));
	strncpy(boxcode_const, fullboxcode+fileBoxLength, 10000);
    char * boxcode = boxcode_const;
    
	char buf[10000];
    // If the boxcode is still not empty, we traverse down the tree and print only
    // once we get to the proper node. We terminate early if the node does not exist
	while (*boxcode && fgets(buf, sizeof(buf), fp)) {
		if (buf[0] != 'X') { // If not a splitting, print the test failed by the truncated box
			*boxcode = '\0';
			//fprintf(stderr, "terminal box = %s%s\n", fileboxcode, boxcode_const);
			if (printTree) fputs(buf, stdout);
            free(boxcode_const);
            fclose(fp);
			exit(0);
		}
		if (*boxcode == '1') { // Actually have to process the tree if we go right at any point in the boxcode
		    bool success = processTree(fp, false, boxcode);
            if (!success) {
                fprintf(stderr, "Fatal! Incomplete tree\n");
                exit(2); // Incomplete tree or boxcode not found
            }
        }
		++boxcode; // Keeps going left in the tree as *boxcode == 0
	}

    free(boxcode_const);

	bool success = processTree(fp, printTree, fullboxcode);
    fclose(fp);

    if (!success) {
        // The tree is incomplete, so we rename the boxfile to mark as incomplete
        // TODO: Not sure if treecat should have the power to rename files
        fprintf(stderr, "Fatal! Incomplete tree\n");
        exit(2);
    }
    exit(0); 
}
