
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include "library.h"
#include <sys/times.h>
#include <time.h>

void add_and_delete_n_blocks(operation_block_array * arr, files_pair_array * files, char * filename, int count)
{
    for(int i = 0;i<count;i++){
        prepare_block(filename, arr);
        remove_block(arr, count+i);
    }
}

void delete_blocks(operation_block_array * arr, int count)
{
    for(int i = 0;i<count;i++){
        remove_block(arr, i);
    }
}


double get_time(clock_t start, clock_t end)
{
    return (double) (end - start) / sysconf(_SC_CLK_TCK);
}

int main(int argc, char** argv)
{
    printf("tests for %s dataset\n", argv[1]);

    int count = argc - 2;
    files_pair_array * files = make_sequence(argv+2, count);
    operation_block_array * op_blocks = create_table(2*count + 2);

    struct tms **tms_time = calloc(6, sizeof(struct tms *));
    clock_t real_time[6];
    for (int i = 0; i < 6; i++) {
        tms_time[i] = (struct tms *) malloc(sizeof(struct tms));
    }

    real_time[0] = times(tms_time[0]);
    compare_pairs(op_blocks, files);
    real_time[1] = times(tms_time[1]);

    size_t block_size =  number_of_operations(op_blocks, 0);

    real_time[2] = times(tms_time[2]);
    delete_blocks(op_blocks, count);
    real_time[3] = times(tms_time[3]);

    real_time[4] = times(tms_time[4]);
    add_and_delete_n_blocks(op_blocks, files,"tmp.txt" ,count);
    real_time[5] = times(tms_time[5]);


    printf("for block size %zu\n", block_size);

    printf("  REAL    User    System\n");

    printf("compare %d pairs\n", count);
    printf("%lf   ", get_time(real_time[0], real_time[1]));
    printf("%lf   ", get_time(tms_time[0]->tms_utime, tms_time[1]->tms_utime));
    printf("%lf   \n", get_time(tms_time[0]->tms_stime, tms_time[1]->tms_stime));

    printf("delete block mean\n");
    printf("%lf   ", get_time(real_time[2], real_time[3]) / count);
    printf("%lf   ", get_time(tms_time[2]->tms_utime, tms_time[3]->tms_utime) / count);
    printf("%lf   \n", get_time(tms_time[2]->tms_stime, tms_time[3]->tms_stime) / count);


    printf("add and delete mean\n");
    printf("%lf   ", get_time(real_time[4], real_time[5]) / count);
    printf("%lf   ", get_time(tms_time[4]->tms_utime, tms_time[5]->tms_utime) / count);
    printf("%lf   \n", get_time(tms_time[4]->tms_stime, tms_time[5]->tms_stime) / count);
    printf("\n\n");
}