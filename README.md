# Disboard

Disboard is an engine for sequential turn based games written in c++.

It includes a collection of interfaces and methods that can be inherited to build games.

The engine also comes with two types of AIs, one based on the MiniMax algorhitm and one on the Monte Carlo Tree Search.
These AIs have both a sequential implementation and two parallel versions that use OpenMP and are costumizable in terms of maximum thinking time, maximum depth and cache size.

These AIs are game-independent and can play any sequntial turn based game made using Disboard.

## Building

In order to buil Disboard, a compiler that supports c++17 and OpenMP is required.

Makefiles are provided for building the project on linux using g++.

## Examples

An implementation of the game ConnectFour is used as an example to show how to use Disboard for creating a game.

Two examples files, play.cpp and benchmarks.cpp, can be modified to play against AIs or test them.