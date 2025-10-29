                        
#include <stdio.h>
#include <unistd.h>

int main() {
    printf("Hello, I’m another program! My PID = %d\n", getpid());
    printf("Press Enter to exit...\n");
    getchar();  // czeka na wciśnięcie Enter
    return 0;
}


