#ifndef find_lib


#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>//realpath
#include <dirent.h>// stat functions
#include <time.h>
#include <sys/stat.h> //struct stat
#include <string.h>
#include <ftw.h>
#include "unistd.h"
#include "sys/types.h"
#include "sys/wait.h"



#define find_lib

void search_by_stat(char * dir_path, int depth, int mode, int sgn, int days);
void print_stat_info(char * absolute_path, struct stat * stats, unsigned char file_type, char * access_time, char * modification_time);
int check_time(struct tm *input_time, int sgn, int days);


#endif

