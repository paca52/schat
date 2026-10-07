#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>

int main(void) {
    int sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd == -1) {
        printf("Failed.\n");
        return 0;
    }

    close(sd);
    return 0;
}
