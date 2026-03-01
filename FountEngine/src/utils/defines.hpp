#pragma once
#include <cassert>

#define RELEASE_COM(x) if (x) { x->Release(); x = 0; }