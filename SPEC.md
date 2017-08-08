# Shakara Specification

Shakara is a dynamically typed toy scripting
language created for fun.

Use in your own projects at your own risk!
Due to this being a toy project, it may
be super insecure and prone to bugs.

### Important Constructs

With Shakara being dynamically typed, the programmer
does not deal with types, at all. Not once.
Of course, should be **mindful** of what types you are
using, but you can't explicitly set types.

Additionally, there are no semicolons, or any type of
terminator for statements in Shakara, just finish a
statement and make a newline, or a space, and volia,
new statement.

### Integers

Integers are by default 32-bit, and are specified with just
a single number without a decimal point anywhere.

### Decimals

Decimals in Shakara are a floating point number, defined by
specifying any number with a decimal point.

### Strings

Strings in Shakara are specified by wrapping characters in two
double quotes.

###### Escape Sequences

Shakara strings, like many other languages, support escape sequences
to use characters that could be seen by the parser, or otherwise would
be parsed.

The ones supported are in the table below.

| Sequence  |  Result      |
| --------- | ------------ |
| \\n       | newline      |
| \\\       | backslash    |
| \\"       | double quote |

### Arithmetic

Shakara does arithmetic expressions in the most
straightforward way possible.
This way is through parsing each individual portion
of a expression in order.

This means, if you type `1 * 6 * 7 / 6 * 9`,
you will get `63`. I may add operator precedence
at some point, or order expressions like an elementary
math expression, but not as of now.

Also supported are the operators as follows: `+=`, `-=`,
`*=`, `/=`, `++`, and `--`. Which function the same as
languages like C or C++.

### Variables

Since Shakara is a dynamically typed language, you don't
have to specify a single type when defining a variable.
In fact, you cannot specify types, simplifying writing
code significantly.

Defining a variable uses this very simple syntax: `<variable_name> = (value)`. That's all.

### Functions

Defining functions in Shakara follows the same rough
syntax of defining variables, with some syntax akin
to say, C/C++.

Below is the syntax for function definition:

    <function_name> = (<arguments>)
    {
        (body)
    }

Functions are defined using the equals sign and then the arguments
as a way to mirror assignment of actual variables (Also, I felt it
was a little more straightforward to parse at that point).

Then, after being defined, to call a function, use the line
below:

    <function_name>(<args>)

### Printing

Printing is a built in call in the language, no imports required
or anything of the sort.

Print statements follow the style of a function call, usage would
be like the code below:

    print(type, other_type)

Print supports as many arguments as you need, and these arguments
will either be printed by their value, or if their value cannot be
determined, it will print the current type of the variable.

Print automatically appends a newline at the end of the statement,
so there is no need to use one.

### Return

Returning can be used within a method to, of course, return some
kind of value.

Return is roughly the same as languages like C/C++ or even Python.
The code for using a return is like below

    counter = (count)
    {
        return count + 1
    }

Return cannot be used outside of a function.

### If Statements

If statements are like any other C-like language, with the following syntax:

    if (condition)
    {
        block
    }

Also, like many other C-like languages, braces in if statements can be omitted,
but then only one statement is allowed in the body.

    if (condition)
        statement