# exengine - Exchange engine for one good


> Exengine is a quite primitive trading backend for one good.

## Usage 

Build source and start program. It reads standard input stream and produces standard output.
It will waiting for two types of commands:

1. Place new order - comma separated tokens (for example: O,1,S,35,199.99).
2. Cancel order - comma separated tokens (for example: C,1)

The meaning of place new order command is:

* (O)rder,id,(S)ell,quantity,price
* (O)rder,id,(B)uy,quantity,price

Field "id" must be global counter of unique increasing numbers for both types of orders placements (sell order 1, buy order 2, sell order 3, ... and so on).

The meaning of cancel order command is:

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

## Algorithm


1. text in progress...

## To do

## Known issues

With gcc 4.8.4 and gcc 5.4.0 when use optimization (-O flag) result binary produces incorrect output of trading records. 

And gcc 10.2.0 with -O2 works fine.

## Authors


Yevgeny Dyatlov ([@edyatl](https://github.com/edyatl))


## License


This project is licensed under the MIT License.

Copyright (c) 2020 Yevgeny Dyatlov ([@edyatl](https://github.com/edyatl))

Please see the [LICENSE](https://github.com/edyatl/exengine/blob/master/LICENSE) file for details.