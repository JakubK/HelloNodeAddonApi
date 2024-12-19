## shared-memory

Node addon introduces methods wrapping the WinAPI specific code:

- initFileMapping - Initialization of shared memory
- sendCommand - Writes string to shared memory command buffer
- writeResponse - Writes string to shared memory response buffer
- readCommand - Returns content of command buffer as a string
- readResponse - Returns content of response buffer as a string
- cleanFileMapping - Clean up of shared memory winapi stuff

Code attached in test directory allows to execute some client-server code on shared memory.

Server has handlers for 2 simple methods:
- inc - which increments inner counter and writes it value to response
- WriteMsg which is passed as a json with message to be printed on server
- Everything else will be just printed out

both command and response buffers are limited to 1024 bytes each.

## Getting started

Evaluating this addon requires admin privileges

### Prerequisites

1. Windows System
2. Visual Studio with C++ configuration

Without these, setup is likely to fail on configure step 

### Setup
1. npm install
2. npm run configure

### Development
1. npm run build

(Terminal session 1)
2. npm run server

(Terminal session 2)
2. npm run client