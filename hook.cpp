// thanks to firecubez (again)

#include <Windows.h>

#include <stdexcept>
#include <vector>
#include <iostream>

#include "hook.h"

trampoline_t trampoline(volatile BYTE* fn, BYTE* hook, BYTE extra) {
    DWORD oldProt;
    VirtualProtect((void*) fn, 5, PAGE_EXECUTE_READWRITE, &oldProt);
    auto tramp = (unsigned char*)VirtualAlloc(NULL,
        5 + 5 + extra + 5, // 5 = hook size
        MEM_COMMIT | MEM_RESERVE,
        PAGE_EXECUTE_READWRITE
    );
    if (tramp == nullptr) {
        perror("VirtualAlloc failed");
        throw std::runtime_error("unrecoverable error");
    }

    // make trampoline call the hook
    tramp[0] = 0xE8;
    DWORD r1 = (size_t)hook - (size_t)tramp - 5;
    memcpy(tramp + 1, &r1, 4);

    // do instructions that we will override
    memcpy(tramp + 5, (const void*) fn, 5 + extra);

    // jump back
    DWORD r2 = (size_t)(fn + 5 + extra) - (size_t)(tramp + 5 + 5 + extra) - 5;
    tramp[5 + 5 + extra] = 0xE9;
    memcpy(tramp + 5 + 5 + extra + 1, &r2, 4);

    // actual hook
    __int64 toBeFn = *(__int64*) fn; // XXX: is unaligned read safe in C++, and are functions always 8 byte aligned?
    BYTE* toBeFnB = (BYTE*) &toBeFn;
    toBeFnB[0] = 0xE9;
    DWORD r3 = (size_t)tramp - (size_t)fn - 5;
    memcpy(toBeFnB + 1, &r3, 4);
    
    // actually do the hook
    __int64 orig = InterlockedExchange64((volatile __int64*) fn, toBeFn);

    return {fn, tramp, orig};
}

void untrampoline(trampoline_t& tramp) {
    InterlockedExchange64((volatile __int64*) tramp.fn, tramp.orig);
    if(!VirtualFree(tramp.tramp, 0, MEM_RELEASE)) {
        perror("VirtualFree failed");
    }
}

std::vector<trampoline_t> trampolines;

void add_trampoline(BYTE* fn, BYTE* hook, BYTE extra) {
    trampolines.push_back(trampoline(fn, hook, extra));
}

void untrampoline_all() {
    for(auto& x : trampolines) {
        untrampoline(x);
    }
    trampolines.clear();
}