#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main()
{
    int sock = socket(AF_INET, SOCK_DGRAM, 0);

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(4242);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    const char* message = "Hello";
    sendto(sock, message, strlen(message), 0,
           (sockaddr*)&server_addr, sizeof(server_addr));

    char buffer[1024];
    sockaddr_in from;
    socklen_t from_len = sizeof(from);
    int bytes = recvfrom(sock, buffer, sizeof(buffer), 0,
                         (sockaddr*)&from, &from_len);

    if (bytes > 0) {
        buffer[bytes] = '\0';
        std::cout << "Received: " << buffer << std::endl;
    }

    close(sock);
    return 0;
}
