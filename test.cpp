#include <iostream>
#include <Windows.h>
#include <libloaderapi.h>

int main() {
    LoadLibraryA("test.dll");

    std::cout << "Hello world!" << std::endl;

    return 0;
}