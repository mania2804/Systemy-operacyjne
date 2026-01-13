#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/sysinfo.h>

#define BLOCK_SIZE (100 * 1024 * 1024) // 100 MB na blok
#define SWAP_LIMIT (100 * 1024 * 1024) // 100 MB limit swapu

typedef struct Node {   // struktura ze wskażnikiem do pamięci i następnego elementu
    char *data;
    struct Node *next;
} Node;

void print_memory_info() { 
    struct sysinfo info;    // deklaracja struktury info
    sysinfo(&info); // pobiera informacje systemowe

    long total_ram = info.totalram * info.mem_unit;
    long free_ram = info.freeram * info.mem_unit;
    long used_ram = total_ram - free_ram;
    long total_swap = info.totalswap * info.mem_unit;
    long free_swap = info.freeswap * info.mem_unit;
    long used_swap = total_swap - free_swap;

    printf("RAM: total=%.2f MB, used=%.2f MB (%.1f%%), free=%.2f MB\n", // wypisanie informacji o RAM
           total_ram / (1024.0 * 1024.0),
           used_ram / (1024.0 * 1024.0),
           (double)used_ram / total_ram * 100.0,
           free_ram / (1024.0 * 1024.0));
    printf("Swap: total=%.2f MB, used=%.2f MB (%.1f%%), free=%.2f MB\n", // wypisanie inforacji o SWAP
           total_swap / (1024.0 * 1024.0),
           used_swap / (1024.0 * 1024.0),
           total_swap > 0 ? (double)used_swap / total_swap * 100.0 : 0.0,
           free_swap / (1024.0 * 1024.0));
    printf("---\n");
}

int main() {
    Node *head = NULL, *tail = NULL;
    int count = 0;
    int swap_exceeded = 0;
    int swap_started = 0; 
    struct sysinfo info;

    printf("Rozpoczynam alokację pamięci...\n");
    print_memory_info();

    while (1) {
        sysinfo(&info);
        long used_swap = (info.totalswap - info.freeswap) * info.mem_unit; //obliczamy uzyty SWAP
        long used_ram = (info.totalram - info.freeram) * info.mem_unit; //obliczamy ozyty RAM

        if (!swap_started && used_swap > 0) {
            printf("[INFO] Rozpoczęto stronicowanie do SWAP!\n");
            swap_started = 1;
        }

        if (!swap_exceeded) { //maksymalny SWAP
            
            if (used_swap > SWAP_LIMIT) { //sprawdzenie czy limit SWAP został przekroczony
                printf("[INFO] Przekroczono 100 MB w SWAP. Zaczynam zwalniać pamięć...\n");
                swap_exceeded = 1;
                continue;
            }
            Node *new_node = (Node*)malloc(sizeof(Node)); //stworzenie nowego węzła
            if (!new_node) {
                perror("Błąd malloc (node)");
                break;
            }
            new_node->data = (char*)malloc(BLOCK_SIZE); //alokacja bloku dla stworzonego węzła
            if (!new_node->data) {
                perror("Błąd malloc (data)");
                free(new_node);
                break;
            }
            memset(new_node->data, 0xAA, BLOCK_SIZE); // zapisujemy dane - wypełniamy bloki, żeby system nie wyczyścił ich
            new_node->next = NULL;

            if (!head) head = tail = new_node; //stworzenie głowy jesli lista jest pusta
            else {
                tail->next = new_node; //dodanie węzła na koniec listy
                tail = new_node;
            }
            count++;
            printf("Dodano blok %d (%.1f MB)\n", count, BLOCK_SIZE / (1024.0 * 1024.0));
            print_memory_info(); // funkcja pokazująca stan pamięci

            
        } else { //zwalnianie
            if (!head) {
                printf("Lista pusta. Kończenie.\n"); //jeśli lista pusta to koniec programu
                break;
            }
            Node *to_free = head; //element do usunięcia
            head = head->next; //przesunięcie głowy na następny element
            free(to_free->data); // zwolnienie danych
            free(to_free); //zwonienie węzła
            count--;
            printf("Usunięto blok. Pozostało: %d\n", count);
            print_memory_info(); // informacja o pamięci po kazdym usunięciu

        }

        sleep(1); // opóźnienie dla czytelności monitorów
    }

    printf("Program zakończony.\n");
    return 0;
}