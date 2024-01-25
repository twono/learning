#include <coroutine>
#include <iostream>
using namespace std;

struct ReturnObject
{
    struct promise_type
    {
        ReturnObject get_return_object()
        {
            return { .handle_ = coroutine_handle<promise_type>::from_promise(*this) };
        }

        std::suspend_never initial_suspend() { return {}; }
        std::suspend_never final_suspend() noexcept { return {}; }
        void unhandled_exception() {}

        int value_;
    };

    coroutine_handle<promise_type> get_handle() const { return handle_; }
    coroutine_handle<promise_type> handle_;
};

template <typename promise_t>
struct Awaiter
{
    bool await_ready() 
    { 
        cout << "await_ready\n";
        return false; 
    }

    bool await_suspend(coroutine_handle<promise_t> handle)
    {
        cout << "await_suspend\n";
        promise_ = &handle.promise();
        return false;
    }

    promise_t* await_resume() 
    {
        cout << "await_resume\n"; 
        return promise_; 
    } 

    promise_t* promise_;  
};

ReturnObject count()
{
    Awaiter<ReturnObject::promise_type> awaiter;
    auto promise{co_await awaiter};
    cout << "created promise\n";
    
    for (int i = 0; true; ++i)
    {
        promise->value_ = i;
        cout << "assigned new value to promise->value_\n";
        co_await std::suspend_always{};

        // calling handle() in main comes back here
    }
}

int main()
{
    cout << "beginning\n";

    auto handle{count().get_handle()};
    cout << "after calling count()\n";

    auto& promise = handle.promise();
    cout << "got promise\n";
    for (int i = 0; i < 5; ++i)
    {
        cout << "promise value: " << promise.value_ << "\n";
        handle();
    }
    handle.destroy();
    
    return 0;
}
