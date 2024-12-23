const { hello, add, asArray, messageBox, getCounter, incrementCounter } = require("bindings")("../build/Release/hello-world-native.node");
const assert = require("assert");

assert(hello, "The expected function is undefined");
assert(add, "The expected function is undefined");
assert(asArray, "The expected function is undefined");
assert(messageBox, "The expected function is undefined");

assert(getCounter, "The expected function is undefined");
assert(incrementCounter, "The expected function is undefined");

function testBasic()
{
    const result = hello();
    assert.strictEqual(result, "world", "Unexpected value returned");

    const a = 5;
    const b = 3;
    const sum = add(a, b);
    assert.strictEqual(sum, a + b, "Unexpected value returned");

    assert.deepEqual(asArray(1,2,3), [1,2,3]);
}

assert.doesNotThrow(testBasic, undefined, "testBasic threw an expection");

messageBox("This is title", "This is content")

assert.equal(getCounter(), 0);
incrementCounter();
assert.equal(getCounter(), 1);

console.log("Tests passed- everything looks OK!");