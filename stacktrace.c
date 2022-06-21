#include <Windows.h>
#include <stddef.h>
#include <stdio.h>
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

int wmain()
{
    struct Backtrace bt = s3();

    wprintf(L"size of backtrace: %zu\n", bt.size);
    wprintf(L"backtrace:\n");
    for (size_t i = 0; i < bt.size; ++i)
    {
        wprintf(L" > 0x%zX\n", (uintptr_t)bt.array[i]);
    }

    return 0;
}
