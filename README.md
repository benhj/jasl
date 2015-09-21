# jasl -- Just Another Scripting Language

My probably crappy attempt at a programming language. Please check out the examples folder for examples!

A program
---------

Every program requires a `start` entry point, and can take any number of arguments that can be extracted using the `args` keyword:
<pre>
// main entry point
start {
    // If script expects input arguments use 'arg' keyword. 
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
</pre>

Above, I create three types, an integer, a real, a boolean and a string and store them in variables `a`, `b`, `c` and `d` respectively. Note that in jasl, all variables are global.

Functions
---------

Functions are more like routines, and since every variable is global, there is no concept of a returned value or a parameterized function. A 'function' is indicated by the keyword `block` and are called with the keyword `call`:

<pre>

block otherFunction {
    // Variables are global and 'a'was created earlier so 
    // it can be accessed here.
    echo_nl a;
}

block myFunction {

    // Print out hello and add newline
    echo_nl "Hello";
    
    // And without a newline:
    echo "Hello ";
    
    // Variables are global
    integer 5 -> a;
    
    call otherFunction;
}

// main entry point
start {
    // Run sub-routine myFunction
    call myFunction;
}
</pre>
