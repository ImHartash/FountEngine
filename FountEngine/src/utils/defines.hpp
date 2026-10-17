#pragma once
#include <cassert>
#include <intrin.h>

#define RELEASE_COM(x) do { if (x) { (x)->Release(); (x) = nullptr; } } while (0)

#if defined(DEBUG) || defined(_DEBUG)
#ifndef HR
#define HR(x)                                                                         \
        {                                                                             \
            HRESULT hr = (x);                                                         \
            if (FAILED(hr))                                                           \
            {                                                                         \
                LOG_ERROR("HRESULT failed: 0x%08lX for %s", hr, #x);                  \
                __debugbreak();                                                       \
            }                                                                         \
        }
#endif
#else
#ifndef HR
#define HR(x) (x)
#endif
#endif