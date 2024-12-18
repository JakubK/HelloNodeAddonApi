## shared-memory

Node addon introduces methods wrapping the WinAPI specific code:

- initFileMapping - Initialization of shared memory
- sendCommand - Writes string to shared memory command buffer
- writeResponse - Writes string to shared memory response buffer
- readCommand - Returns content of command buffer as a string
- readResponse - Returns content of response buffer as a string
- cleanFileMapping - Clean up of shared memory winapi stuff

## Getting started

### Prerequisites

1. Windows System
2. Visual Studio with C++ configuration

Without these, setup is likely to fail on configure step 

### Setup
1. npm install
2. npm run configure

### Development
1. npm run build
2. npm run test