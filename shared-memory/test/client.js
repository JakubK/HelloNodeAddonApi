const {
    initFileMapping,
    sendCommand,
    readResponse,
    cleanFileMapping
} = require("bindings")("../build/Release/shared-memory-native.node");

initFileMapping();

sendCommand("SomeCommand");
sendCommand("AnotherCommand");
sendCommand("Test");
sendCommand("exit");

// readResponse();
cleanFileMapping();
