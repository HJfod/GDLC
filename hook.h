// thanks to firecubez

#pragma once

#include <Windows.h>

#include <vector>

typedef struct {
    volatile BYTE* fn;
    BYTE* tramp;
    __int64 orig;
} trampoline_t;

trampoline_t trampoline(volatile BYTE* fn, BYTE* hook, BYTE extra);
void untrampoline(trampoline_t& tramp);

extern std::vector<trampoline_t> trampolines;

void add_trampoline(BYTE* fn, BYTE* hook, BYTE extra);
void untrampoline_all();

#define HOOK_START(name) __declspec(naked) void name ## _HOOK() { __asm {

#define HOOK_END } }