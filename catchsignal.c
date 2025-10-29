#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

void handle_signal(int sig, siginfo_t *info, void *ucontext) {
    printf("Odebrano sygnał: %d (%s)\n", sig, strsignal(sig));
    if (info != NULL) {
        printf("  PID nadawcy: %d\n", info->si_pid);
        printf("  UID nadawcy: %d\n", info->si_uid);
        printf("  Kod sygnału: %d\n", info->si_code);
    }
}

int main() {
    struct sigaction sa;
    
    sa.sa_sigaction = handle_signal;
    sa.sa_flags = SA_SIGINFO;  // potrzebne, aby korzystać z siginfo_t
    sigemptyset(&sa.sa_mask);  // maska sygnałów
    
    // Instalacja handlera dla podstawowych sygnałów do testów
    for (int i = 1; i < 32; i++) {
        sigaction(i, &sa, NULL);
    }

    printf("PID: %d\n", getpid());
    printf("Oczekiwanie na sygnały...\n");

    while(1) {
        pause();  // czekaj na sygnał
    }

    return 0;
}
