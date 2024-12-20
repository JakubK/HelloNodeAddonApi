import bindings from "bindings";
import { SharedMemoryType } from "../types";

const {
    initFileMapping,
    readCommand,
    writeResponse,
    cleanFileMapping
} = bindings("../build/Release/shared-memory-native.node") as SharedMemoryType;


let counter = 0;

initFileMapping();

console.log("[Server] has started listening");

let command = readCommand();

do {
    if (command !== '') {
        if (command === 'inc') {
            counter++;
            writeResponse("counter:" + counter);
        } else {
            try {
                const cmd = JSON.parse(command);
                if (cmd.type = "WriteMsg") {
                    console.log("[Server] " + cmd.message);
                }
            } catch {
                console.log("Received command: " + command);
            }
        }

    }
    command = readCommand();
}
while (command !== "exit")

console.log(command);

cleanFileMapping();
