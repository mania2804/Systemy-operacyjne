#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

typedef long long ll;

typedef struct {
    int *arr;
    long start;
    long end;
    ll *result;
} ThreadData;


void* sum_subarray(void *arg) {
    ThreadData *data = arg;
    ll sum = 0;
    for (long i = data->start; i < data->end; ++i)
        sum += data->arr[i];

    *(data->result) = sum;
    pthread_exit(NULL);
}

double get_time_in_microseconds() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (double)tv.tv_sec * 1000000 + (double)tv.tv_usec;
}

double measure_time(long n, long k, long t) {
    long total = n * k;
    int *arr = malloc(sizeof(int) * total);

    srand((unsigned)time(NULL));
    for (long i = 0; i < total; ++i)
        arr[i] = rand() % 10;


    ll *k_ele = malloc(sizeof(ll) * k);

    pthread_t threads[t];
    ThreadData thread_data[t];

    long subarrays_per_thread = k / t;
    long remaining_subarrays = k % t;
    long current_subarray = 0;

    double start = get_time_in_microseconds();

    for (long i = 0; i < t; ++i) {
        long subarrays_to_sum = subarrays_per_thread + (i < remaining_subarrays ? 1 : 0);
        thread_data[i].arr = arr;
        thread_data[i].start = current_subarray * n;
        thread_data[i].end = thread_data[i].start + subarrays_to_sum * n;
        thread_data[i].result = &k_ele[current_subarray];
        current_subarray += subarrays_to_sum;

        pthread_create(&threads[i], NULL, sum_subarray, &thread_data[i]);
    }

    for (long i = 0; i < t; ++i) {
        pthread_join(threads[i], NULL);
    }

    double end = get_time_in_microseconds();

    free(arr);
    free(k_ele);
    return end - start;
}

int main(int argc, char *argv[]) {
    long n_values[] = {1000, 5000, 10000, 20000, 50000};
    long k_values[] = {10, 50, 100, 200, 500};
    long t_values[] = {1, 2, 4, 8, 16};

    FILE *fp = fopen("wyniki_nkt.csv", "w");

    for (size_t i = 0; i < sizeof(n_values) / sizeof(n_values[0]); ++i) {
        for (size_t j = 0; j < sizeof(k_values) / sizeof(k_values[0]); ++j) {
            for (size_t l = 0; l < sizeof(t_values) / sizeof(t_values[0]); ++l) {
                long n = n_values[i];
                long k = k_values[j];
                long t = t_values[l];

                double time = measure_time(n, k, t);
                if (time >= 0) {
                    printf("n=%ld, k=%ld, t=%ld: %.2f mikrosekund\n", n, k, t, time);
                    fprintf(fp, "%ld,%ld,%ld,%.2f\n", n, k, t, time);
                }
            }
        }
    }

    fclose(fp);

    return 0;
}
