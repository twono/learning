#include <boost/asio.hpp>
#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>
#include <memory>
using namespace std;
using namespace std::chrono_literals;
namespace io = boost::asio;

int main()
{
    cout << "Starting main thread " << this_thread::get_id() << '\n';

    io::io_service io_service;

    using boost::system::error_code;

    io::ip::tcp::socket socket{io_service};
    io::ip::tcp::endpoint endpoint{io::ip::tcp::v4(), 44444};
    io::ip::tcp::acceptor acceptor{io_service, endpoint};
    acceptor.listen();

    auto write_handler = [&socket](const error_code& error, size_t)
    {
        if (!error)
        {
            socket.shutdown(io::ip::tcp::socket::shutdown_both);
        }
        else
        {
            cout << error;
        }
    };

    auto accept_handler = [&socket, &write_handler](const error_code& error)
    {
        if (!error)
        {
            cout << "thread " << this_thread::get_id() << ": accepted\n";
            string message{">>>> server accepted you <<<<\n"};
            io::async_write(socket, io::buffer(message), write_handler);
        }
        else
        {
            cout << error;
        }
    };

    acceptor.async_accept(socket, accept_handler);
    io_service.run();

    return 0;
}