#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <vector>
#include <string>
#include <getopt.h>

struct Config {
  bool print_tree = true;
  bool print_holes = false;
  bool print_killed = false;
  bool mark_incomplete = false;
  bool recursive = false;
  bool verbose = false;
  bool silent = false;
  bool start_is_root = false;
  char* tree_location;
  char kill_test[1000];
};

struct Config g_config;

std::vector<std::string> unopened_out_files;

FILE* open_box(char* boxcode, char* file_name)
{
	FILE* fp;
	char boxcode_file[10000];
	strcpy(boxcode_file, boxcode);
  // Open the root file if empty
	if (strcmp(boxcode_file, "") == 0) {
		strcpy(boxcode_file, "root");
  }
	sprintf(file_name, "%s/%s.out", g_config.tree_location, boxcode_file);
	struct stat sb;
	if (0 == stat(file_name, &sb)) {
		if (g_config.verbose) {
      fprintf(stderr, "opening %s\n", file_name);
    }
    if (g_config.start_is_root && g_config.mark_incomplete && g_config.recursive) {
      for (std::vector<std::string>::iterator it = unopened_out_files.begin() ; it != unopened_out_files.end(); ++it) {
        if (it->compare(file_name) == 0) {
          unopened_out_files.erase(it);
          break;
        }
      }
    }
    fp = fopen(file_name, "r");
    return fp;
	}
  // Look for a bzipped file
	sprintf(file_name, "%s/%s.out.bz2", g_config.tree_location, boxcode_file);
	if (0 == stat(file_name, &sb)) {
		char command_buf[10000];
		if (g_config.verbose) {
      fprintf(stderr, "opening %s\n", file_name);
    }
		sprintf(command_buf, "bzcat \'%s\'", file_name); // cool trick
    fp = popen(command_buf, "r");
		return fp;
	}
  // Look for a gzipped file
	sprintf(file_name, "%s/%s.out.tar.gz", g_config.tree_location, boxcode_file);
	if (0 == stat(file_name, &sb)) {
		char command_buf[10000];
		if (g_config.verbose) {
      fprintf(stderr, "opening %s\n", file_name);
    }
		sprintf(command_buf, "tar -xOzf \'%s\'", file_name); // cool trick
    fp = popen(command_buf, "r");
		return fp;
	}
	return 0;
}

bool ends_with(const char *str, const char *suffix)
{
  if (!str || !suffix) {
    return false;
  }
  size_t lenstr = strlen(str);
  size_t lensuffix = strlen(suffix);
  if (lensuffix > lenstr) {
    return false;
  }
  return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}

#define BUF_SIZE 1048576

bool  put_stream(FILE* dest, FILE* source) {
  size_t size;
  char buf[BUFSIZ];
  size_t total = 0;
  while ((size = fread(buf, 1, BUFSIZ, source))) {
    fwrite(buf, 1, size, dest);
    total += size;
  }
  if (ferror(dest) != 0) {
    fprintf(stderr, "failed destination: bytes %d/%d\n", total, BUFSIZ);
    return false;
  } else if (ferror(source) != 0) {
    fprintf(stderr, "failed source: bytes %d/%d\n", total, BUFSIZ);
    return false;
  } else {
    return true;
  }
}

bool mark_file(char const* file_name, const char* ending_mark) {
  char marked_file_name[10000];
  strcpy(marked_file_name,file_name);
  if (ending_mark != NULL) {
    strcat(marked_file_name, ending_mark);
  } else {
    strcat(marked_file_name, ".incomplete");
  }
  return (rename(file_name, marked_file_name) == 0);
}

