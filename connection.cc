#include <cstdio>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <string>
#include <stdexcept>

using namespace std;

class ProcessConnector {
    private:
    public:
        ProcessConnector(){
        }
        ~ProcessConnector(){
        }
        int _connect(string ip, short port){
            int fd = socket(AF_INET, SOCK_STREAM, 0);
            struct sockaddr_in servaddr;
            servaddr.sin_family = AF_INET;
            servaddr.sin_addr.s_addr = inet_addr(ip.c_str());
            servaddr.sin_port = htons(port);
            int rc = connect(fd, (struct sockaddr*)&servaddr, sizeof(servaddr));
            if(rc < 0){ throw runtime_error("connection error"); }
            return fd;
        }
};
