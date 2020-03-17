#include "lib.h"


void quicksort(int * arr, int left, int right);
int partition(int * arr, int left, int right);

int main()
{
    generate_random_strings("test.txt", 10, 50);
    copy_file_sys("test.txt", "test_sys.txt", 10, 50);
    copy_file_sys("test.txt", "test_lib.txt", 10, 50);
    sort_strings_in_file_sys("test_sys.txt", 10, 50);
    sort_strings_in_file_lib("test_lib.txt", 10, 50);
    return 0;
}
