# Disboard

Disboard is a c++ framework useful for studying sequential turn based games.
It includes an engine for implementing games and AIs that can play them.

## Engine
The engine is composed by a series of c++ interfaces and methods that can be used to implement sequential turn based games like Chess, Checkers or Go.
It is already predisposed with optimizations to exploit properties a game may have such as board symmetry.

## AI

Disboard includes two types of AIs, one based on the MiniMax algorhitm and one on the Monte Carlo Tree Search. They are game-independent and can play any game made using Disboard.
Both AIs have a sequential implementation and two parallel versions that use OpenMP, and both are costumizable in terms of maximum thinking time, maximum depth and cache size.

## RNG

To ensure better results, the [PCG library](https://github.com/imneme/pcg-cpp) is used for RNG.

## Building

In order to build Disboard, a compiler that supports c++17 and OpenMP is required.

Makefiles are provided for building the project on linux using g++ by simply using the `make` command in the main folder.


## Examples

An implementation of the Connect4 game is used as an example to show how to use Disboard.

Two examples files, play.cpp and benchmarks.cpp, can be modified to play against AIs or test them.

The play.cpp file can be easily edited to set the AI type and the difficulty. Both MonteCarloPlayer and MiniMaxPlayer classes have a two paramenters constructors. The second parameter `verbose` specifies wether the AI should print info about the taken decision. The first parameter can be used to set the strenght of the AIs.

Suggested difficulty settings:
 - Easy: MonteCarloPlayer(100), MinMaxPlayer(6)
 - Medium: MonteCarloPlayer(1000), MiniMaxPlayer(8)
 - Hard: MonteCarloPlayer(5000), MiniMaxPlayer(10)
 
Developed by Paolo D'Alessandro and Luca Calderini for the HPC course at Unipr
