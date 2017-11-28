# Darkseer

Darkseer is an experimental JVM profiler for analyzing actual memory allocations in a thread. It may be of interest to people looking to analyze the real-world memory usage of c2 compiled methods, free from the interference that comes from bytecode manipulating profilers and without having to use commercial features of the Oracle JDK (eg JFR). For more background on how it works, read [my blog post](http://jcdav.is/2016/07/11/JVM-allocation-secrets/).

**This should be considered to be a proof of concept for the curious only.**

# Warning

Darkseer works by taking advantage of specific register allocation and memory layout of recent builds of OpenJDK on x86-64 to read internal JVM state. These assumptions are not likely to work on other JDKs, or even older versions of OpenJDK. These access patterns are very non-kosher and are likely to cause the JVM to crash. Don't run this anywhere you can't tolerate a JVM crash (ie any production deployment).

# Building

The only requirements for building Darkseer are having clang/gcc and `JAVA_HOME` configured to point to an installed jdk. To build, run `make demo`, which will build both the agent as well as the interface jar, and then run a simple demo.

If you are modifying the signature of the native methods, running `make stubs` will create a generated header file in `target` with the correct function definitions

# Running

To run, pass `-agentpath:path/to/ldsagent.so` to java and make sure `darkseer.jar` in on your classpath (see the demo in the Makefile). You start recording via `DarkSeer.start()` and stop with `DarkSeer.end()`, which will print out the total number of bytes allocated as well and the class and size of every allocated object, with controllable object content printing. (0 disables all output - 3 prints all objects) 

If you have scala installed, `make scalarepl` will start a scala repl with correct agent/classpath params.
If you have java 9+, `make jshell` will start a java repl with the correct agent/classpath params.

# Limitations

Here are just a few:

* Darkseer is fragile and only has basic sanity checks. Some GC events will cause it to crash the JVM (Somewhat fixable)
* We can only look at allocations in a single TLAB slice. This is a fundamental limitation, but can be somewhat circumvented via increasing the TLAB size using `-XX:MinTLABSize=`
* Some large objects will not be allocated in the TLAB. Darkseer can detect that this has happened, but cannot identify those alocations (unfixable)
* Darkseer can only record allocations for the thread that called it (unfixable)
* Darkseer can only record allocations for a single thread at a time (fixable)
* No stack traces of allocations can be provided (likely unfixable)
* Output is currently just printed to stdout (fixable)


