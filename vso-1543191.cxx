#include <cstdio>
#include <cstring>
#include <vector>
#include <malloc.h>

#define MEMTAG1 1330464077
#define MEMTAG2 1279416658
#define MEMTAG3 558580559
#define MEMFREE 1162170950

typedef struct MemHead {
    int tag1;
    size_t len;

    int tag2;
    short pad1;
    short alignment;
} MemHead;

typedef MemHead MemHeadAligned;

typedef struct MemTail {
    int tag3, pad;
} MemTail;

#define MEMHEAD_ALIGN_PADDING(alignment) \
  ((size_t)alignment - (sizeof(MemHeadAligned) % (size_t)alignment))
#define MEMHEAD_REAL_PTR(memh) ((char *)memh - MEMHEAD_ALIGN_PADDING(memh->alignment))


void* MEM_mallocN_aligned(size_t len, size_t alignment)
{
    const size_t extra_padding = 8;
#ifdef _WIN32
    MemHead* memh = (MemHead*)_aligned_malloc(
        len + extra_padding + sizeof(MemHead) + sizeof(MemTail), alignment);
#else
    MemHead* memh = (MemHead*)memalign(
        alignment, len + extra_padding + sizeof(MemHead) + sizeof(MemTail));
#endif
    memh = (MemHead*)((char*)memh + extra_padding);

    memh->tag1 = MEMTAG1;
    memh->len = len;
    memh->pad1 = 0;
    memh->alignment = 0;
    memh->tag2 = MEMTAG2;

    MemTail* memt = (MemTail*)(((char*)memh) + sizeof(MemHead) + len);
    memt->tag3 = MEMTAG3;

    memh->alignment = alignment;

    memset(memh + 1, 255, len);

    return (++memh);
}

void MEM_freeN(void* vmemh)
{
    MemTail* memt;
    MemHead* memh = (MemHead*)vmemh;

    memh--;
    if (memh->tag1 == MEMFREE && memh->tag2 == MEMFREE) {
        return;
    }

    if ((memh->tag1 == MEMTAG1) && (memh->tag2 == MEMTAG2) && ((memh->len & 0x3) == 0)) {
        memt = (MemTail*)(((char*)memh) + sizeof(MemHead) + memh->len);
        if (memt->tag3 == MEMTAG3) {  // !!! MSVC 17.2 ASAN fails with "unknown-crash" error on the access of memt->tag3 :(
            memh->tag1 = MEMFREE;
            memh->tag2 = MEMFREE;
            memt->tag3 = MEMFREE;

#ifdef _WIN32
            _aligned_free(MEMHEAD_REAL_PTR(memh));
#else
            free(MEMHEAD_REAL_PTR(memh));
#endif

            return;
        }
    }
}

template<typename T> class GuardedAllocator {
public:
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T value_type;

    GuardedAllocator()
    {
    }
    GuardedAllocator(const GuardedAllocator&)
    {
    }

    T* allocate(size_t n, const void* hint = 0)
    {
        (void)hint;
        size_t size = n * sizeof(T);
        if (n == 0) {
            return NULL;
        }
        T* mem = (T*)MEM_mallocN_aligned(size, 16);
        if (mem == NULL) {
            throw std::bad_alloc();
        }
        return mem;
    }

    void deallocate(T* p, size_t n)
    {
        if (p != NULL) {
            MEM_freeN(p);
        }
    }

    T* address(T& x) const
    {
        return &x;
    }

    const T* address(const T& x) const
    {
        return &x;
    }

    GuardedAllocator<T>& operator=(const GuardedAllocator&)
    {
        return *this;
    }

    size_t max_size() const
    {
        return size_t(-1);
    }

    template<class U> struct rebind {
        typedef GuardedAllocator<U> other;
    };

    template<class U> GuardedAllocator(const GuardedAllocator<U>&)
    {
    }

    template<class U> GuardedAllocator& operator=(const GuardedAllocator<U>&)
    {
        return *this;
    }

    inline bool operator==(GuardedAllocator const& /*other*/) const
    {
        return true;
    }
    inline bool operator!=(GuardedAllocator const& other) const
    {
        return !operator==(other);
    }

#ifdef _MSC_VER
    template<> struct rebind<std::_Container_proxy> {
        typedef std::allocator<std::_Container_proxy> other;
    };

    operator std::allocator<std::_Container_proxy>() const
    {
        return std::allocator<std::_Container_proxy>();
    }
#endif
};

template<typename value_type, typename allocator_type = GuardedAllocator<value_type>>
class my_vector : public std::vector<value_type, allocator_type> {
public:
    typedef std::vector<value_type, allocator_type> BaseClass;

    /* Inherit all constructors from base class. */
    using BaseClass::vector;
};

enum ItemType
{
    None = 0,
    ITEM_A,
    ITEM_B
};

int main()
{
    my_vector<ItemType> stuff;
    stuff.push_back(ITEM_A);
    stuff.push_back(ITEM_B);

    printf("%d %d\n", stuff[0], stuff[1]);
    printf("Success!!\n");
    return 0;
}
