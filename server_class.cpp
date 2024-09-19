#include "server_class.hpp"
#include "supportive_classes.hpp"
#include <string>

Server::Server(int pNum):Server_pNum(pNum)
{
    // std::cout << "Server Init" << std::endl;

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        throw std::runtime_error("Socket creation failed");
    }

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        close(server_fd);
        throw std::runtime_error("Setting socket options failed");
    }
    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(pNum);

    // Bind socket
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        close(server_fd);
        throw std::runtime_error("Socket binding failed");
    }

    // Listen for connections
    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        close(server_fd);
        throw std::runtime_error("Socket listening failed");
    }
    std::cout << "Server listening on port " << Server_pNum << std::endl;
}

Server::~Server()
{
    // std::cout << "Server DeInit" << std::endl;
    close(server_fd);
}

ServerSocket::ServerSocket(int pNum) : Server(pNum)
{
    try {
        run();
    } catch (const std::exception& e) {
        std::cerr << "Exception in ServerSocket: " << e.what() << std::endl;
    }
}

ServerSocket::~ServerSocket()
{
    // std::cout << "Socket DeInit" << std::endl;
    close(client_socket);
}

void ServerSocket::run()
{
    while (true)
    {
        // std::cout << "Socket Init" << std::endl;
        client_socket = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket < 0) {
            perror("accept failed");
            close(server_fd);
            throw std::runtime_error("Socket accept failed");
        }

        handleRequest();
        // std::cout << "Socket DeInit " << client_socket << std::endl;
        close(client_socket);
    }
}

void ServerSocket::handleRequest()
{
    const size_t buffer_size = 1024;
    char buffer[buffer_size] = {0};
    std::string request;

    // Read data from client
    ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0'; // Null-terminate the string
        request.append(buffer, bytes_received);
    } else {
        std::cerr << "Failed to receive data" << std::endl;
        return;
    }

    // Find the end of the headers
    std::string::size_type header_end = request.find("\r\n\r\n");
    if (header_end != std::string::npos) {
        // Extract the body
        std::string body = request.substr(header_end + 4); // +4 to skip "\r\n\r\n"
        // std::cout << "Request Body: " << body << std::endl;
        try {
            handle_request_body(body);
        } catch (const std::exception& e) {
            throw;
        }
    } else {
        std::cerr << "Invalid HTTP request format: Headers end not found." << std::endl;
    }
}

void ServerSocket::handle_request_body(std::string& request_st)
{
    int request = std::stoi(request_st);
    std::cout << "Handle request number " << request << std::endl;
    switch (request)
    {
    case 0: //Shut Down
        throw ShutdownException(); 
    case 1: //Open Facebook
    {
        startThread thread(ServerSocket::openWebPage,"https://www.facebook.com");
        break;
    }
    case 2: //Open VS Code
    {
        startThread thread(ServerSocket::openVSCode,"");
        break;
    }
    case 3: //Open Calculator
    {
        startThread thread(ServerSocket::openCalculator,"");
        break;
    }
    case 4: //Open My GitHub
    {
        startThread thread(ServerSocket::openWebPage,"https://github.com/Omar-Talaat11");
        break;
    }
    case 5: //Open Youtube
    {
        startThread thread(ServerSocket::openWebPage,"https://www.youtube.com/");
        break;
    }
    case 6: //Open Chat GPT
    {
        startThread thread(ServerSocket::openWebPage,"https://chatgpt.com/");
        break;
    }
    case 7: //Restart PC
    {
        startThread thread(ServerSocket::restartPC,"");
        break;
    }
    case 8: //Shut Down PC
    {
        startThread thread(ServerSocket::shutDownPC,"");
        break;
    }
    case 9: //Sleep PC
    {
        startThread thread(ServerSocket::sleepPC,"");
        break;
    }
    case 10: //Open My LinkedIn
    {
        startThread thread(ServerSocket::openWebPage,"https://www.linkedin.com/in/omar-talaat-522468218/");
        break;
    }
    case 11: //Toggle Wifi
    {
        startThread thread(ServerSocket::toggleWiFi,"");
        break;
    }
    case 12: //Send Notification
    {
        startThread thread(ServerSocket::sendNotification,"Notification Message");
        break;
    }
    case 13: //Open Terminal
    {
        startThread thread(ServerSocket::openTerminal,"");
        break;
    }
    case 14: //Increase Volume
    {
        startThread thread(ServerSocket::changeVolume,"i");
        break;
    }
    case 15: //Decrease Volume
    {
        startThread thread(ServerSocket::changeVolume,"d");
        break;
    }
    case 16: //Mute Volume
    {
        startThread thread(ServerSocket::changeVolume,"m");
        break;
    }
    default:
        std::cerr << "Invalid request number" << std::endl;
        break;
    }
    std::cout << "Server listening on port " << Server_pNum << std::endl;
}

void ServerSocket::openWebPage(const std::string& url)
{
    std::string command = "xdg-open " + url;
    executeCommand(command);
}

void ServerSocket::openVSCode(const std::string& arg)
{
    executeCommand("code");
}

void ServerSocket::openCalculator(const std::string& arg)
{
    executeCommand("gnome-calculator");
}

void ServerSocket::restartPC(const std::string& arg)
{
    executeCommand("sudo reboot");
}

void ServerSocket::shutDownPC(const std::string& arg)
{
    executeCommand("sudo shutdown now");
}

void ServerSocket::sleepPC(const std::string& arg)
{
    executeCommand("systemctl suspend");
}

void ServerSocket::toggleWiFi(const std::string& arg) 
{
    // Check current status
    std::string command = "nmcli radio wifi";
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) return;

    char buffer[128];
    std::string result = "";
    
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }
    pclose(pipe);
    
    // Toggle based on current status
    if (result.find("enabled") != std::string::npos) {
        executeCommand("nmcli radio wifi off");
    } else {
        executeCommand("nmcli radio wifi on");
    }
}

void ServerSocket::sendNotification(const std::string& arg) {
    std::string command = "notify-send \"" + arg + "\"";
    executeCommand(command);
}

void ServerSocket::openTerminal(const std::string& arg) 
{
    executeCommand("gnome-terminal"); // Use & to run it in the background
}

void ServerSocket::changeVolume(const std::string& arg) 
{
    if (arg == "i")     
    {
        executeCommand("amixer -D pulse sset Master unmute");
        executeCommand("amixer -D pulse sset Master 10%+");
    }
    else if (arg == "d")     
    {
        executeCommand("amixer -D pulse sset Master unmute");
        executeCommand("amixer -D pulse sset Master 10%-");
    }
    else if (arg == "m")     
    {
        executeCommand("amixer -D pulse sset Master mute");
    }
}

void ServerSocket::executeCommand(const std::string& command) {
    std::string full_command = command + " > /dev/null 2>&1"; // Redirect output
    system(full_command.c_str());
}
