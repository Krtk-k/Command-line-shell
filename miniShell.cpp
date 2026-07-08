#include <iostream>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string>
#include <vector>
#include <sstream>
#include <limits.h>

int main() {
    std::cout << "Welcome to custom shell\n";
    std::string line;
    while(1) {
        char buffer[PATH_MAX];
        std::cout << "\n" << getcwd(buffer, sizeof(buffer)) << "<<$ ";
        if(!std::getline(std::cin, line)) {
            std::cout << "Exiting";
            break;
        }
        if(line.empty()) continue;
        else if(line == "end") break;
        std::istringstream ss(line);
        std::string token;
        std::vector<std::string> tokens;

        while(ss >> token)  {
            if(!token.empty()) tokens.push_back(token);
        }
        
        std::vector<std::vector<char*>> args;
        std::vector<char*> arg;
        for(auto &s : tokens) {
            if(s == "|") {
                arg.push_back(nullptr);
                args.push_back(arg);
            }
            else
                arg.push_back(const_cast<char*>(s.c_str()));
        }

        int row = args.size();

        int prev_fd = STDIN_FILENO;

        for(int i = 0; i<row; i++) {
            int p[2];

            if(i < row - 1) {
                if(pipe(p) < 0) {
                    std::cerr << "Pipe creation failed\n";
                    break;
                }
            }

            pid_t pid = fork();

            if(pid<0) {
                std::cerr << "\nnew process creation failed";
            }
            else if(pid == 0) {
                if (prev_fd != STDIN_FILENO) {
                    dup2(prev_fd, STDIN_FILENO);
                    close(prev_fd);
                }
                if (i < row - 1) {
                    dup2(p[1], STDOUT_FILENO);
                    close(p[1]);
                    close(p[0]);
                }

                execvp(args[i][0], args[i].data());
                exit(1);
            }
            else {
                if(prev_fd != STDIN_FILENO) {
                    close(prev_fd);
                }
                if(i < row - 1) {
                    close(p[1]); 
                    prev_fd = p[0];
                }
            }
        }
        for (int i = 0; i < row; i++) {
            wait(NULL);
        }
    }
    return 0;
}
