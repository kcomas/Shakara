# Shakara

A programming langauge made as a simple toy project.
Reference interpreter implemented in C++.

### Dependencies

At this point, nothing aside from the standard library.

### Building

The Visual Studio 2017 solution file is provided for building the project.

### Tests

Also included with the Visual Studio 2017 solution file is a VC++ Native Test
project, with tests wired up to all of the important pieces of the reference
interpreter.

These tests are decoupled from the actual interpreter library and can be safely
discarded if you're using the library, or looking to compile the command line
version of the library for usage.

Though, despite the tests not required for library *usage*, they are required for
library *contribution*. Thus, if working on Shakara on your own, you must be sure
that your code doesn't break any of the existing tests.

In addition, with regards to tests, if you feel there is something that is not
completely represented within the tests, feel free to add to them, as long as it
makes them more robust.

### Language Examples

Some examples of the language can be found in the
[Examples](https://github.com/tinfoilboy/Shakara/tree/master/Examples) folder
in the repo. Though, below you can find an example of FizzBuzz written
in the language.

    fizzbuzz = (number)
    {
        buzzer = ""

        if (number % 3 == 0)
            buzzer += "Fizz"

        if (number % 5 == 0)
            buzzer += "Buzz"

        if (buzzer == "")
            return number
			
        return buzzer
    }

    count = 1

    while (count <= 100)
    {
        print(fizzbuzz(count), " ")

        count++
    }

### Why "Shakara"

[See this song.](https://www.youtube.com/watch?v=vvYxd35xFx8)
