# exengine - Exchange engine for one good


> Exengine is a quite primitive trading backend single-product trading.


## Usage 

Build the source and start the program. It reads standard input stream and produces standard output.

Exengine will waiting for two types of commands:

1. Place new order - comma separated tokens (for example: O,1,S,35,199.99).
2. Cancel order - comma separated tokens (for example: C,1)

The meaning of **place new order** command is:

* (O)rder,id,(S)ell,quantity,price
* (O)rder,id,(B)uy,quantity,price

Field "id" must be a global counter of unique increasing numbers for both types of order placement (sell order 1, buy order 2, sell order 3, ... and so on).

The meaning of **cancel order** command is:

* (C)ancel,id

When there are sell and buy orders with relevant price, program will make the deal and print the trade record to standard output.


### Example:

Imagine that you are trading a new cryptocurrency - Super Crypto Coin (SCC). Only coins of the same type are available and cannot be split into less than one coin.
The market price of the SCC coin ranges from $250 to $300.

Let's trade.

**StdIn:**


    O,1,S,23,275.77
    O,2,S,93,275.10
    O,3,S,8,293.61
    O,4,S,31,292.84
    O,5,S,16,275.12
    O,6,S,17,296.69
    O,7,B,10,290.84
    O,8,S,55,264.63
    O,9,B,57,265.27

*7 orders for sell and 2 orders for buy on the exchange.*

**StdOut:**


    T,1,S,2,7,10,275.1
    T,2,S,8,9,55,264.63

*2 trade deals were made. Order #2 sold 10 coins to order #7 at $275.1 per coin. And order #7 has been fully cleared from the exchange. After that, order #8 sold 55 coins to order #9  at price $264.63 per coin and clearly closed.*

If the program receives a cancel order command, then the order with this id removes from the stack and prints a cancel record.


**StdIn:**


    C,3


**StdOut:**


    X,3

*Order #3 removed from the stock.*

## Purpose

A simple, non-bloated, error-free solution of exchange engine with maximum performance.

## Achieved

- Runs ~120x faster then its [prototype](https://github.com/edyatl/exengine_py) in Python.

## Algorithm and approach


1. Read input and split tokens into an array.  
 
2. Two different stacks are created for buy_orders and sel_orders. The stack structure is used to store orders as the simplest and most efficient structure.

3. One item of the stack consists of three fields: id, quantity, price. The total size of one stack item is 12 bytes. For each stack, an array of 1024 items is reserved by default. (You can change this capacity by changing the CAPACITY constant at the head of c file.) 

4. Dynamic memory allocation is not used because static memory is much faster and gives us less room for memory errors. 

5. There is no particular field for the trading side (buy or sell), but the side is determined by the stack. 

6. The price is stored as a float, obviously, this is not a good type for money field, but in this task price is static and is only used  for comparison operations. Therefore, I think that in this case it is permissible to use the float type for simplicity.

7. Each time a new order is added, the trade function performs trades between buy and sell orders in recursion. If a buy order stack and a sell order stack exist, it determines the max and min orders by price, and then check if  their prices are suitable for the trade deal. If so order with less quantity removes and the quantity of the remaining order is reduced by the transaction amount. If both orders have the same quantity, then the second order is also deleted.

8. The order is removed from the trade stack by assigning the values of the next element to the removed item of the stack, and so on along the chain to the top of the stack. And then the top of the stack is shifted down one item.

9. Orders are traded and removed according to FIFO method (First In First Out).

10. Also don't be confused by one alien crutch - `pyprint_float()` function, which is just for printing floats in Python style. It adds `.0` if the number has no digits after the decimal separator. The first output file for the tests was the result of a Python program, that's why this function was appeared. 

## To do

- Make protection from going into infinite recursion.
- Solve the issue with compilation with optimization.
- Try to make a port in Cython  to compare performance.

## Known issues

With gcc 4.8.4 and gcc 5.4.0 when use optimization (-O flag) result binary produces incorrect output of trading records. 

And gcc 10.2.0 with -O2 works like a charm.


## Contributing

Any ideas, patches, bug reports and so on are always welcome.


## Authors

Yevgeny Dyatlov ([@edyatl](https://github.com/edyatl))


## License

This project is licensed under the MIT License.

Copyright (c) 2021 Yevgeny Dyatlov ([@edyatl](https://github.com/edyatl))

Please see the [LICENSE](https://github.com/edyatl/exengine/blob/master/LICENSE) file for details.