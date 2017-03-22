# The Inger Programming Language
## What is Inger?

Inger is a simple imperative programming language, much like C (only somewhat cleaner). Inger was developed to illustrate the process of building a compiler from scratch, and writing a book about it which is freely available in the `book` folder. Over time, Inger has grown beyond our expectations and is approaching a level where we can actually write simple games with it.

Inger has full C-interoperability (it can call C-functions, provided that you write an Inger header file that describes the C-function, just like you would do in C) and work is underway to write header files for important operating system libraries (both Windows and Linux).

## Project Origins

The development of the Inger compiler started off as a project at the University of Arnhem and Nijmegen (The Netherlands), aimed at taking a closer look at the back end of a (possibly new) compiler (the code generation part). Eventually we started developing our own little language, inspired by the symbolic machine language C. 

## The Book
The primary goal was always to write a book that describes the process of writing a compiler in C, with lots of practical example. In the end, Inger has become the biggest practical example of all. The following people have contributed to the Inger compiler and the Compiler Construction - A Practical Approach book:

* Frank Benders
* Marco Devillers
* Dennis Meffert
* Jan-Willem Haaring
* Thijs Janssen
* Alexander van Oostenrijk  vanoostenrijk [@] hotmail [.] com

## What Next?
The book would be a good starting point for implementations in different languages to show how it could be done: fun! 
The Inger Project could continue its legacy with an initiative in the same vein as [Make a Mal Lisp.](https://github.com/kanaka/mal). Simply add your implementation in the implementations folder.

## Disclaimer
The Inger compiler project is by no means complete. We’ve written a book that can get you started with building your own compiler, but the Inger compiler itself is still only a sample. It cannot be used to write real-world, full-fledged programs with, partly because the language is not as sophisiticated as, say, C++, and partly because we are very sure there are a number of bugs to be ironed out. In fact, now that version 0.3 is complete we intend to rewrite major parts of the code, applying the knowledge we have acquired so far. You’re welcome to play around with the Inger compiler as much as you like, or even contribute to it, but it is not advisable to use it for writing real programs yet.


