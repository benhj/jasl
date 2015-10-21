# jasl -- Just Another Scripting Language

My probably crappy attempt at a programming language. Please check out the examples folder for examples! To build the actual `jasl` interpretor, probably best to just run the following in a terminal prompt:

<pre>
sh build.sh all
</pre>

Note, I have only tested this on a mac platform but it should work on linux and with a bit more work, on windows too. Since the main parsing engine uses boost::spirit, several boost headers are required and should be installed before attempting to build.

The interpretor will let you run a jasl script. E.g.:

<pre>
./jasl examples/recursiveQueens.jasl 8
</pre>

A program
---------

Every program requires a `start` entry point, and can take any number of arguments that can be extracted using the `args` keyword:
<pre>
// main entry point
start {
    // If script expects input arguments use 'args' keyword. 
    // This will put the 0th argument in the variable a. 
    // Note that a is a string.
    args 0 -> a;
}
</pre>


Variables
---------

<pre>
// This is a comment
integer 5 -> a;
decimal 1.1 -> b;
boolean true -> c;
string "Hello!" -> d;
list [This is a list] -> e;
list [This is [a nested list] list] -> f;
</pre>

Above, I create six variables, an integer, a real, a boolean, a string and two lists the latter of which contains a nested list 
and store them in variables `a`, `b`, `c`, `d`, `e` and `f` respectively. ~~Note that in jasl, all variables are global.~~ This is no
longer true. Variables are now scoped at the level of a given block or returnable function. 

Functions
---------

There are two types, 'blocks' and 'returnables' the former of which can be interpreted as a void function, while the
latter is designed to return one of the above basic types.

They have this type of syntax:

<pre>
block myBlock () {

}

returnable integer myReturnable() -> toReturn {

    return toReturn;
}

</pre>

Both are called with the keyword `call`:

<pre>

returnable integer funk (a) -> toReturn {
    integer 10 + a -> toReturn;
    return toReturn;
}

block otherFunction () {
    // Call returnable funk, put result into b
    call funk (5) -> b;
    
    // should print out 15
    echo_nl b;
}

block myFunction () {

    // Print out hello and add newline
    echo_nl "Hello";
    
    // And without a newline:
    echo "Hello ";
    
    // Variables are scoped to the function blockl
    integer 5 -> a;

    // A loop that executes statements within
    // the enclosing braces several times.
    // Note that since variables are scoped to the enclosing
    // function block, the variable b will be accessible still
    // after the loop
    repeat 5 times {
        integer 1 -> b;
    }

    // This is valid. Integer b is still accessible
    echo_nl b;
    
    call otherFunction ();
}

// main entry point
start {
    // Run sub-routine myFunction
    call myFunction ();
}
</pre>

Loops
-----

Control-flow
------------

Handling input
--------------

More on lists
-------------
