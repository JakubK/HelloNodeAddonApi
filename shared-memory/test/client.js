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
sendCommand(JSON.stringify({
    type: 'WriteMsg',
    message: 'Hello world!'
}));
sendCommand("inc");
console.log(readResponse());
sendCommand("inc");
console.log(readResponse());
sendCommand("exit");

cleanFileMapping();
