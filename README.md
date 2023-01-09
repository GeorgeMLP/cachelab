# Cache Lab

## Introduction

ICS Cache Lab, Peking University.

This lab will help you understand the impact that cache memories can have on the performance of your C programs.

The lab consists of two parts. In the first part you will write a small C program (about 200-300 lines) that simulates the behavior of a hardware cache memory. In the second part, you will optimize a small matrix transpose function, with the goal of minimizing the number of cache misses.

For more information about this lab, please refer to ```cachelab.pdf```.

## Installation

It is recommended to do this lab on Ubuntu 22.04.

Before getting your hands dirty, a Linux program called ```valgrind``` will be doing you a great deal of things. You can type the following command to install ```valgrind```.
```
sudo apt install valgrind
```

## Score

My score for this lab is as follows.
| Total score | Total misses | Misses (32×32) | Misses (64×64) | Misses (60×68) |
| ----------- | ------------ | -------------- | -------------- | -------------- |
| 53.0        | 2682         | 256            | 1024           | 1402           |

The miss counts of transposing 32×32 and 64×64 matrices are **theoretically optimal**. The best miss count of transposing a 60×68 matrix in my class is 1382.

## Auto-search Tool

I have written a tool in ```parameter auto-search.cpp``` to automatically calculate the miss counts of different transpose methods. All you have to do is to write your transpose function in ```transpose_submit``` using ```load``` and ```store```.

If you have some parameters in your transpose function, you can search for the best parameters automatically using this tool. I have given an example in ```parameter auto-search.cpp```. Moreover, this tool enables more advanced approach, such as heuristic search and even neural networks.

Another powerful feature of this tool is that by setting ```print_codes = true```, it prints out the corresponding C code of your method, which you can paste to ```trans.c``` and submit directly. This feature may come in handy if you are using more advanced approaches.