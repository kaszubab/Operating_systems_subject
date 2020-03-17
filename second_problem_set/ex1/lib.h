#ifndef files_library

#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "unistd.h"
#include "fcntl.h"
#include "sys/stat.h"
#include "string.h"


#define files_library

int generate_random_strings(char * filename, int string_count, int string_len);
int sort_strings_in_file_sys(char * filename, int string_count, int string_len);
int sort_strings_in_file_lib(char * filename, int string_count, int string_len);
int copy_file_sys(char * file1, char * file2, int string_count, int string_len);
int copy_file_lib(char * file1, char * file2, int string_count, int string_len);

#endif

