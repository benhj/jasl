### Abandoning 
This project has been fun, but due to very bad implementation choices (not least of which is the reliance on Boost Spirit to handle parsing), it has been very difficult to maintain and extend. For this reason, I am abandoning JASL and re-implementing the interpretor from scratch. Updates on this to follow.

```
 ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄           
▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░▌          
 ▀▀▀▀▀█░█▀▀▀ ▐░█▀▀▀▀▀▀▀█░▌▐░█▀▀▀▀▀▀▀▀▀ ▐░▌          
      ▐░▌    ▐░▌       ▐░▌▐░▌          ▐░▌          
      ▐░▌    ▐░█▄▄▄▄▄▄▄█░▌▐░█▄▄▄▄▄▄▄▄▄ ▐░▌          
      ▐░▌    ▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░▌          
      ▐░▌    ▐░█▀▀▀▀▀▀▀█░▌ ▀▀▀▀▀▀▀▀▀█░▌▐░▌          
      ▐░▌    ▐░▌       ▐░▌          ▐░▌▐░▌          
 ▄▄▄▄▄█░▌    ▐░▌       ▐░▌ ▄▄▄▄▄▄▄▄▄█░▌▐░█▄▄▄▄▄▄▄▄▄ 
▐░░░░░░░▌    ▐░▌       ▐░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌
 ▀▀▀▀▀▀▀      ▀         ▀  ▀▀▀▀▀▀▀▀▀▀▀  ▀▀▀▀▀▀▀▀▀▀▀ 
 ```

# jasl -- Just Another Scripting Language

My attempt at a programming language. Please check out the examples folder for examples! To build the actual `jasl` interpretor, prob best to follow the cmake convention:

```
mkdir build
cd build
cmake ..
make
```

Note, I have only tested this on a mac platform but it should work on linux and with a bit more work, on windows too. Since the main parsing engine uses boost::spirit, several boost headers are required and should be installed before attempting to build. The libicu development libraries an headers are also required by the parsing engine -- basically to allow the parsing of code points and escape characters. It's a pain in the arse to build but seems to work well. It is assumed by the build script that all boost and icu libs are under `/usr/local/lib` and headers under `/usr/local/include`. Finally, libcurl and associated developer headers are required for the networking API.

The interpretor will let you run a jasl script. E.g.:

```
./jasl examples/recursiveQueens.jasl 8
```

Note: running the binary without any arguments will drop you into an interactive shell.

TODO
----

The plan is to support the following stuff. I will cross out when each bit is done, and add more stuff as I think of it.

* ~~basic types (64-bit integers, reals, arrays, strings, lists).~~
* ~~random number generation~~
* ~~conditionals~~
* ~~loops~~
* ~~functions~~
* ~~file i/o API~~
* ~~scoped variable allocation~~
* ~~program argument handling~~
* ~~command line input~~
* ~~printing to standard out~~
* ~~regex string matching~~
* basic networking API
* basic GUI support (draing of primitive shapes etc.)


A program
---------

Every program requires a `start` entry point, and can take any number of arguments that can be extracted using the `args` keyword:
```
;;; main entry point
start {
    ;;; If script expects input arguments use 'args' keyword.
    ;;; This will put the 0th argument in the variable a.
    ;;; Note that a is a string.
    args 0 -> a;
}
```


Variables
---------

```
;;; This is a comment
int 5 -> a;
real 1.1 -> b;
bool true -> c;
string "Hello!" -> d;
byte 80 -> e;
list [This is a list] -> f;
list [This is [a nested list] list] -> g;
```

Above, I create seven variables, an int, a real, a bool, a string, a byte, and two lists the latter of which contains a nested list
and store them in variables `a`, `b`, `c`, `d`, `e` and `f` respectively. ~~Note that in jasl, all variables are global.~~ This is no
longer true. Variables are now scoped at a given block level (blocks, functions, if, else, while, for, repeat, etc.).

To update them, one can use the `put` command, e.g.:

```
put (a + 1) -> a;
put 2.2 -> b;
put false -> c;
put "goodbye" -> d;
;;; etc.
```

Arrays
------

Arrays are a fairly new and experimental feature and aren't yet fully supported. Only integer and real types are currently 
available. 

To create some arrays, the primitive types are 'pluralised'  so you do something like:

```
;;; integer array
ints(5) -> a;

;;; double array
reals(10) -> d

;;; byte array
bytes(10) -> bytes;
```

Note all arrays are implcitly zeroed out. 

To set a value at a given index, use `put`:

```
;;; put value 1 in to index location 2
put 1 -> a(2);
```

As with many languages, array indexing starts at zero. 

To retrieve a value from the array, use `get`:

```
;;; retrieve the value at index 2 from a and put into b
get a(2) -> b;
```

Please consult example `workingWithArrays.jasl` for a larger treatment.

Functions
---------

There are two types, 'blocks' and 'returnables' the former of which can be interpreted as a void function, while the
latter is designed to return one of the above basic types.

They have this type of syntax:

```
block myBlock () {
}

;;; No need for an 'explicit' return statement
;;; since this is already implicit in the arrow return
fn:int myReturnable() -> toReturn {
}
```

