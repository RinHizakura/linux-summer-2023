#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

static inline uintptr_t align_up(uintptr_t sz, size_t alignment)
{
    uintptr_t mask = alignment - 1;
    if ((alignment & mask) == 0) { /* power of two? */
        return (sz + mask) & ~mask;
    }
    return (((sz + mask) / alignment) * alignment);
}

int main()
{
    for (int i = 0; i < 100; i++)
        printf("%d align to %ld\n", i, align_up(i, 8));

    return 0;
}
