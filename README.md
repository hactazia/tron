# Tron Game

## Description

Tron is a simple game implemented in C. The game can be run using either SDL or ncurses for rendering.

## Build

This source code is for linux only.
The build is required to have the SDL2 and ncurses libraries installed.
To build the game, follow the steps below:

1. Clone the repository:
    ```sh
    git clone https://github.com/hactazia/tron.git
    cd tron-game
    ```
2. Install the necessary dependencies:
    - For SDL:
        ```sh
        sudo apt-get install libsdl2-2.0-0 libsdl2-dev
        ```
    - For ncurses:
        ```sh
        sudo apt install libncurses5 libncurses5-dev
        ```
3. Build the game:
    ```sh
    mkdir build
    cd build
    cmake ..
    make
    ```
4. Run the game:
    - For SDL:
        ```sh
        ./tron -sdl
        ```
    - For ncurses:
        ```sh
        ./tron -ncurses
        ```
5. Enjoy the game!

## Usage

- Player 1 controls: Arrow keys
- Player 2 controls: ZQSD
- Player 3 controls: IJKL
- Player 4 controls: TFGH
- Player 5 controls (numpad): 5123
- Player 6 controls (numpad): -789

### Ncurses specific

- Start the game: *
- Quit or Cancel: =
- Add a player: +
- Remove a player: -

### SDL specific

Your can add or remove players in the Options menu.

## Credits

This game was created by [hactazia](https://github.com/hactazia) 
and [Figy-exe](https://github.com/Figy-exe).