bool process_tree(FILE* fp, FILE* out, char* boxcode) {
	int boxdepth = strlen(boxcode);
	char buf[10000];
	char tmp[10000];
  char file_name[10000];
	int depth = 0;
	while (fgets(buf, sizeof(buf), fp)) {
    // Open HOLE file if exists. If no printing is set, we don't need to traverse HOLEs
		bool hole_filled = false;
		if (buf[0] == 'H') {
		  if (g_config.recursive && depth > 0) {
        FILE* fp_hole = open_box(boxcode, file_name);
        if (fp_hole) {
          FILE* hole_out = tmpfile();
          if (hole_out) {
            bool success = process_tree(fp_hole, hole_out, boxcode);
            fclose(fp_hole);
            if (!success) {
              fclose(hole_out);
              // The tree is incomplete, so we rename the boxfile to mark as incomplete
              // TODO: Not sure if treecat should have the power to rename files
              if (g_config.mark_incomplete) {
                if (!mark_file(file_name,".incomplete")) {
                  fprintf(stderr, "failed to mark %s as incomplete\n", file_name);
                } 
              }
            } else {
              // If the HOLE subtree is complete, print it to the output stream
              rewind(hole_out);
              bool success = put_stream(out, hole_out);
              fclose(hole_out);
              if (!success) {
                fprintf(stderr, "failed to print HOLE %s subtree\n", boxcode);
                break;
              } else {
                hole_filled = true;
              }
            }
          }
        }
      }
      if (!hole_filled && g_config.print_holes)  {
        fprintf(out, "%s\n", boxcode); // Print the missing boxcode to stdout
      }
		}
    if (g_config.print_tree && !hole_filled) {
			fputs(buf, out); // Print the buffer if we are printing out the filled tree
    }
    if (g_config.print_killed && strncmp(g_config.kill_test, buf, strlen(g_config.kill_test)) == 0) {
      size_t end = strlen(buf);
      strncpy(tmp, buf, end);
      tmp[end-1] = '\0';
      fprintf(out, "%s: %s\n", tmp, boxcode); // Print the killed boxcode to stdout
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
  if (g_config.print_holes > 0) {
    // Print the box we "should" be at as missing/hole
    fprintf(out, "%s\n", boxcode); // Print the missing/hole boxcode to stdout

    // We list all other missing boxes
    for (int i = depth; i > 0; --i) {
      if (boxcode[boxdepth + i-1] != '1') {
        boxcode[boxdepth + i-1] = '1'; // Jump from left to right node
        boxcode[boxdepth + i] = '\0'; // Truncate to keep box current
        fprintf(out, "%s\n", boxcode); // Print the missing/hole boxcode to stdout
      } else {
        boxcode[boxdepth + i] = '\0'; // Truncate to keep box current
      } 
    } 
  }           
  return false; 
}

void usage() {
    fprintf(stderr, "Usage: treecat [--only_holes] [--mark_incomplete] [--killed_by] <test> [-s] [-v] [-r] tree_location boxcode\n");
    fprintf(stderr, "only_holes: do not print the tree but print the holes.\n");
    fprintf(stderr, "mark_incomplete: rename incomplete (sub)tree file(s) containing given boxcode (sub if -r)\n");
    fprintf(stderr, "killed_by: give all terminal nodes from boxcode killed by the given test\n");
    fprintf(stderr, "s: silent, don't print trees or holes\n");
    fprintf(stderr, "v: verbose\n");
    fprintf(stderr, "r: recur over all subtree files to prince full subtree of boxcode\n");
    fprintf(stderr, "WARNING: If --mark_incomplete -r are set and boxcode is root or '', then mark any foreign tree files\n");
    exit(1);
}
		
static struct option long_options[] = {
  {"open_holes",  no_argument, NULL, 'o' },
  {"mark_incomplete", no_argument, NULL, 'm' },
  {"silent", no_argument, NULL, 's'},
  {"recursive", no_argument, NULL, 'r' },
  {"verbose", no_argument, NULL, 'v' },
  {"killed_by", required_argument, NULL, 'k' },
  {NULL, 0, NULL, 0}
};

static char opt_str[1000] = "";
void set_opt_str() {
  char* osp = opt_str;
  for (int i = 0; long_options[i].name; ++i) {
    *osp++ = long_options[i].val;
    if (long_options[i].has_arg != no_argument)
      *osp++ = ':';
  }
  *osp = '\0';
}

int main(int argc, char** argv)
{
  set_opt_str();
  if (argc < 2) {
    usage();
    exit(1);
  }

  int ch;
  while ((ch = getopt_long(argc, argv, opt_str, long_options, NULL)) != -1) {
    switch(ch) {
      case 'o': {
        g_config.print_tree = false;
        g_config.print_holes = true;
        break;
      }
      case 'm': g_config.mark_incomplete = true; break; 
      case 's': g_config.silent = true; break;
      case 'r': g_config.recursive = true; break;
      case 'v': g_config.verbose = true; break;
      case 'k': {
        strncpy(g_config.kill_test, optarg, sizeof(g_config.kill_test));
        // size_t end = strlen(optarg);
        // g_config.kill_test[end] = '\0';
        // g_config.kill_test[end+1] = '\0'; 
        g_config.print_killed = true;
        break;
      }
      default: usage(); exit(1);
    }
  }
  if (g_config.silent) { 
    g_config.print_tree = false;
    g_config.print_holes = false;
    g_config.print_killed = false;
  }
  if (g_config.print_killed) {
    g_config.print_tree = false;
    g_config.print_holes = false;
  }  

  // The fullboxcode parameter can specify the file_name and sequetial boxcode
  // A boxcode is just a sequence of zeros and ones giving a posiiton in a binary tree depth-first traversal
  // The treeFile will also be in pre-order depth-first
  char fullboxcode[10000];
	char boxcode_file[10000];

  if (optind + 1 < argc) {
	  g_config.tree_location = argv[optind];
    strncpy(fullboxcode,  argv[optind+1], 10000);
    strncpy(boxcode_file, argv[optind+1], 10000);
  } else {
    usage();
    exit(1);
  }
  
	int code_length = strlen(fullboxcode);
  if (code_length == 0 || strncmp(fullboxcode, "root", code_length) == 0) {
    g_config.start_is_root = true;
  }

  // TODO: Taging of foreign files may be dangerous 
  char file_name[10000];
  if (g_config.start_is_root && g_config.mark_incomplete && g_config.recursive) {
    DIR * dirp = opendir(g_config.tree_location);
    struct dirent * dp;
    while ((dp = readdir(dirp)) != NULL) {
      if (ends_with(dp->d_name,"out")) {
       sprintf(file_name, "%s/%s", g_config.tree_location, dp->d_name);
         unopened_out_files.push_back(std::string(file_name));
      }
    }
  }   
 
  // See if a file with the tree for a prefix of the box exists
	FILE* fp = 0;
	while (code_length >= 0) {
		boxcode_file[code_length] = '\0';
		fp = open_box(boxcode_file, file_name);
		if (fp) {
      break;
    }
    --code_length;
	}

  if (!fp) {
    fprintf(stderr, "no box files at tree location\n");
    exit(1);
  }
    
  char * boxcode_const = (char *)calloc(10000, sizeof(char));
	strncpy(boxcode_const, fullboxcode+code_length, 10000);
  char * boxcode = boxcode_const;

	char buf[10000];
  // If the boxcode is still not empty, we traverse down the tree and print only
  // once we get to the proper node. We terminate early if the node does not exist
	while (*boxcode && fgets(buf, sizeof(buf), fp)) {
		if (buf[0] != 'X') { // If not a splitting, print the test failed by the truncated box
			*boxcode = '\0';
			fprintf(stderr, "terminal box = %s%s\n", boxcode_file, boxcode_const);
			if (g_config.print_tree) {
        fputs(buf, stdout);
      }
      free(boxcode_const);
      fclose(fp);
			exit(0);
		}
		if (*boxcode == '1') { // Actually have to process the tree if we go right at any point in the boxcode
      FILE* dev_null = fopen("/dev/null","w");
      int success = process_tree(fp, dev_null, boxcode);
      fclose(dev_null); 
      if (!success) exit(1); // Incomplete tree or boxcode not found
    }
		++boxcode; // Keeps going left in the tree as *boxcode == 0
	}

  free(boxcode_const);
  FILE* out = tmpfile();
  if (!out) {
    exit(1);
  }

	bool success = process_tree(fp, out, fullboxcode);
  fclose(fp);

  if (g_config.start_is_root && g_config.mark_incomplete && g_config.recursive) { 
    for (std::vector<std::string>::iterator it = unopened_out_files.begin() ; it != unopened_out_files.end(); ++it) {
      fprintf(stderr, "unopened/foreign out file = %s\n", it->c_str());
      // TODO: Not sure if treecat should have the power to rename files
      if (!mark_file(it->c_str(),".foreign")) {
        fprintf(stderr, "failed to mark %s as foreign\n", it->c_str());
      }
    }
  }
  if (!success) {
    fclose(out);
    // The tree is incomplete, so we rename the boxfile to mark as incomplete
    // TODO: Not sure if treecat should have the power to rename files
    if (g_config.mark_incomplete) {
      if (!mark_file(file_name,".incomplete")) {
        fprintf(stderr, "failed to mark %s as incomplete\n", file_name);
      }
    }
    exit(1);
  } else {
    rewind(out);
    bool put_success = put_stream(stdout, out);
    fclose(out);
    if (!put_success) {
      fprintf(stderr, "failed to write final output\n");
      exit(1);
    }
    else exit(0); 
  }
}
