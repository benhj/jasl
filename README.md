JASL: Just Another Scripting Language
-------------------------------------

Why? I read somewhere that every programmer worth his salt should attempt to write their own programming language. This is my attempt :-)

### The basics

Every JASL script has a main entry point. I call this 'start':

<pre>
// start is the main entry point
start {
    echo_nl("Hello, world!");
}
</pre>

The above fragment will print out what you think it will print out when run with `./jasl hello.jasl'.

### Program output

So I should probably show these echo commands. We basically have two of them:

<pre>
echo("Hello"); 

// or with a new line
echo_nl("Hello");

// can also print out the result of math expressions
echo_nl((3*6) + (1*5));
</pre>

### Variables
In JASL, we have 64 bit signed integers, doubles and strings:
<pre>
// the following all do the same thing:
var(int, a, 1);
int a = 1;
integer(1, a);

// as do the following
var(double, d, 2.2);
double d = 2.2;
decimal(2.2, d);

// for strings we simply have
string("Hello!", s);
</pre>

### More on strings

JASL features a number of string utility functions:

<pre>
// create s rting and reverse it
string("Hello!", s);
string_reverse(s);

// will print out "!olleH"
echo_nl(s); 

// now put back to correct "Hello!"
string_reverse(s);
string(" World!", w);
string_append(s, w);

// will print out "Hello! World!"
echo_nl(s);

// get length of string
string_length(s, len);

// print out 13
echo_nl(len);
</pre>

### Functions

In JASL, fUnctions are really subroutines. I call them blocks:

<pre>
block foo {
    echo_nl("In foo");
}

// entry point
start {
    // call block foo
    call foo;
}
</pre>

### Loops
Currently, two loops are supported:

<pre>
// print hello 5 times:
repeat 5 time {
    echo_nl("Hello");
}

// or using a while loop
var(int, a, 1);
while (a < 5) {
    echo_nl("Hello");
    var(int, a, (a + 1));
}
</pre>

### Program flow
TODO
