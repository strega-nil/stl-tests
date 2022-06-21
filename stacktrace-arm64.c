#include <Windows.h>
#include <stddef.h>
#include <stdlib.h>

struct Backtrace
{
    void **array;
    size_t size;
};

struct Backtrace s1(void)
{
    struct Backtrace res;

    res.array = calloc(256, sizeof *res.array);
    res.size = CaptureStackBackTrace(0, 256, res.array, 0);

    return res;
}

struct Backtrace s2()
{
    return s1();
}

struct Backtrace s3()
{
    return s2();
}
