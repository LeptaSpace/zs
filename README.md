# ZeroScript

## This an Experiment

I want a language that makes it trivially easy to write very large scale distributed apps. So easy that I can teach this to my kids and watch them program a thousand devices. My son is four. He uses a computer every day, and plays with his sister's discarded smartphone. Every 18 months, he'll be playing with twice as many devices. By eighteen he'll have a thousand on his desk. He's my target.

Wouldn't it be nice if we could send the code to the data? I mean, this is Web 2.0. I believe it's why JavaScript is such a popular language, I think. Solves the right problem. To deploy JavaScript you push it to a web server. You don't need a compiler, linker, packages, containers, downloads, installers. The tools of our industry, so wrong in so many ways. I like C, yet am keenly aware of the layers of friction between me and my goal of making machines do fun stuff.

So JavaScript is successful despite the actual language, because you can ship the code to a hundred million boxes with no more effort than shipping it to a single one. This is incredibly powerful. Failure costs nothing, and success scales without limit.

Let me observe that the Web is just a poor messaging system. Replace that old clunky HTTP with something like ZeroMQ, and you remove even the remaining cost to scaling. Suddenly you don't need massive servers at the heart of your networks. We can move from Soviet-style central planning to a real market. "You want some code? I got some code."

There's a second reason for JavaScript's emerging domination. That is, we share source code. When you receive JavaScript, you get source code you can read, modify, and reuse. It may not be entirely legal, and people try to prevent this, yet it's the dominant culture. If I get to use your CPU cycles then you can remix my code.

Remixability isn't an added feature. It's a cultural vitamin. Without it, culture grows stunted and crooked, and dies way too young. As does technology. Our industry is built on the corpses of projects too proud or stupid or greedy to take their vitamins. All my closed source work, ever, is dead. All my open source that went past "experiment" is still alive except for two (Libero and Xitami) that I had to actively end in 1999. In the 20th century we hadn't yet invented Github.

Sending source code around the network, and forced remixability go nicely together. I'll cover the legal aspects later. Let's look at sending source code.

Sending source code -- rather than bytecode or worse, machine code -- means that code becomes a valid question, "can you do this?" Let's say I send this to every device in the house:

    temperature name location

A handful will reply, and most will just ignore the request. It's like shouting "who wants beer?" in a cafe in Seoul. You will pick out the English speakers, and the rest of the house will pretend you're not there.

The real world is fuzzy. It's approximate. And it's incredibly lazy. I don't know exactly how many mobile phones I own. Three, maybe. Five, if I count my kids' phones. How can I possibly manage the apps they run? The "personal computer" does not scale. The Web already smashed that paradigm in the last century. Kudos to Sun for recognizing this and pushing it so hard for so long. "The network is the operating system" was Sun's slogan.

My kids are also lazy, so this new language has to be simple. I don't mean trivial. It can be as deep as the ocean. The deeper the better, in fact. However it should start with a gentle paddle in toe-deep water.

We already covered "throwable" (shouting code at a crowd of devices and seeing what happens) and "remixable" (shouting source code, not some stripped form). It has to embrace fuzzy reality, where failure is valid data. I've learned that a successful open source community is a learning machine that needs a steady diet of small, fun failures. This is how I want to build distributed systems.

It must be "concurrent" by default. Obviously things happen all over the place, because that's how reality works. Every atom spins alone, every brain sees its own world. Why is our software  mostly glued to a fake view of a monolithic world? Get rid of that view, and there's nothing to learn.

Lastly, it should be fun to make stuff, share stuff. I want the process of using the language to flow through a learn-play-work-teach cycle. Again, it must happen from first contact. Here's how you make a brick. Here's what the brick can do. Try it! It's safe! Now you can use the brick.

## Inspirations

I remember the moment that computers turned fun, and I turned from being a failing CompSci student to top of the class. My mother bought me a VIC 20 and I started programming it. Then Commodore gave me a C-64 to play with, just because. I wrote games and sold them on cassette, paying my way through studies and beer. But BASIC ran too slowly, and assembler was too much work (and ran too fast, seriously). So for my thesis I wrote a language for making games.

My inspiration was the book, "Threaded Interpretive Languages", by R. G. Loeliger. This remains one of the best books ever written on our art. The book is very hard to find in paper. Luckily there's [a PDF available](http://sinclairql.speccy.org/archivo/docs/books/Threaded_interpretive_languages.pdf).

The C-64's 6502 CPU was a [work of minimalist art](http://e-tradition.net/bytes/6502/6502_instruction_set.html). It only has two general 8-bit registers, X and Y. It has a stack pointer and an accumulator. The instructions are: add, and, branch, compare, decrement, exor, increment, jump, load, shift, push, pop, rotate, store, and subtract. No multiply or divide.

On top of that I built a layer of assembly primitives, and then an assembler and an editor, and then games. Loeliger described how to build Forth, or something like it. Forth is what we should have been studying at university, instead of Pascal.

