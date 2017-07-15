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

### Arithmetic

Shakara does arithmetic expressions in the most
straightforward way possible.
This way is through parsing each individual portion
of a expression in order.

This means, if you type `1 * 6 * 7 / 6 * 9`,
you will get `63`. I may add operator precedence
at some point, or order expressions like an elementary
math expression, but not as of now.

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

Then, after being defined, to call a function, use the line
below:

    <function_name>(<args>)