#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>






class Server
{
public:
    explicit Server(int pNum);
    virtual ~Server();
protected:
    int Server_pNum;
    int server_fd;
    struct sockaddr_in server_addr;
};

class ServerSocket : public Server
{
public:
    explicit ServerSocket(int pNum);
    ~ServerSocket();
private:
    int client_socket;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    void run();
    void handleRequest();
    void handle_request_body(std::string& request);

    void static executeCommand(const std::string& command);

    void static openWebPage(const std::string& url);
    void static openVSCode(const std::string& arg);
    void static openCalculator(const std::string& arg);
    void static restartPC(const std::string& arg);
    void static shutDownPC(const std::string& arg);
    void static sleepPC(const std::string& arg);
    void static toggleWiFi(const std::string& arg);
    void static sendNotification(const std::string& arg);
    void static openTerminal(const std::string& arg);
    void static changeVolume(const std::string& arg);
};