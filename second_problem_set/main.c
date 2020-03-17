#include "lib.h"

int main()
{
    generate_random_strings("test.txt", 10, 50);
    copy_file_sys("test.txt", "test_sys.txt", 10, 50);
    copy_file_lib("test.txt", "test_lib.txt", 10, 50);
    return 0;
}