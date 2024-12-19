#include <napi.h>
#include <windows.h>
#include <string>
#include <iostream>

using namespace Napi;

// Structure for shared memory
struct SharedMemory {
    char command[1024];
    char response[1024];
};

// Global variables
static HANDLE hMapFile = nullptr;
static SharedMemory* pBuf = nullptr;
static HANDLE hMutex = nullptr;
static HANDLE hClientCommandEvent = nullptr;
static HANDLE hServerProcessedEvent = nullptr;

// Helper function to create a handle and check for errors
HANDLE CreateHandle(LPCSTR name, HANDLE (*createFunc)(LPSECURITY_ATTRIBUTES, BOOL, LPCSTR), const char* errorMsg) {
    HANDLE handle = createFunc(nullptr, FALSE, name);
    if (!handle) {
        std::cerr << errorMsg << ": " << GetLastError() << std::endl;
    }
    return handle;
}

// Initializes shared memory and synchronization primitives
Napi::Number InitFileMapping(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    // Create shared memory
    hMapFile = CreateFileMapping(
        INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, sizeof(SharedMemory), "Global\\MySharedMemory"
    );
    if (!hMapFile) {
        std::cerr << "Failed to create file mapping: " << GetLastError() << std::endl;
        return Napi::Number::New(env, 1);
    }

    // Map shared memory
    pBuf = static_cast<SharedMemory*>(MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0));
    if (!pBuf) {
        std::cerr << "Failed to map view of file: " << GetLastError() << std::endl;
        CloseHandle(hMapFile);
        return Napi::Number::New(env, 1);
    }

    // Create synchronization primitives
    hMutex = CreateHandle("Global\\MySharedMemoryMutex", CreateMutex, "Failed to create mutex");
    hClientCommandEvent = CreateHandle("Global\\ClientCommandEvent", CreateEvent, "Failed to create client command event");
    hServerProcessedEvent = CreateHandle("Global\\ServerProcessedEvent", CreateEvent, "Failed to create server processed event");

    if (!hMutex || !hClientCommandEvent || !hServerProcessedEvent) {
        CleanFileMapping(info); // Cleanup already allocated resources
        return Napi::Number::New(env, 1);
    }

    // Reset shared memory buffers
    strncpy_s(pBuf->command, sizeof(pBuf->command), "", _TRUNCATE);
    strncpy_s(pBuf->response, sizeof(pBuf->response), "", _TRUNCATE);

    return Napi::Number::New(env, 0);
}

// Sends a command through shared memory
Napi::Number SendCommand(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    std::string payload = info[0].As<Napi::String>();

    WaitForSingleObject(hMutex, INFINITE);
    strncpy_s(pBuf->command, sizeof(pBuf->command), payload.c_str(), _TRUNCATE);
    SetEvent(hClientCommandEvent);
    WaitForSingleObject(hServerProcessedEvent, INFINITE);
    ReleaseMutex(hMutex);

    return Napi::Number::New(env, 0);
}

// Reads a command from shared memory
Napi::String ReadCommand(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    WaitForSingleObject(hClientCommandEvent, INFINITE);
    Napi::String command = Napi::String::New(env, pBuf->command);
    SetEvent(hServerProcessedEvent);

    return command;
}

// Writes a response to shared memory
Napi::Number WriteResponse(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    std::string response = info[0].As<Napi::String>();

    WaitForSingleObject(hMutex, INFINITE);
    strncpy_s(pBuf->response, sizeof(pBuf->response), response.c_str(), _TRUNCATE);
    ReleaseMutex(hMutex);

    return Napi::Number::New(env, 0);
}

// Reads a response from shared memory
Napi::String ReadResponse(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    WaitForSingleObject(hMutex, INFINITE);
    Napi::String response = Napi::String::New(env, pBuf->response);
    strncpy_s(pBuf->response, sizeof(pBuf->response), "", _TRUNCATE); // Reset buffer
    ReleaseMutex(hMutex);

    return response;
}

// Cleans up shared memory and synchronization primitives
void CleanFileMapping(const Napi::CallbackInfo& info) {
    if (pBuf) {
        UnmapViewOfFile(pBuf);
        pBuf = nullptr;
    }
    if (hMapFile) {
        CloseHandle(hMapFile);
        hMapFile = nullptr;
    }
    if (hMutex) {
        CloseHandle(hMutex);
        hMutex = nullptr;
    }
    if (hClientCommandEvent) {
        CloseHandle(hClientCommandEvent);
        hClientCommandEvent = nullptr;
    }
    if (hServerProcessedEvent) {
        CloseHandle(hServerProcessedEvent);
        hServerProcessedEvent = nullptr;
    }
}

// Module initialization
Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set("initFileMapping", Napi::Function::New(env, InitFileMapping));
    exports.Set("sendCommand", Napi::Function::New(env, SendCommand));
    exports.Set("writeResponse", Napi::Function::New(env, WriteResponse));
    exports.Set("readCommand", Napi::Function::New(env, ReadCommand));
    exports.Set("readResponse", Napi::Function::New(env, ReadResponse));
    exports.Set("cleanFileMapping", Napi::Function::New(env, CleanFileMapping));
    return exports;
}

NODE_API_MODULE(addon, Init)
