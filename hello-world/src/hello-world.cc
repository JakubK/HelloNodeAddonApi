#include <napi.h>
#include <windows.h>

using namespace Napi;

Napi::String Method(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  return Napi::String::New(env, "world");
}

Napi::Number AddMethod(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  
  auto a = double(info[0].As<Napi::Number>());
  auto b = double(info[1].As<Napi::Number>());

  return Napi::Number::New(env, a + b);
}

Napi::Array AsArrayMethod(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  
  auto n = info.Length();

  auto tab = Napi::Array::New(env, n);
  for(int i = 0;i < n;i++) {
    tab[i] = info[i];
  }

  return tab;
}

void MessageBoxMethod(const Napi::CallbackInfo& info) {
  std::string titleString = info[0].As<Napi::String>();
  auto lpTitle = titleString.c_str();

  std::string messageString = info[1].As<Napi::String>();
  auto lpMessage = messageString.c_str();

  MessageBox(
    NULL,
    lpMessage,
    lpTitle,
    MB_OK | MB_ICONINFORMATION
  );
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set(Napi::String::New(env, "hello"), Napi::Function::New(env, Method));
  exports.Set(Napi::String::New(env, "add"), Napi::Function::New(env, AddMethod));
  exports.Set(Napi::String::New(env, "asArray"), Napi::Function::New(env, AsArrayMethod));
  exports.Set(Napi::String::New(env, "messageBox"), Napi::Function::New(env, MessageBoxMethod));
  
  return exports;
}


NODE_API_MODULE(addon, Init)
