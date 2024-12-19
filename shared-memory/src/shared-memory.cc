#include <napi.h>
#include <windows.h>
#include <string>
#include <iostream>

using namespace Napi;

struct SharedMemory {
  char command[1024];
  char response[1024];
};

static HANDLE hMapFile = NULL;
static SharedMemory* pBuf = NULL;
static HANDLE hMutex = NULL;

static HANDLE hClientCommandEvent = NULL;
static HANDLE hServerProcessedEvent = NULL;

Napi::Number InitFileMapping(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  hMapFile = CreateFileMapping(
    INVALID_HANDLE_VALUE,    // Use the system paging file
    NULL,                    // Default security attributes
    PAGE_READWRITE,          // Read/write access
    0,                       // Maximum object size (high-order DWORD)
    sizeof(SharedMemory),                    // Maximum object size (low-order DWORD)
    "Global\\MySharedMemory" // Name of the shared memory object
  );

  if (hMapFile == NULL) {
    printf("Could not create file mapping object: %d", GetLastError());
    std::cerr << "" << GetLastError() << std::endl;
    return Napi::Number::New(env, 1);
  }

  // Map the shared memory object into the address space
  pBuf = (SharedMemory*)MapViewOfFile(
    hMapFile,                // Handle to the map object
    FILE_MAP_ALL_ACCESS,     // Read/write access
    0,                       // File offset (high-order DWORD)
    0,                       // File offset (low-order DWORD)
    0                        // Number of bytes to map (0 maps the entire file)
  );

  if (pBuf == NULL) {
    printf("Could not map view of file: %d", GetLastError());
    CloseHandle(hMapFile);
    return Napi::Number::New(env, 1);
  }

  hMutex = CreateMutex(NULL, FALSE, "Global\\MySharedMemoryMutex");
  if (hMutex == NULL) {
    printf("Could not create mutex: %d", GetLastError());
    UnmapViewOfFile(hMapFile);
    CloseHandle(hMapFile);
    return Napi::Number::New(env, 1);
  }

  hClientCommandEvent = CreateEvent(NULL, FALSE, FALSE, "Global\\ClientCommandEvent");
  if (hClientCommandEvent == NULL) {
    printf("Could not create client event: %d", GetLastError());
    return Napi::Number::New(env, 1);
  }

  
  hServerProcessedEvent = CreateEvent(NULL, FALSE, FALSE, "Global\\ServerProcessedEvent");
  if (hServerProcessedEvent == NULL) {
    printf("Could not create server event: %d", GetLastError());
    return Napi::Number::New(env, 1);
  }

  // Reset shared memory
  strncpy_s(pBuf->command, sizeof(pBuf->command), "", _TRUNCATE);
  strncpy_s(pBuf->response, sizeof(pBuf->response), "", _TRUNCATE);

  return Napi::Number::New(env, 0);
}

Napi::Number SendCommand(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  std::string payloadString = info[0].As<Napi::String>();
  auto payload = payloadString.c_str();

  WaitForSingleObject(hMutex, INFINITE);
  strncpy_s(pBuf->command, sizeof(pBuf->command), payload, _TRUNCATE);
  SetEvent(hClientCommandEvent);
  WaitForSingleObject(hServerProcessedEvent, INFINITE);
  ReleaseMutex(hMutex);

  return Napi::Number::New(env, 0);
}

Napi::String ReadCommand(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  WaitForSingleObject(hClientCommandEvent, INFINITE);
  auto command = Napi::String::New(env, pBuf->command);
  SetEvent(hServerProcessedEvent);

  return command;
}

Napi::Number WriteResponse(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  std::string payloadString = info[0].As<Napi::String>();
  auto payload = payloadString.c_str();

  WaitForSingleObject(hMutex, INFINITE);
  strncpy_s(pBuf->response, sizeof(pBuf->response), payload, _TRUNCATE);
  ReleaseMutex(hMutex);

  return Napi::Number::New(env, 0);
}

Napi::String ReadResponse(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  WaitForSingleObject(hMutex, INFINITE);
  auto response = Napi::String::New(env, pBuf->response);
  // Reset buffer
  strncpy_s(pBuf->response, sizeof(pBuf->response), "", _TRUNCATE);
  ReleaseMutex(hMutex);
  return response;
}

void TryCleanHandle(HANDLE h) {
  if (h != NULL) {
    CloseHandle(h);
    h = NULL;
  }
}

void CleanFileMapping(const Napi::CallbackInfo& info) {
  if (pBuf != NULL) {
    UnmapViewOfFile(pBuf);
    pBuf = NULL;
  }
  TryCleanHandle(hMapFile);
  TryCleanHandle(hMutex);
  TryCleanHandle(hClientCommandEvent);
  TryCleanHandle(hServerProcessedEvent);
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set(Napi::String::New(env, "initFileMapping"), Napi::Function::New(env, InitFileMapping));
  exports.Set(Napi::String::New(env, "sendCommand"), Napi::Function::New(env, SendCommand));
  exports.Set(Napi::String::New(env, "writeResponse"), Napi::Function::New(env, WriteResponse));
  exports.Set(Napi::String::New(env, "readCommand"), Napi::Function::New(env, ReadCommand));
  exports.Set(Napi::String::New(env, "readResponse"), Napi::Function::New(env, ReadResponse));
  exports.Set(Napi::String::New(env, "cleanFileMapping"), Napi::Function::New(env, CleanFileMapping));
  
  return exports;
}

NODE_API_MODULE(addon, Init)
