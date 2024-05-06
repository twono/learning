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

const string port = "44445";

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

addrinfo* get_addr_info()
{
    addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;          // only IPv4
    hints.ai_socktype = SOCK_DGRAM;

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
    if (argc != 3)
    {
        cerr << "usage: datagram_client <server_hostname> <message>\n";
        return 1;
    }
    
    auto* addr_info_list = get_addr_info();

    bool successful{false};
    int sending_descriptor{-1};
    auto* server_info = addr_info_list;
    for (; server_info != nullptr; server_info = server_info->ai_next)
    {
        if (sending_descriptor = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);
            sending_descriptor == -1)
        {
            cerr << "socket() failed: family[" << server_info->ai_family 
                 << "] type[" << server_info->ai_socktype
                 << "] protocol[" << server_info->ai_protocol << "]\n";
            continue;
        }

        successful = true;
        print_address(server_info->ai_addr, "created socket to connect to ");
        break;
    }

    freeaddrinfo(addr_info_list);

    if (!successful)
    {
        cerr << "socket() never succeeded\n";
        return 1;
    }

    int bytes_sent{-1};
    string message(argv[2]);
    if (bytes_sent = sendto(sending_descriptor, message.c_str(), message.size(), 0, 
                            server_info->ai_addr, server_info->ai_addrlen);
        bytes_sent == -1)
    {
        cerr << "sendto() failed\n";
        return 1;
    }

    cout << "sent " << bytes_sent << " bytes to " << argv[1] << '\n';
    close(sending_descriptor);

    return 0;
}
