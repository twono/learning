#include <cstring>
#include <string>
#include <array>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
using namespace std;

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        cerr << "usage: show_ip hostname\n";
        return 1;
    }

    addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    addrinfo* res;
    if (int status = getaddrinfo(argv[1], nullptr, &hints, &res); 
        status != 0)
    {
        cerr << "getaddrinfo: " << gai_strerror(status) << '\n';
        return 1;
    }

    array<char, INET6_ADDRSTRLEN> ipstr;
    for (auto* i = res; i != nullptr; i = i->ai_next)
    {
        void* addr;
        string version;

        if (i->ai_family == AF_INET)
        {
            auto* ipv4 = reinterpret_cast<sockaddr_in*>(i->ai_addr);
            addr = &(ipv4->sin_addr);
            version = "IPv4";
        }
        else
        {
            auto* ipv6 = reinterpret_cast<sockaddr_in6*>(i->ai_addr);
            addr = &(ipv6->sin6_addr);
            version = "IPv6";
        }

        inet_ntop(i->ai_family, addr, ipstr.data(), ipstr.size());
        cout << version << ": " << ipstr.data() << '\n';
    }

    freeaddrinfo(res);

    return 0;
}