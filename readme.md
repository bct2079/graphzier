# Graph Plotter in C

This is a simple Graph Plotter implemented in C that allows you to visualize mathematical functions on a graph. The plotter uses the Cairo graphics library to render the graph on the screen.

## How to Use

1. Compile the code using a C compiler, such as GCC and execute.
   
   ## For Linux or Window (use Bash based command line and MSYS2)
   ```bash
   ./runner.sh
   ```

# Input the mathematical expression for the function you want to plot.

For example, you can input expressions like:

+ sin(x)
+ exp(x)
+ add(sin(x), cos(x))

The Graph Builder will compile the input expression using the Math Interpreter and plot the graph on the screen using the Cairo graphics library.

# Supported Functions
### The interpreter supports the following mathematical functions:
+ add: add to number `add(2, 3)` 
+ minus: as usual `minus(2, 3)`
+ divide: as usual `divide(6, 3)`
+ multiply: as usual `multiply(2, 3)`
+ power: power of something `power(2,3)` => 8 
+ sin: Sine function. `sin(3.14/3)`
+ cos: Cosine function.
+ tan: Tangent function.
+ log: Natural logarithm function (base e).
+ log10: Logarithm base 10.
+ exp: Exponential function (e^x).
+ logBase: Logarithm with a specified base (logarithm of the second argument with base of the first argument).

# Example
+ `sin(3)` - Computes the sine of 3.
+ `add(sin(3), 4)` - Adds the sine of 3 and 4.
+ `mul(2, add(sin(3), 4))` - Multiplies 2 with the result of adding the sine of 3 and 4.
+ `logBase(10, 100)` - Calculates the logarithm base 10 of 100.

# Supported Variables
The interpreter supports a variable x that you can use in your expressions.

# Important Note
Since this interpreter is designed for basic math expressions, it may not handle complex expressions, operator precedence, or nested function calls in certain cases. Additionally, it doesn't support advanced mathematical operations like calculus or symbolic manipulation.

# Author
This interpreter was created by Rabin Lamichhane.
   
