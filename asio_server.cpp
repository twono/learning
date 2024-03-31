#include <boost/asio.hpp>
#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>
#include <memory>
using namespace std;
using namespace boost;
using namespace std::chrono_literals;

class server
{
public:
    server(asio::io_context& io_context) 
        : acceptor_{io_context, asio::ip::tcp::endpoint{asio::ip::tcp::v4(), 44444}}
    {
        start_accept();
    }

private:
    void start_accept()
    {
        acceptor_.async_accept([this](const system::error_code& error, asio::ip::tcp::socket socket)
        {
            if (!error.failed())
            {
                cout << "thread " << this_thread::get_id() << ": accepted\n";
                string message{">>>> server accepted you <<<<\n"};
                asio::async_write(socket, asio::buffer(message), 
                                  [](const system::error_code& error, size_t)
                {
                    if (error.failed())
                    {
                        cout << error;
                    }
                });

                start_accept();
            }
            else
            {
                cout << error;
            }
        });
    }

    asio::ip::tcp::acceptor acceptor_;
};

int main()
{
    cout << "Starting main thread " << this_thread::get_id() << '\n';

    asio::io_context io_context;
    server s{io_context};
    
    io_context.run();

    return 0;
}