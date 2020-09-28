# Disboard

Disboard is a c++ framework useful for studying sequential turn based games.
It includes an engine for implementing games and AIs that can play them.

## Engine
The engine is composed by a series of c++ interfaces and methods that can be used to implement sequential turn based games like Chess, Checkers or Go.
It is already predisposed with optimizations to exploit properties a game may have such as board symmetry.

## AI

Disboard includes two types of AIs, one based on the MiniMax algorhitm and one on the Monte Carlo Tree Search. They are game-independent and can play any game made using Disboard.
Both AIs have a sequential implementation and two parallel versions that use OpenMP, and are costumizable in terms of maximum thinking time, maximum depth and cache size.

## RNG

To ensure better results, the [PCG library](https://github.com/imneme/pcg-cpp) is used for RNG.

## Building

In order to buil Disboard, a compiler that supports c++17 and OpenMP is required.

Makefiles are provided for building the project on linux using g++.


## Examples

An implementation of the game ConnectFour is used as an example to show how to use Disboard for creating a game.

Two examples files, play.cpp and benchmarks.cpp, can be modified to play against AIs or test them.