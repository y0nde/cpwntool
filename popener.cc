#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <string>
#include <stdexcept>

using namespace std;

#define READ 0
#define WRITE 1

class ProcessExe{
    virtual void exec() = 0;
};

struct ProcessPipe {
    pid_t pid;
    int stdIn;
    int stdOut;
}
class Popener {
    public:
        ProcessPipe openProcess(ProcessExe& exe){
            ProcessPipe pp;
            int inpipe[2];
            int outpipe[2];
            pid_t pid;
            if(pipe(inpipe) == -1){
                throw runtime_error("pipe error");
            }
            if(pipe(outpipe) == -1){
                throw runtime_error("pipe error");
            }
            pid = fork();
            if(pid == -1){
                throw runtime_error("fork error");
            }
            if(pid == 0){
                close(inpipe[WRITE]);
                close(outpipe[READ]);
                if(dup2(inpipe[READ], 0) == -1){
                    throw runtime_error("dup2 error");
                }
                if(dup2(outpipe[WRITE], 1) == -1){
                    throw runtime_error("dup2 error");
                }
                close(inpipe[READ]);
                close(outpipe[WRITE]);
                exe.exec();
            }
            pp.pid = pid;
            pp.stdIn = inpipe[WRITE];
            pp.stdOut = outpipe[READ];
            return pp;
        }

        void closeProcess(ProcessPipe& pp){
            int wstatus;
            close(pp.stdIn);
            close(pp.stdOut);
            if(waitpid(pp.pid, &wstatus, 0) < 0){
                throw runtime_error("wait error");
            }
        }
}
