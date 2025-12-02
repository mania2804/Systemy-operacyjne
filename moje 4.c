#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

typedef long long ll;
ll suma_watków=0;

typedef struct {
    int *tab;
    long start;
    long end;
    ll result;
}Watek;

void *sum_1_watek( void *arg){
    Watek *watek = arg;
    ll suma=0;

    for (int i=watek->start; i<watek->end; i++) {
        suma += watek->tab[i];
    }

    watek->result = suma;
    pthread_exit(NULL);
}


double czas() {   //funkcja do liczenia czasu wykonania operacji
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (double)tv.tv_sec * 1000000 + (double)tv.tv_usec; //microsekundy
}


int main(int argc, char *argv[]) {

    if (argc != 4) {
        printf("Użycie: %s <n> <t> <k>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    int k = atoi(argv[2]);
    int t = atoi(argv[3]);
    int total=n*k;
    if (n <= 0 || t <= 0 || k <= 0) {
        printf("Wartości muszą być dodatnie.\n");
        return 1;
    }
    int *tab=malloc(sizeof(int)*total);  //tablica wszystkich liczb

    srand(time(NULL));
    for(int i=0; i<total; i++){
        tab[i]=rand()%10;
    }

    double start1=czas();

    ll suma=0;
    for(int i=0; i<total; i++){
        suma+=tab[i];
    }

    // for(int i=0; i<total; i++){
    //     printf("%d ",tab[i]);
    // }

    double end1 = czas();
    printf("\nSuma: %d ", suma);
    printf("\nCzas sekwencyjny: %f ", end1-start1);

    FILE *fp = fopen("wyniki.csv", "w");
    if (!fp) {
        printf("Błąd otwarcia pliku.\n");
        return 1;
    }
    fprintf(fp, "t,czas_watkow\n");

    /*wątki*/
    long  il_tab_1_wat = total/t;
    long pozostale_wat= total%t;
    int idx=0;

    pthread_t watki[t];  // tworzymy tablice wątków
    Watek *watki_data[t]; // dane watkow

    double start2=czas();
    for (int i=0; i<t; i++) {
        watki_data[i] =malloc(sizeof(Watek));
        long new_il_tab=il_tab_1_wat+(pozostale_wat>0) ? 1 : 0;
        watki_data[i]->tab=tab;
        watki_data[i]->start=idx*n;
        watki_data[i]->end=watki_data[i]->start+new_il_tab *n;
        idx+=new_il_tab;

        pthread_create(&watki[i], NULL, sum_1_watek, &watki_data[i]);
        pozostale_wat--;
        // free(watek);
    }


    for (int i=0; i<t; i++) {
        pthread_join(watki[i], NULL);     //wątek glowny czeka na zakonczenie wszystkich watkow
    }
    double end2=czas();


    for (int i = 0; i < t; i++) {
        suma_watków += watki_data[i]->result;
        free(watki_data[i]);
    }

    printf("\n Suma: %lld", suma_watków);
    printf("\nCzas watkowy: %f ", end2-start2);



    free(tab);
    return 0;
}