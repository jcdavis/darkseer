# Darkseer

Darkseer is an experimental JVM microprofiler for analyzing actual memory allocations in a thread. It will be particularly of interest to people looking to analyze the real-world memory usage of c2 compiled methods, free from the interference that comes from bytecode manipulating profilers.

# Warning

Darkseer works by taking advantage of specific register allocation and memory layout of recent builds of OpenJDK on x86-64 to read internal JVM state. These assumptions are not likely to work on other JDKs, or even older versions of OpenJDK. These access patterns are very non-kosher and are highly likely to cause the JVM to crash. Never run Darkseer anywhere you can't tolerate a JVM crash (ie any production deployment).

# Building

The only requirement for building Darkseer is having `JAVA_HOME` configured to point to an installed jdk. To build, run `make demo`, which will build both the agent as well as the interface jar, and then build a run a simple demo.

If you are modifying the signature of the native methods, running `make stubs` will create a generated wrapper file `target/stubs.c`

# Running

To run, pass `-agentpath:path/to/ldsagent.so` to java and make sure `darkseer.jar` in on your classpath (see demo in Makefile). You start recording via `DarkSeer.start()` and stop with `DarkSeer.end()`, which will print out the total number of bytes allocated as well and the class and size of every allocated object.

# Limitations

Here are just a few:

* Darkseer is very fragile and has little sanity checks. Most GC events will cause it to crash the JVM (Somewhat fixable)
* We can only look at allocations in a single TLAB slice. This is a fundamental limitation, but can be somewhat circumvented via increasing the TLAB size using `-XX:MinTLABSize=`
* Darkseer can only record allocations for the thread that called it (unfixable)
* Darkseer can only record allocations for a single thread at a time (fixable)
* No stack traces of allocations can be provided (likely unfixable)
* Output is currently just printed to stdout (fixable)


