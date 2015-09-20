# jasl -- Just Another Scripting Language

My probably crappy attempt at a programming language. Please check out the examples folder for examples!

Variables
---------

<pre>
// This is a comment
integer 5 -> a;
decimal 1.1 -> b;
string "Hello!" -> c;
</pre>

Above, I create three types, an integer, a real and a string and store them in variables `a`, `b` and `c` respectively. Note that in jasl, all variables are global.

Functions
---------

Every script has a main entry point `start`. Functions are more like routines, since every variable is global, there is no concept of a returned value or a parameterized function. A 'function' is indicated by the keyword `block` and are called with the keyword `call`:

<pre>
block myFunction {
    // other code!
    
    // Print out hello and add newline
    echo_nl "Hello";
    
    // And without a newline:
    echo "Hello";
}

// main entry point
start {

    // If script expects input arguments use 'arg' keyword. 
    // This will put the 0th argument in the variable a. Note
    // the type is a string.
    args 0 -> a;
    
    // Implicitly convert string a to an integer:
    integer a -> b;

    // Run sub-routine myFunction
    call myFunction;

}
</pre>
