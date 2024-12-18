#include <napi.h>
#include <windows.h>
#include <cstring>  // For strncpy_s
#include <string>

using namespace Napi;

struct SharedMemory {
  char command[1024];
  char response[1024];
};

static HANDLE hMapFile = NULL;
static SharedMemory* pBuf = NULL;

Napi::Number InitFileMapping(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  hMapFile = CreateFileMapping(
    INVALID_HANDLE_VALUE,    // Use the system paging file
    NULL,                    // Default security attributes
    PAGE_READWRITE,          // Read/write access
    0,                       // Maximum object size (high-order DWORD)
    1024,                    // Maximum object size (low-order DWORD)
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

  return Napi::Number::New(env, 0);
}

Napi::Number SendCommand(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  std::string payloadString = info[0].As<Napi::String>();
  auto payload = payloadString.c_str();
  strncpy_s(pBuf->command, sizeof(pBuf->command), payload, _TRUNCATE);

  return Napi::Number::New(env, 0);
}

Napi::Number WriteResponse(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  std::string payloadString = info[0].As<Napi::String>();
  auto payload = payloadString.c_str();
  strncpy_s(pBuf->response, sizeof(pBuf->response), payload, _TRUNCATE);

  return Napi::Number::New(env, 0);
}


Napi::String ReadCommand(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  return Napi::String::New(env, pBuf->command);
}

Napi::String ReadResponse(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  return Napi::String::New(env, pBuf->response);
}

void CleanFileMapping() {
  if (pBuf != NULL) {
    UnmapViewOfFile(pBuf);
    pBuf = NULL;
  }
  if (hMapFile != NULL) {
    CloseHandle(hMapFile);
    hMapFile = NULL;
  }
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
