#include <future>
#include <thread>

struct A {};

int main()
{
    std::promise<const A> p;
#if 0
    std::thread thread([](std::promise<const A> a) {
        a.set_value(std::move(A{}));
    }, std::move(p));
    thread.join();
#endif
    return 0;
}
