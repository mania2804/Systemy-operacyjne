                          
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
    printf("Hello, I’m system program! My PID = %d\n", getpid());

    // Uruchamiamy program another jako nowy proces
    system("./another");

    printf("Hello, I’m again system program! My PID = %d\n", getpid());

    return 0;
}




