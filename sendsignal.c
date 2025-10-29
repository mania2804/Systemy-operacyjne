#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>
#include <ctype.h>

int is_number(const char *s) {
    while (*s) {
        if (!isdigit(*s)) return 0;
        s++;
    }
    return 1;
}

void send_signal_to_pid(pid_t pid, int sig) {
    if (kill(pid, sig) == -1) {
        perror("kill");
    } else {
        printf("Sygnał %d wysłany do PID %d\n", sig, pid);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Użycie: %s <process> <signal>\n", argv[0]);
        return 1;
    }

    char *process = argv[1];
    char *signal_str = argv[2];
    int sig;

    // Konwersja sygnału: numer lub nazwa (np. SIGINT)
    if (is_number(signal_str)) {
        sig = strtol(signal_str, NULL, 10);
    } else if (strncmp(signal_str, "SIG", 3) == 0) {
        // uproszczona obsługa tylko kilku sygnałów
        if (strcmp(signal_str, "SIGINT") == 0) sig = SIGINT;
        else if (strcmp(signal_str, "SIGTERM") == 0) sig = SIGTERM;
        else if (strcmp(signal_str, "SIGKILL") == 0) sig = SIGKILL;
        else {
            fprintf(stderr, "Nieznany sygnał: %s\n", signal_str);
            return 1;
        }
    } else {
        fprintf(stderr, "Nieprawidłowy sygnał: %s\n", signal_str);
        return 1;
    }

    // Jeśli argument process jest numerem
    if (is_number(process)) {
        pid_t pid = strtol(process, NULL, 10);
        send_signal_to_pid(pid, sig);
    } else {
        // Szukamy PID po nazwie procesu
        DIR *dir = opendir("/proc");
        if (!dir) {
            perror("opendir");
            return 1;
        }
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {
            if (!is_number(entry->d_name)) continue;
            pid_t pid = strtol(entry->d_name, NULL, 10);

            char path[256];
            snprintf(path, sizeof(path), "/proc/%d/comm", pid);
            FILE *f = fopen(path, "r");
            if (!f) continue;
            char name[256];
            if (fgets(name, sizeof(name), f) != NULL) {
                name[strcspn(name, "\n")] = 0; // usuń \n
                if (strcmp(name, process) == 0) {
                    send_signal_to_pid(pid, sig);
                }
            }
            fclose(f);
        }
        closedir(dir);
    }

    return 0;
}
