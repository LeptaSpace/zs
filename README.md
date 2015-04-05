# ZeroScript

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

