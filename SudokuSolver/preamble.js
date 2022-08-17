// Taken from https://github.com/nmrugg/stockfish.js
class Queue {
  constructor() {
    this.getter = null;
    this.list = [];
  }
  async get() {
    if (this.list.length > 0) { return this.list.shift(); }
    return new Promise(resolve => this.getter = resolve);
  }
  put(x) {
    if (this.getter) { this.getter(x); this.getter = null; return; }
    this.list.push(x);
  }
};

Module["queue"] = new Queue();
Module["print"] = function(data) {
  postMessage(data);
}

Module.onRuntimeInitialized = function() {
  Module._interactive_mode();
  console.log("SudokuSolver has started")
};

onmessage = function(event) {
  Module["queue"].put(event.data);
}