const {
    initFileMapping,
    readCommand,
    writeResponse,
    cleanFileMapping
} = require("bindings")("../build/Release/shared-memory-native.node");

initFileMapping();


console.log("[Server] has started listening");

let command = readCommand();

do {
    if (command !== '') {
        console.log("Received command: " + command);
    }
    command = readCommand();
} 
while(command !== "exit")

cleanFileMapping();
