#pragma once

#include <cstdint>
#include <string>

#define CHECK(f)                                                           \
    do                                                                     \
    {                                                                      \
        if (!(f))                                                          \
        {                                                                  \
            printf("Abort. %s failed at %s:%d\n", #f, __FILE__, __LINE__); \
            abort();                                                       \
        }                                                                  \
    } while (false)

#define LOGE(f)                                                \
    do                                                         \
    {                                                          \
        printf("ERROR: %s at %s:%d\n", f, __FILE__, __LINE__); \
        abort();                                               \
    } while (false)

#define LOGW(f)                                                  \
    do                                                           \
    {                                                            \
        printf("WARNING: %s at %s:%d\n", f, __FILE__, __LINE__); \
    } while (false)

const int c_windowWidth = 1600;
const int c_windowHeight = 1200;

template<typename T>
uint32_t ui32Size(const T& container)
{
    return static_cast<uint32_t>(container.size());
}