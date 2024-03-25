#include <boost/asio.hpp>
#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>
#include <memory>
using namespace std;
using namespace std::chrono_literals;
namespace io = boost::asio;

mutex cout_mutex;

int main()
{
    io::io_service io_service;
    auto work = make_unique<io::io_service::work>(io_service);

    {
        lock_guard guard{cout_mutex};
        cout << "Starting main thread\n";
    }

    thread worker([&io_service]
    {
        {
            lock_guard guard{cout_mutex};
            cout << "worker thread start\n";
        }

        io_service.run();

        lock_guard guard{cout_mutex};
        cout << "worker thread finish\n";
    });

    io_service.post([&io_service]
    {
        for (int i = 0; i < 5; ++i)
        {
            io_service.dispatch([&io_service, i]
            { 
                lock_guard lock{cout_mutex}; 
                cout << "dispatch " << i << '\n'; 
            });

            io_service.post([&io_service, i]
            {
                lock_guard lock{cout_mutex};
                cout << "post " << i << '\n';
            });

            this_thread::sleep_for(1s);
        }
    });

    work.reset();
    worker.join();

    return 0;
}