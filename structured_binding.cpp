#include <iostream>
#include <array>
#include <string>
#include <tuple>
#include <vector>
using namespace std;

class X
{
public:
    X() { cout << "constructor\n"; }
    ~X() { cout << c; }

    void print() { cout << a << " " << c << endl; }

    template<int N> auto& get()
    {
        if constexpr (N == 0) return a;
        else return c;
    }

private:
    int a = -13;
    string c = "hello";
};

namespace std
{
    template<> struct tuple_size<X> { static const int value = 2; };
    template<> struct tuple_element<0, X>{ using type = int; };
    template<> struct tuple_element<1, X>{ using type = std::string; };
}

int main()
{
    auto [a, b] = X();
    b = "not good my friend";
}
