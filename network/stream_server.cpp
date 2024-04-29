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

const string port = "44444";
const int max_waiting_connections = 10;

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
    int listening_descriptor{-1};
    bool bind_successful{false};
    auto* service_info = get_service_info();

    for (auto* info = service_info; info != nullptr; info = info->ai_next)
    {
        if (listening_descriptor = socket(info->ai_family, info->ai_socktype, info->ai_protocol);
            listening_descriptor == -1)
        {
            cerr << "socket() failed: family[" << info->ai_family 
                 << "] type[" << info->ai_socktype
                 << "] protocol[" << info->ai_protocol << "]\n";
            continue;
        }

        const int yes{1};
        if (setsockopt(listening_descriptor, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
        {
            cerr << "setsockopt() to reuse port failed\n";
            return 1;
        }

        if (bind(listening_descriptor, info->ai_addr, info->ai_addrlen) == 1)
        {
            close(listening_descriptor);
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

    if (listen(listening_descriptor, max_waiting_connections) == -1)
    {
        cerr << "listen() failed for descriptor " << listening_descriptor << '\n';
        return 1;
    }

    handle_sigchld();

    cout << "server: waiting for connections...\n";

    while (true)
    {
        sockaddr_storage client_addr;
        socklen_t client_addr_size{sizeof(client_addr)};
        int connection_descriptor = accept(listening_descriptor, 
                                           reinterpret_cast<sockaddr*>(&client_addr), 
                                           &client_addr_size);
        if (connection_descriptor == -1)
        {
            cerr << "accept() failed\n";
            continue;
        }

        print_address(reinterpret_cast<sockaddr*>(&client_addr), "got connection from ");

        if (fork() == 0)
        {
            close(listening_descriptor);    // child process doesn't use this

            const string msg = "server sends its regards!\n";
            if (send(connection_descriptor, msg.c_str(), msg.size(), 0) == -1)
            {
                cerr << "send() to descriptor " << connection_descriptor << "failed\n";
            }

            close(connection_descriptor);
            return 0;    // child process handles just one connection and then exits
        }

        close(connection_descriptor);   // parent process doesn't need this
    }

    return 0;
}