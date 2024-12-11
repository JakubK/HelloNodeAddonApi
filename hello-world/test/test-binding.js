const { hello } = require("bindings")("../build/Release/hello-world-native.node");
const assert = require("assert");

assert(hello, "The expected function is undefined");

function testBasic()
{
    const result = hello();
    assert.strictEqual(result, "world", "Unexpected value returned");
}

assert.doesNotThrow(testBasic, undefined, "testBasic threw an expection");

console.log("Tests passed- everything looks OK!");