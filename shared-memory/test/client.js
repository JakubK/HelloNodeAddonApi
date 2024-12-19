const {
    initFileMapping,
    sendCommand,
    readResponse,
    cleanFileMapping
} = require("bindings")("../build/Release/shared-memory-native.node");

initFileMapping();

sendCommand("SomeCommand");
sendCommand("SomeCommand1");
sendCommand("exit");

// readResponse();
cleanFileMapping();
