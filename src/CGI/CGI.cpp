#include "socket.hpp"

CGI::CGI()
{
    this->req = NULL;
}

CGI::CGI(std::string bodyPath, std::string CGI_path, std::string uri): bodyPath(bodyPath), CGI_path(CGI_path), uri(uri)
{
   stat(this->bodyPath.c_str(), &this->bodyStat);

}
CGI::~CGI()
{
}
 
void CGI::setEnvironmentVariables()
{
    this->env["AUTH_TYPE"] = "";
    this->env["CONTENT_LENGTH"] = this->headers["Content-Length"];
    this->env["CONTENT_TYPE"] = this->headers["Content-Type"];
    this->env["GATEWAY_INTERFACE"] = "CGI/1.1";
    this->env["PATH_INFO"] = this->uri;
    this->env["PATH_TRANSLATED"] = this->uri;
    this->env["QUERY_STRING"] = this->uri;
    this->env["REMOTE_ADDR"] = "";
    this->env["REMOTE_IDENT"] = "";
    this->env["REMOTE_USER"] = "";
    // this->env["REQUEST_METHOD"] = this->req->getMethod();
    this->env["REQUEST_URI"] = this->uri;
    this->env["SCRIPT_NAME"] = this->CGI_path;
    this->env["SERVER_NAME"] = "1337.ma";
    this->env["SERVER_PORT"] = "8080";
    this->env["SERVER_PROTOCOL"] = "HTTP/1.1";
    this->env["SERVER_SOFTWARE"] = "";
    this->env["REDIRECT_STATUS"] = "200";
    this->env["SCRIPT_FILENAME"] = this->CGI_path;
    this->env["SERVER_ADMIN"] = "";
}

char** CGI::getEnv()
{
    char **env = new char*[this->env.size() + 1];
    int i = 0;
    for (std::map<std::string, std::string>::iterator it = this->env.begin(); it != this->env.end(); it++)
    {
        std::string tmp = it->first + "=" + it->second;
        env[i] = new char[tmp.size() + 1];
        strcpy(env[i], tmp.c_str());
        i++;
    }
    env[i] = NULL;
    return env;
}

int CGI::ExecuteCGI()
{
    // int pipeIn[2];
    // int pipeOut[2];
    // pid_t pid;
    // char **env = this->getEnv();
    // char *argv[2];
    // argv[0] = new char[this->CGI_path.size() + 1];
    // strcpy(argv[0], this->CGI_path.c_str());
    // argv[1] = NULL;
    // if (pipe(pipeIn) == -1)
    // {
    //     std::cerr << "pipeIn failed" << std::endl;
    //     return;
    // }
    // if (pipe(pipeOut) == -1)
    // {
    //     std::cerr << "pipeOut failed" << std::endl;
    //     return;
    // }
    // pid = fork();
    // if (pid == -1)
    // {
    //     std::cerr << "fork failed" << std::endl;
    //     return;
    // }
    // if (pid == 0)
    // {
    //     close(pipeIn[1]);
    //     close(pipeOut[0]);
    //     dup2(pipeIn[0], 0);
    //     dup2(pipeOut[1], 1);
    //     close(pipeIn[0]);
    //     close(pipeOut[1]);
    //     execve(this->CGI_path.c_str(), argv, env);
    //     std::cerr << "execve failed" << std::endl;
    //     exit(1);
    // }
    // else
    // {
    //     close(pipeIn[0]);
    //     close(pipeOut[1]);
    //     write(pipeIn[1], this->body.c_str(), this->body.size());
    //     close(pipeIn[1]);
    //     char buffer[4096];
    //     int bytesRead;
    //     while ((bytesRead = read(pipeOut[0], buffer, 4096)) > 0)
    //     {
    //         this->bufferToSend.append(buffer, bytesRead);
    //     }
    //     close(pipeOut[0]);
    // }
    // waitpid(pid, NULL, 0);
    // for (int i = 0; env[i]; i++)
    // {
    //     delete[] env[i];
    // }
    // delete[] env;
    // delete[] argv[0];
}
