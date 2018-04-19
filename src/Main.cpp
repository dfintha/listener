#include "DebugThread.hpp"
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char **argv) {
    WORD colorAttrs;
    HANDLE stdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    
    {
        CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
        GetConsoleScreenBufferInfo(stdOut, &bufferInfo);
        colorAttrs = bufferInfo.wAttributes;
    }

    std::vector<DebugThread *> threads;
    --argc, ++argv;
    for (int i = 0; i < argc; ++i) {
        const DWORD pid = std::atoi(argv[i]);
        if (pid != 0) {
            threads.push_back(new DebugThread(pid, nullptr));
            threads.back()->Attach();
        }
    }

    std::string line;
    while (std::getline(std::cin, line)) {
        // let the user interrupt listening by sending an eof
    }

    SetConsoleTextAttribute(stdOut, colorAttrs);
    for (auto& thread : threads)
        delete thread;
    return EXIT_SUCCESS;
}
