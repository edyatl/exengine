# exengine - Exchange engine for one good


> Exengine is a quite primitive trading backend for one good.

## Usage 

Build source and start program. It reads standard input stream and produces standard output.

Exengine will waiting for two types of commands:

1. Place new order - comma separated tokens (for example: O,1,S,35,199.99).
2. Cancel order - comma separated tokens (for example: C,1)

The meaning of **place new order** command is:

* (O)rder,id,(S)ell,quantity,price
* (O)rder,id,(B)uy,quantity,price

Field "id" must be global counter of unique increasing numbers for both types of orders placements (sell order 1, buy order 2, sell order 3, ... and so on).

The meaning of **cancel order** command is:

* (C)ancel,id

When there are sell and buy orders with relevant price, program will make deal and print trading record to standard output.

### Example:

StdIn:


    O,1,S,23,275.77
    O,2,S,93,275.10
    O,3,S,8,293.61
    O,4,S,31,292.84
    O,5,S,16,275.12
    O,6,S,17,296.69
    O,7,B,10,290.84
    O,8,S,55,264.63
    O,9,B,57,265.27


StdOut:


    T,1,S,2,7,10,275.1
    T,2,S,8,9,55,264.63

If program receive cancel order command then order with that id removes from stack, and prints cancel record.

### Example:

StdIn:


    C,3


StdOut:


    X,3

## Algorithm and approach


1. Read input and split tokens to an array.  
 
2. Make two different stacks for buy_orders and sel_orders. The stack structure is used to store orders as most simple and efficient structure. 

3. One item of stack consists from three fields: id, quantity, price. Total size of one stack item is 12 bytes. For every stack by default reserved an array of 1024 items. (You can change this capacity by changing CAPACITY constant at the head of c file.) 

4. No dynamic memory allocation used because static memory much faster and give us less opportunists to make errors with memory. 

5. There is no particular field for trading side (buy or sell), but the side determines by stack. 

6. Price stored as float, obviously, this is not a good type for money field, but in this task price is static and is used only for comparison operations. So I think that in this case it is permissible to use float type for simplification.

7. Every time then adds new order, a trading function goes after and makes trade deals between buy and sell orders in recursion. If buy orders stack and sell orders stack exists it define max and min orders by price, and then check is their prices suitable for trade deal. If so order with less quantity remove and quantity of keep order decrees for deal amount. If both orders have the same quantity then second order also deletes.

8. Order removes from trading stack by assigning values of next stack item and so on by chain until top of stack. And then top shift for one item down.

9. Also don't be confused by one alien crutch - `pyprint_float()` function which is just for printing floats in Python style. It adds `.0` if figure has no digits after decimal separator. The first output file for tests was result of Python program that's why this function appeared. 

## To do

- Make protection from going into infinite recursion.
- Solve an issue with optimization compilation.

## Known issues

With gcc 4.8.4 and gcc 5.4.0 when use optimization (-O flag) result binary produces incorrect output of trading records. 

And gcc 10.2.0 with -O2 works fine.

## Authors


Yevgeny Dyatlov ([@edyatl](https://github.com/edyatl))


## License


This project is licensed under the MIT License.

Copyright (c) 2021 Yevgeny Dyatlov ([@edyatl](https://github.com/edyatl))

Please see the [LICENSE](https://github.com/edyatl/exengine/blob/master/LICENSE) file for details.