Update 8/6/16: now also have a `nil` return type, which is like `void` in other languages:

```
fn:nil myBlock () {
}
```

is equivalent to the 'block' example from above.

Both are called with the keyword `call`:

```
fn:int funk (a) -> toReturn {
    int 10 + a -> toReturn;
}

;;; A function whereby the return type is an array
fn:reals arrayFunc() -> result {
    reals(10) -> result;
    put 3.14 -> result(0);
}

block otherFunction () {
    ;;; Call returnable funk, put result into b
    call funk (5) -> b;

    ;;; should print out 15
    prn b;
}

block myFunction () {

    ;;; Print out hello and add newline
    prn "Hello";

    ;;; And without a newline:
    pr "Hello ";

    ;;; Variables are scoped to the function blockl
    int 5 -> a;

    ;;; A loop that executes statements within
    ;;; the enclosing braces several times.
    ;;; Note that since variables are scoped to the enclosing
    ;;; block, the variable b will be no longer be accessible
    ;;; after the loop
    repeat 5 times {
        int 1 -> b;
    }

    call otherFunction ();
}

;;; main entry point
start {
    ;;; Run sub-routine myFunction
    call myFunction ();
}
```

Loops
-----

Three types of loop are provided.
A while loop that has C-style syntax:

```
int 0 -> i;
while (i < 10) {
    put (i + 1) -> i;
}
```

A repeat loop. I think I saw this in POP-11 and I found it damn handy:

```
repeat 10 times {
   prn "hello!";
}
```

Finally a list or array element iterator:

```
;;; list example
list [this is a list] -> mylist;
for element in mylist {
    prn element;
}

;;; array example
reals(10) -> myarray;
for element in myarray {
    prn element;
}
```



If-else
-------
Conditional branching is c-style with the one exception that `if else` is not currently supported. Examples:

```
if (1 < 2) {
    prn "true";
}
if (2 < 1) {
    prn "true";
} else {
    prn "false";
}
```

I also later introduced some other syntax (either can be used) whereby `?` can be used in place of `if` and `:` can be used in place of `else`.
```
? (2 < 1) {
    prn "true";
} : {
    prn "false";
}
```

Handling input
--------------
Handling input is easy:
```
;;; start is the main entry point
start {
    ;;; query for user input
    input "What is your name? " -> name;
    string "Hello, " -> s;
    append (s, name) -> s;
    prn s;
}
```

More on lists
=============

Lists are containers of string elements that may contain nested lists:

```
list [] -> emptyList;
list [just a plain list] -> A;
list [Test a [nested list] list] -> L;
list [a very [very [even more nested] nested] nested list] -> Q;
```

List ^ and ^^ operators
-----------------------

(Note: these operators are directly inspired by POP-11s operators of the same syntax)

The `^` operator can be used to insert elements into lists. For example:

```
list [one two three] -> L;
string "inserted" -> str;
list [one two ^str three] -> L;
;;; list L is now [one two inserted three]

list [one two three] -> L;
list [nested bit] -> P;
list [one two three ^P] -> L;
;;; list L is now [one two three [nested bit]]
```

The `^^` operator can be used to flatten lists when inserting, e.g.:

```
list [one two three] -> L;
list [unnested bit] -> P;
list [one two three ^^P] -> L;
;;; list L is now [one two three unnested bit]
```

Can also be useful when one wants to append elements to the end of a list, e.g.:

```
list [one two three] -> L;
string "four" -> str;
list [five six] -> P;
list [^^L ^str ^^P] -> L;
;;; list L is now [one two three four five six]
```

List addition and removal of elements
-------------------------------------

A number of operators can be used to add elements to and extract elements from lists.

Setting tokens -- use the `put` or `put_token` command:

```
list [hello there] -> t;
put "goodbye" -> t(0);
;;; list t is now [goodbye there]

put [nested bit] -> t(0);
;;; list t is now [[nested bit] there]

set_token (1, [another list], "test") -> q;
;;; q is [another test]
```

Getting tokens -- use the `get` or `get_token` command:

```
;;; extract token 0, store in string t
get_token(0, [hello there]) -> t;

;;; make a new list, q
list [what the flip] -> q;

;;; extract token 1 from q and store in string r
get q(1) -> r;
```

Adding tokens (similar to `set_token`):

```
;;; add a token to the end of a list
list [a big list] -> L;
add_token("token", L);
```

Find the index of first matching token:

```
index_of ("hello", [hello there]) -> s;
```

Converting a list to a string:

```
list_to_string [hello there] -> s
```

Iterating over list elements:

```
list [this is a list] -> mylist;
for element in mylist {
    prn element;
}
```

Miscellaneous
-------------
Getting the type of a variable:

```
;;; suppose we need to figure out the type of a,
;;; use the type command and put result in a string.
int 1 -> a;
type a -> theTypeOfA;
```

Releasing a variable if we no longer need it:

```
real 1.1 -> d;
;;; some time later, after we're finished with d:
release d;
```

Generating random numbers:

```
;;; generate an int in the range [0, 5]:
random:int 5 -> resultA;

;;; generate a real in the range [0, 3.14]
random:real 3.14 -> resultB;
```
