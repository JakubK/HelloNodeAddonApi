import bindings from "bindings";
import { SharedMemoryType } from "../types";

const {
    initFileMapping,
    sendCommand,
    readResponse,
    cleanFileMapping
} = bindings("../build/Release/shared-memory-native.node") as SharedMemoryType

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