The thing about Forth, like Lisp, is that your language is the machine. No abstraction stands between your words and the behavior of the "computer". I put that into quotes because obviously you're talking to a virtual computer. Nonetheless, you write executable code. Put that into a read-evaluate-print-loop (REPL) and you get something mind-blowing and addictive.

Forth is also notable for other reasons. It has a tiny core syntax and almost no grammar. The bulk of the language is self-hosting. This means it runs well in very small environments. This is relevant in 2015 because the world is increasingly filled with tiny dirt-cheap computers begging to be programmed. Just not in Java, please.

When I write C I always try to recreate that REPL feeling. I make little scripts that compile, link, run, and print "OK" when the test passes. It works yet it's still slower and clumsier than I'd like, and my precious code still sits on the wrong side of a high, non-portable compile-link-deploy wall.

I like REPL shells that recreate my ideal learn-play-work-teach cycle. It makes programming fun (again). And when programming is fun, and easy, and addictive, then we can dream of a world where to use a device and to program it are the same. A world where we own our software just as we own our languages, our recipes, our poems, our songs.

There are a few other things I really enjoy, from a long career writing software in anger. Perhaps the most powerful is the Finite State Machine (FSM). It's hard to over-state how profitable FSMs have been for me, in taming complex problems and making software that just worked, and never crashed.

My first free software tool was [Libero](http://legacy.imatix.com/html/libero/), which I used in many projects and which had a loyal following. Its charm was that one wrote FSMs as simple text, which Libero turned into code. This was also my first code generator in C, and could produce FSMs in any language. We did C, Pascal, Java, and even PL/SQL and COBOL.

Libero has no bugs as far as I know, and the bits did not rot. However we got much better at code generation, so today I can build [a FSM code generator](https://github.com/hintjens/zs/blob/master/fsm_c.gsl) in a few hours, whereas Libero took me months of work.

The Libero-style state machine is a high level event-driven control language. It replaces iterations and conditionals. At its heart it says, "when I am in state S and I get event E, then do actions A, B, and C". The biggest downside with writing FSMs is that we need two languages; one for the control and one for the actions. I'd like to make this a single language.

I like events and especially, queues of events, which are pipes carrying messages and commands. We explored this in CZMQ 3.0, with actors. These turn out to work incredibly well in C, a language that has no native support for concurrency. We learned how to build very solid message-based APIs, between pieces in the same program.

I'll explore actors in more detail later. For now, let me pin "pipes" on the wall, and remind you of the widely known and under-appreciated concurrent REPL environment called the Unix shell. When you connect commands with pipes, these run concurrently, the output from one becoming the input to the next. It is a simple and obvious model, and fits many of our criteria for distributed code. State sits in the messages, not the code. Each piece can run asynchronously.

So in my language, the default flow is output-to-input, pipes carrying messages and commands from one piece to the next. It's a starting point: I'm sure we'll need more sophsticated queues later.

Other inspirations are obvious: Erlang, Go, Rust (aka "Rushed"), and Clojure. I've noticed from several ZeroMQ workshops that people using Clojure always seem to get their examples done fastest. I suspect it's the REPL again. Whatever, when someone can write ZeroMQ code faster than me, it's time for me to shift to newer tools. And that means moving away from C, at least for the 90% of cases that don't need a systems language.

## Irritations

Now to stuff I hate. It's a long list so I'll try to keep it relevant.

Both Forth and Lisp offer you their salaciously unfiltered virtual machines, and yet both could be simpler. I do not like reverse Polish notation. Stacks are fine data structures, yet they are not intuitive for humans. This hurts my brain, and I don't want to have to try to explain it to anyone:

    1 2 3 4 5 + + + *

Forth is too low on the abstraction level. Do I want to have to explain things like word sizes to my kids just so they can switch on the heating? Clearly not. We can't make throwable code if we care about bits and bytes. And we have C to deal with that part.

I never used Lisp, so my criticisms here are vague and opinionated. I dislike recursion except when it's the obvious algorithm, which means for navigating recursive data structures like trees. I'm not a great fan of trees, as nested structures tend to confuse people. Long ago I learned most people can learn three levels of nesting: this one, the one above, and the one below. I've no opinion on Lisp's parenthesis except they feel like they're in the wrong place:

    (command argument argument)

Which I'd rather write like:

    command (argument argument)

So no stacks, RPNs, or lists of lists. No recursive descent parsers either. A language that needs recursion to parse it is too complex.

Lastly, I dislike error handling. Partly it's from laziness. More though, it's from experience. Oh, great, you got an error code from a library call! What do you do now? It's like getting a phone call from your takeaway pizza place telling you their oven is switched off. What do you do now? Wait, abort, or retry?

No, real code never gives errors. It either works or it dies grimly and with minimal noise. The tolerance of ambiguity causes the very worst crashes. So I want the Erlang approach, where code goes off and tries stuff, and either succeeds or kills itself.

## First Steps



## Bibliography

* http://www.complang.tuwien.ac.at/forth/threaded-code.html
