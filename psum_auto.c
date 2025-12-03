#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <time.h>
#include <math.h>

typedef long long ll;

typedef struct {
    int *tab;
    long start;
    long end;
    ll result;
} Watek;

void *sum_1_watek(void *arg) {
    Watek *w = (Watek*) arg;
    ll suma = 0;
    for (long i = w->start; i < w->end; i++)
        suma += w->tab[i];
    w->result = suma;
    return NULL;
}

double czas() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (double)tv.tv_sec * 1e6 + (double)tv.tv_usec;
}

ll suma_watkowa(int *tab, int m, int t) {
    pthread_t *watki = malloc(t * sizeof(pthread_t));
    Watek *dane = malloc(t * sizeof(Watek));

    long chunk = m / t;
    long extra = m % t;

    long start = 0;
    for (int i = 0; i < t; i++) {
        long end = start + chunk + (extra > 0 ? 1 : 0);
        extra--;

        dane[i].tab = tab;
        dane[i].start = start;
        dane[i].end = end;

        pthread_create(&watki[i], NULL, sum_1_watek, &dane[i]);
        start = end;
    }

    ll suma = 0;
    for (int i = 0; i < t; i++) {
        pthread_join(watki[i], NULL);
        suma += dane[i].result;
    }

    free(watki);
    free(dane);
    return suma;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Użycie: %s <m>\n", argv[0]);
        return 1;
    }

    int m = atoi(argv[1]);
    if (m <= 0) {
        printf("m musi być dodatnie\n");
        return 1;
    }

    // ---- Alokacja tablicy ----
    int *tab = malloc(sizeof(int) * m);
    srand(time(NULL));
    for (int i = 0; i < m; i++)
        tab[i] = rand() % 10;

    double best_time = 1e100;
    int best_n = 0, best_k = 0, best_t = 0;

    int max_threads = 32;  // możesz zmienić

    printf("Szukam najlepszych parametrów dla m=%d...\n", m);

    // ---- Szukanie wszystkich możliwych n i k (n*k = m) ----
    for (int n = 1; n <= sqrt(m); n++) {
        if (m % n != 0) continue;
        int k = m / n;

        for (int t = 1; t <= max_threads; t++) {

            double start = czas();
            ll suma = suma_watkowa(tab, m, t);
            double end = czas();

            double duration = end - start;

            // zapamiętaj najlepsze
            if (duration < best_time) {
                best_time = duration;
                best_n = n;
                best_k = k;
                best_t = t;
            }
        }
    }

    printf("\n=== NAJLEPSZA KONFIGURACJA ===\n");
    printf("n = %d\n", best_n);
    printf("k = %d\n", best_k);
    printf("t = %d\n", best_t);
    printf("Najkrótszy czas: %f mikrosekund\n", best_time);

    free(tab);
    return 0;
}
