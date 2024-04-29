#include <iostream>
#include <cstring>
#include <string>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
using namespace std;

const string port = "44444";
const int max_data_size = 100;

void* get_ip_addr(sockaddr* address)
{
    return address->sa_family == AF_INET6 
        ? reinterpret_cast<void*>(&(reinterpret_cast<sockaddr_in6*>(address)->sin6_addr))
        : reinterpret_cast<void*>(&(reinterpret_cast<sockaddr_in*>(address)->sin_addr));
}

void print_address(sockaddr* address, const string& msg)
{
    string s(INET6_ADDRSTRLEN, '\0');
    inet_ntop(address->sa_family, get_ip_addr(address), s.data(), s.size());
    cout << msg << s << '\n';
}

addrinfo* get_service_info()
{
    addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    addrinfo* service_info;
    if (int status = getaddrinfo(nullptr, port.c_str(), &hints, &service_info);
        status != 0)
    {
        cerr << "getaddrinfo: " << gai_strerror(status) << '\n';
        exit(1);
    }

    return service_info;
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        cerr << "usage: stream_client <server_hostname>\n";
        return 1;
    }
    
    int sending_descriptor{-1};
    bool connect_successful{false};
    auto* service_info = get_service_info();

    for (auto* info = service_info; info != nullptr; info = info->ai_next)
    {
        if (sending_descriptor = socket(info->ai_family, info->ai_socktype, info->ai_protocol);
            sending_descriptor == -1)
        {
            cerr << "socket() failed: family[" << info->ai_family 
                 << "] type[" << info->ai_socktype
                 << "] protocol[" << info->ai_protocol << "]\n";
            continue;
        }

        if (connect(sending_descriptor, info->ai_addr, info->ai_addrlen) == -1)
        {
            close(sending_descriptor);
            cerr << "connect(" << sending_descriptor << ") failed\n";
            continue;
        }

        connect_successful = true;
        print_address(info->ai_addr, "connected to ");
        break;
    }

    freeaddrinfo(service_info);

    if (!connect_successful)
    {
        cerr << "connect() never succeeded\n";
        return 1;
    }

    string msg(max_data_size, '\0');
    if (recv(sending_descriptor, msg.data(), msg.size(), 0) == -1)
    {
        cerr << "recv(" << sending_descriptor << ") failed\n";
        return 1;
    }

    cout << "message from server: " << msg << '\n';
    close(sending_descriptor);

    return 0;
}
