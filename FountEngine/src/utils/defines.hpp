#pragma once
#include <cassert>
#include <intrin.h>
#include "systems/logging/CLogSystem.hpp"

#define RELEASE_COM(x) if (x) { x->Release(); x = 0; }

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