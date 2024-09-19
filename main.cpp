#include "server_class.hpp"


int main()
{
    try {
        ServerSocket server(8080);
    } catch (const std::exception& e) {
        std::cerr << "Exception in main: " << e.what() << std::endl;
    }
    return 0;
}