#include <cstring>
#include <csignal>
#include <iostream>
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
const int max_data_size = 100;

addrinfo* get_addr_info()
{
    addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;          // use IPv4 only
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;        // use my IP

    addrinfo* service_info;
    if (int status = getaddrinfo(nullptr, port.c_str(), &hints, &service_info);
        status != 0)
    {
        cerr << "getaddrinfo: " << gai_strerror(status) << '\n';
        exit(1);
    }

    return service_info;
}

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

void handle_sigchld()
{
    struct sigaction action;
    sigemptyset(&action.sa_mask);
    action.sa_flags = SA_RESTART;
    action.sa_handler = [](int signal)
    {
        int saved_errno = errno;
        while (waitpid(-1, nullptr, WNOHANG) > 0);
        errno = saved_errno;
    };

    if (sigaction(SIGCHLD, &action, nullptr) == -1)
    {
        cerr << "sigaction(SIGCHLD) failed\n";
        exit(1);
    }
}

int main()
{
    int receiving_descriptor{-1};
    bool bind_successful{false};
    auto* service_info = get_addr_info();

    for (auto* info = service_info; info != nullptr; info = info->ai_next)
    {
        if (receiving_descriptor = socket(info->ai_family, info->ai_socktype, info->ai_protocol);
            receiving_descriptor == -1)
        {
            cerr << "socket() failed: family[" << info->ai_family 
                 << "] type[" << info->ai_socktype
                 << "] protocol[" << info->ai_protocol << "]\n";
            continue;
        }

        if (bind(receiving_descriptor, info->ai_addr, info->ai_addrlen) == 1)
        {
            close(receiving_descriptor);
            cerr << "failed to bind()\n";
            continue;
        }

        print_address(info->ai_addr, "server bound to ");
        bind_successful = true;
        break;
    }

    freeaddrinfo(service_info);

    if (!bind_successful)
    {
        cerr << "bind() never succeeded\n";
        return 1;
    }

    handle_sigchld();

    cout << "server: ready to receive packets...\n";

    string buf(max_data_size, '\0');
    sockaddr_storage client_address;
    socklen_t client_address_len{sizeof(client_address)};

    int bytes_received{-1};
    if (bytes_received = recvfrom(receiving_descriptor, buf.data(), buf.size() - 1, 0, 
                                  reinterpret_cast<sockaddr*>(&client_address), &client_address_len);
        bytes_received == -1)
    {
        print_address(reinterpret_cast<sockaddr*>(&client_address), "failed to recvfrom() ");
        return 1;
    }

    print_address(reinterpret_cast<sockaddr*>(&client_address), 
                    "received " + to_string(bytes_received) + " from ");
    cout << "received message: " << buf << '\n';

    close(receiving_descriptor);

    return 0;
}