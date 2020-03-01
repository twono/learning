#include <iostream>
#include <array>
#include <string>
using namespace std;

class X
{
public:
    X() { cout << "constructor\n"; }
    ~X() { cout << c; }

    void print() { cout << a << " " << c << endl; }

    int getA() const { return a; }
    string getC() const { return c; }

private:
    int a = -13;
    string c = "hello";
};

template<int N> auto get(const X& x)
{
    if constexpr (N == 0) return x.getA();
    else return x.getC();
}

namespace std
{
    template<> struct tuple_size<X> { static constexpr int value = 2; };
    template<> struct tuple_element<0, X>{ using type = int; };
    template<> struct tuple_element<1, X>{ using type = std::string; };
}

int main()
{
    auto [a, b] = X();
    b = "not good my friend";
}
