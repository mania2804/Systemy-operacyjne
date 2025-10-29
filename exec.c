                           
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
    printf("Hello, I’m exec program! My PID = %d\n", getpid());

    // Uruchamiamy another (zakładając, że został wcześniej skompilowany d>
    execl("./another", "another", NULL);

    // Jeśli exec się nie powiedzie, to wypisujemy błąd
    perror("execl failed");

    // Ta linia nigdy się nie wykona, jeśli exec się powiedzie
    printf("Hello, I’m again exec program! My PID = %d\n", getpid());

    return 0;
}

