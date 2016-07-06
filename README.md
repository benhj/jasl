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
int 5 -> a;
real 1.1 -> b;
bool true -> c;
string "Hello!" -> d;
list [This is a list] -> e;
list [This is [a nested list] list] -> f;
</pre>

Above, I create six variables, an int, a real, a bool, a string and two lists the latter of which contains a nested list
and store them in variables `a`, `b`, `c`, `d`, `e` and `f` respectively. ~~Note that in jasl, all variables are global.~~ This is no
longer true. Variables are now scoped at the level of a given block or returnable function.

To update them, one can use the `put` command, e.g.:

<pre>
put (a + 1) -> a;
put 2.2 -> b;
put false -> c;
put "goodbye" -> d;
// etc.
</pre>

Arrays
------

Arrays are a fairly new and experimental feature and aren't yet fully supported. Only integer and real types are currently 
available. 

To create some arrays, you do something like:

<pre>

// integer array
array:int(5) -> a;

// double array
array:real(10) -> d;

</pre>

Note all arrays are implcitly zeroed out. 

To set a value at a given index, use `put`:

<pre>
// put value 1 in to index location 2
put 1 -> a(2);
</pre>

As with many languages, array indexing starts at zero. 

To retrieve a value from the array, use `get`:

<pre>
// retrieve the value at index 2 from a and put into b
get a(2) -> b;
</pre>

Please consult example `workingWithArrays.jasl` for a larger treatment.

Functions
---------

There are two types, 'blocks' and 'returnables' the former of which can be interpreted as a void function, while the
latter is designed to return one of the above basic types.

They have this type of syntax:

<pre>
block myBlock () {
}

// No need for an 'explicit' return statement
// since this is already implicit in the arrow return
fn:int myReturnable() -> toReturn {
}

</pre>

Update 8/6/16: now also have a void return type, hence the `nil` type:

<pre>
fn:nil myBlock () {
}
</pre>

is equivalent to the 'block' example from above.

Both are called with the keyword `call`:

<pre>

fn:int funk (a) -> toReturn {
    int 10 + a -> toReturn;
}

block otherFunction () {
    // Call returnable funk, put result into b
    call funk (5) -> b;

    // should print out 15
    prn b;
}

block myFunction () {

    // Print out hello and add newline
    prn "Hello";

    // And without a newline:
    pr "Hello ";

    // Variables are scoped to the function blockl
    int 5 -> a;

    // A loop that executes statements within
    // the enclosing braces several times.
    // Note that since variables are scoped to the enclosing
    // function block, the variable b will be accessible still
    // after the loop
    repeat 5 times {
        int 1 -> b;
    }

    // This is valid. Integer b is still accessible
    prn b;

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

Three types of loop are provided.
A while loop that has C-style syntax:

<pre>
int 0 -> i;
while (i < 10) {
    int (i + 1) -> i;
}
</pre>

A repeat loop. I think I saw this in POP-11 and I found it damn handy:

<pre>
repeat 10 times {
   prn "hello!";
}
</pre>

Finally a list or array element iterator:

<pre>
// list example
list [this is a list] -> mylist;
for element in mylist {
    prn element;
}

// array example
array:real(10) -> myarray;
for element in myarray {
    prn element;
}
</pre>



If-else
-------
Conditional branching is c-style with the one exception that `if else` is not currently supported. Examples:

<pre>
if (1 < 2) {
    prn "true";
}
if (2 < 1) {
    prn "true";
} else {
    prn "false";
}
</pre>

Handling input
--------------
Handling input is easy:
<pre>
// start is the main entry point
start {
    // query for user input
    input "What is your name? " -> name;
    string "Hello, " -> s;
    append (s, name) -> s;
    prn s;
}
</pre>

More on lists
-------------
Lists are containers of string elements that may contain nested lists:

<pre>
list [] -> emptyList;
list [just a plain list] -> A;
list [Test a [nested list] list] -> L;
list [a very [very [even more nested] nested] nested list] -> Q;
</pre>

A number of operators can be used to add elements to and extract elements from lists.

Setting tokens:

<pre>
list [hello there] -> t;
set_token (0, t, "goodbye") -> t;
// list t is now [goodbye there]

set_token (0, t, [nested bit]) -> t;
// list t is now [[nested bit] there]

set_token (1, [another list], "test") -> q;
// q is [another test]
</pre>

Getting tokens:

<pre>
// extract token 0, store in string t
get_token(0, [hello there]) -> t;

// make a new list, q
list [what the flip] -> q;

// extract token 1 from q and store in string r
get_token(1, q) -> r;
</pre>

Adding tokens (similar to `set_token`):

<pre>
// add a token to the end of a list
list [a big list] -> L;
add_token("token", L);
</pre>

Find the index of first matching token:

<pre>
index_of ("hello", [hello there]) -> s;
</pre>

Converting a list to a string:

<pre>
list_to_string [hello there] -> s
</pre>

Iterating over list elements:

<pre>
list [this is a list] -> mylist;
for element in mylist {
    prn element;
}
</pre>

Miscellaneous
-------------
Getting the type of a variable:

<pre>
// suppose we need to figure out the type of a,
// use the type command and put result in a string.
int 1 -> a;
type a -> theTypeOfA;
</pre>

Releasing a variable if we no longer need it:

<pre>
real 1.1 -> d;
// some time later, after we're finished with d:
release d;
</pre>

Generating random ints:

<pre>
// generate an int in the range [0, 5]:
random_int(5) -> result;
</pre>
