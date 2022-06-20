#include <stacktrace>
#include <iostream>

void print_stacktrace()
{
    auto s = std::stacktrace::current();
    if (s.empty())
    {
        std::cout << "failed to initialize\n";
    }
    else
    {
        std::cout << *s.rbegin() << '\n';
    }
}

#pragma optimize("", off) // inhibit TCO
void stackin(unsigned current_frame)
{
    if (current_frame == 0x10000)
    {
        print_stacktrace();
    }
    else
    {
        stackin(current_frame + 1);
    }
}
#pragma optimize("", on)

int main()
{
    stackin(0);
}
