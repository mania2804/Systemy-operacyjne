#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

typedef long long ll;

ll suma_watków = 0;

typedef struct {
    int *tab;
    long start;
    long end;
    ll result;
} Watek;

void *sum_1_watek(void *arg) {
    Watek *watek = (Watek *)arg;
    ll suma = 0;
    for (long i = watek->start; i < watek->end; i++) {
        suma += watek->tab[i];
    }
    watek->result = suma;
    pthread_exit(NULL);
}

double czas() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (double)tv.tv_sec * 1000000 + (double)tv.tv_usec;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Użycie: %s <n> <k> <t>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    int k = atoi(argv[2]);
    int t = atoi(argv[3]);
    int total = n * k;

    if (n <= 0 || t <= 0 || k <= 0) {
        printf("Wartości muszą być dodatnie.\n");
        return 1;
    }

    int *tab = malloc(sizeof(int) * total);
    srand(time(NULL));
    for (int i = 0; i < total; i++) {
        tab[i] = rand() % 10;
    }

    // Sumowanie sekwencyjne
    double start1 = czas();
    ll suma = 0;
    for (int i = 0; i < total; i++) {
        suma += tab[i];
    }
    double end1 = czas();
    printf("\nSuma sekwencyjna: %lld", suma);
    printf("\nCzas sekwencyjny: %f mikrosekund", end1 - start1);

    // Tworzenie wątków
    pthread_t *watki = malloc(t * sizeof(pthread_t));
    Watek **watki_data = malloc(t * sizeof(Watek*));

    long il_tab_1_wat = total / t;
    long pozostale_wat = total % t;
    long start = 0;

    double start2 = czas();
    for (int i = 0; i < t; i++) {
        watki_data[i] = malloc(sizeof(Watek));
        long end = start + il_tab_1_wat + (pozostale_wat > 0 ? 1 : 0);
        watki_data[i]->tab = tab;
        watki_data[i]->start = start;
        watki_data[i]->end = end;
        start = end;
        pozostale_wat--;
        pthread_create(&watki[i], NULL, sum_1_watek, watki_data[i]);
    }

    for (int i = 0; i < t; i++) {
        pthread_join(watki[i], NULL);
    }
    double end2 = czas();

    for (int i = 0; i < t; i++) {
        suma_watków += watki_data[i]->result;
        free(watki_data[i]);
    }
    free(watki_data);
    free(watki);

    printf("\nSuma wątkowa: %lld", suma_watków);
    printf("\nCzas wątków: %f mikrosekund", end2 - start2);


    free(tab);
    return 0;
}
