# Gravity Balls

A very small game written in C using [raylib library](https://github.com/raysan5/raylib)

## Game description

Throw colourful balls that bounce off the walls and floor, pretty much like you'd throw Angry Birds.

<img src="https://github.com/temaxuck/Gravity-Balls/blob/main/demo/demo.gif">

## Demo

You can try the game [here](https://temaxuck.github.io/Gravity-Balls/).

## Build from source

To build the game on your local machine it is required to have c compiler and raylib library with all it's prerequisites installed (see [raylib docs](https://github.com/raysan5/raylib?tab=readme-ov-file#build-and-installation) how to achieve this). 

Then you can build simply like this:

```shell
cc game.c -lraylib -lm -o ./build/game
```

And then run it:

```shell
./build/game
```

## Pre-builds

Currently only [web](https://temaxuck.github.io/Gravity-Balls/) and Linux x86-64 prebuilded versions are available (see [build/](https://github.com/temaxuck/Gravity-Balls/tree/main/build) directory or [releases](https://github.com/temaxuck/Gravity-Balls/releases) page).

## Patch notes

**v1.2.1**

**Updates**

- Gravity was tweaked for a slightly better feel

**Fixes**

- Optimized the way objects are stored in memory
