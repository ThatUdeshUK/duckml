#pragma once

#define LLM_USE_THREADS 1
#define IS_SCHEMA 1

#ifdef NDEBUG
#define LLM_LOG(x) do {} while(0)
#else
#include <iostream>
// #define LLM_LOG(x) do {} while(0)
#define LLM_LOG(x) do { std::cout << x; } while(0)
#endif