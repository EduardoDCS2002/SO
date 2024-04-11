#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

long int time_diff(struct timeval *start, struct timeval *end) {
    return (1e+3 *(end->tv_sec - start->tv_sec)) + (1e-3 * (end->tv_usec - start->tv_usec));
}

int main() {
    struct timeval start, end;

    // Measure time for code block 1
    gettimeofday(&start, NULL);
    sleep(1);
    gettimeofday(&end, NULL);
    printf("1 - time spent: %ld milisec\n", time_diff(&start, &end));

    // Measure time for code block 2
    gettimeofday(&start, NULL);
    sleep(2);
    gettimeofday(&end, NULL);
    printf("2 - time spent: %ld milisec\n", time_diff(&start, &end));

    return 0;
}