# ZeroScript

This an experiment.

Imagine a language with almost no syntax, designed from scratch for tiny devices.

We write:

    sum (1 2 3)

Or, we write

    1 2 3 sum

phrase:

    ( function [ '(' phrase ')' ] | number | string ) ...

compose:

    function: ( phrase )



This does not yet work, at all.

Problem: I need a neutral language for the Guide. C is fine yet too much of a barrier to many people.
Solution: design a new simple ZeroMQ focused language that provides sufficient power to write examples.

Problem: I'd like a scripting language for Malamute.
Solution: use a scripting language that is fast enough for real work and totally sandboxed.

Problem: I need a neutral language for contract testing. That is, writing code to test the ZeroMQ
contracts around libzmq, czmq, zyre, etc. This language has to be multithreaded, minimalistic.

Problem: I need a simpler way to show how ZeroMQ works for tutorials and conferences.
Solution: a scriptable language that works with an interactive shell.

Problem: I'd like to encourage other languages to implement the CZMQ semantics.
Solution: provide a ZeroScript to C compiler, which targets CZMQ, and then make it easy for
other languages to do the same.

# Inspirations

Forth and Lisp, for simplicity of compilation and extensibility.

Go and Rust, for built in concurrency.

Erlang, for message passing actors.

Gawk, for regular expression matching.

GSL, for code generation.

# Technical Requirements

Performance is valuable but not essential. The language should not get in the way of doing a hundred K messages per second.

Security is paramount. It should not expose any of the machine to abusive code.

Concurrency is paramount.

Readability is paramount. The language should have no funny notations or characters.

Composability is paramount. The language should be built like Forth and Lisp, on itself.

Compilability is valuable. It should be feasible to convert the language into other languages for performance.


Bibliography

http://www.complang.tuwien.ac.at/forth/threaded-code.html
Call Threading
http://www.complang.tuwien.ac.at/forth/threaded-code.html#call-threading


tail call recursion? not reliably optimized

