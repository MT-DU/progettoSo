# Space Defenders

Authors: [Matteo Tuzi](https://www.github.com/MatteoTuzi) & [Davide Uccheddu](https://www.github.com/barux)
***

## Functional Analysis

The assignment asked to develop a game inspired by the famous _Space Invaders_ and _Defenders_. The project expects the implementation of a game menu, different game difficulties, and the game itself. Through the menu the user can start the game, choose the game mode or quit the game.

## Design decisions

- Ally starship bullets can bounce off the walls.
- User can choose number of enemies in the main menu.
- Ally starship have multiple health points based on selected difficulty.
- User can customize difficulty of the game.

Those decisions were made to make the game more fun and challenging, in particular for the bouncing bullets mechanics.

## How the game works

The user can control an ally starship which can move only vertically in the left part of the screen; it starts in the middle left of the screen with variable health points based on choosen difficulty: 3 hp for easy, 1 hp for hard, if custom difficulty is selected health points are set to 3 if number of enemies is lower than 3, 1 hp otherwise.

The ally starship can fire 2 bullets diagonally when user presses the _space bar_ key: when bullets hit upper or lower walls they bounce back, when they hit an enemy or the right border they disappear.

In the right part of the screen there are 3 enemies if game mode is easy, 9 if hard, up to 12 if custom, 3 per column. Enemy starships move together following a specific pattern (every 5 vertically moves, they move horizontally once). Every time _rand()%10_ is equal to 0, enemy starship shoots a bomb that moves from right to left until  hit ally starship or left border.

When an enemy starship is hit, in its place spawns a level 2 enemy starship with a different sprite and 2 health points, so the enemy starship has 3 total health points.

## End of the game

The game ends if:

- Enemy starship reaches the left border where ally starship can move (defeat);
- Ally starship is hit N times by enemy starship where N is number of ally health points (defeat);
- All the enemy starships are destroyed (victory).

## Technical Analysis process version

The process version is based on pipes, a IPC unidiretional mechanism that allows to imparented processes to communicate with each other. This project is based on a _printObjects_ function that uses read system call to get objects (consumer) from child processes (producers).

Used functions/procedures:

- **void allyShipController** : it manages ally starship movements and firing bullets; when user presses the _space bar_ key, it fires 2 bullets diagonally and set two variables (isBulletShot and canShoot) to true to indicate that bullets are fired, implemented so player can shoot only two bullets at a time; those variables are set to false only if bullets process ends.
There is also a zombie processes cleaning thanks to waitpid with WNOHANG option. The entire algorythm is inside a while loop that continuously write ally starship position in the pipe.
- **void bulletController** : its purpose is to write bullets position in the pipe and manage wall bounces: when it reaches right border the loop ends, when it reaches upper or lower wall bounces back, changing its direction from _DOWN_DIRECTION_ to _UP_DIRECTION_ or vice versa.
- **void enemyShipController** : it manages enemy starship movements and writing position in the pipe; when _rand()%10_ is equal to 0, a fork is executed to create bomb process. Like the ally function, there is a zombie process cleaning with waitpid and a boolean flag _generateBomb_ to allow only one existing bomb at a time for each enemy.
- **void bombController** : it manages bomb movements and writing position in the pipe; when it reaches left border the loop ends.

Here a graphical representation of the process version:

![](/Documents/Analisi.png)

## Technical Analysis threads version

The threads version is based on global variables for communication between threads, taking advantage of shared memory; a common issue of shared memory synchronization is critical section management which is solved with mutexes in this project. All threads are initialized with pthreads_create and ptread_detach, in order to automatically free resources after they end their execution.

Used functions/procedures:

- **void mainGame** : it manages game start: create ally and enemy starships threads based on difficultyMode.numAliens variable; then all buffers, global and state variables are initialized. After printing loop ends, all resources are deallocated and any running thread is terminated.
- **void\* allyShipController** : it manages ally starship movements and firing bullets; during initialization a mutex is used to avoid memory consistency errors. When user presses the _space bar_ key, 2 threads are created and associated to bulletController function. Like process version, there are controls to avoid multiple bullets spawn.
- **void\* bulletController** : it manages bullets movements; an argument is passed to this function to access the _bullets_ buffer. Every coordinates update is managed by mutex lock and unlock.
- **void\* enemyShipController** : this function is assigned to N thread (N is number of enemies). The argument idNumberT is an id used to access the right buffer cell. Like the ally function, a mutex is used to avoid memory consistency errors. The movement pattern is the same as process version but updated inside a mutex lock unlock section. When _rand()%10_ is equal to 0 and there aren't any existing bombs with same idNumberT, a thread is created and associate to bombController.
- **void\* bombController** : it manages bomb movements; an argument is passed to this function to access the _bombs_ buffer and identify the alien who created the bomb. Every coordinates update is managed by mutex lock and unlock.
- **EndGame printObject** : this function is a loop that manages printing game and collision between objects, while gameStatus variable is set to _CONTINUE_, return to caller function if gameStatus is equal to _VICTORY_ or _DEFEAT_, returning that value.
- **void checkCollision** : this procedure check collisions and manages game status. If a collision between ally and enemy starship is detected, the game ends and gameStatus is set to _DEFEAT_. If a collision between ally starship and bomb or enemy starship and bullet is detected, the bullet/bomb thread is killed and the starship lose a health point; if it reaches 0, also starship thread is terminated and its status is set to __OBJ_DEAD__.
- **EndGame isGameOver** : this function verify if game is over checking the number of enemies alive and ally starship health points and return the right value.

Here a graphical representation of the process version:

![](/Documents/AnalisiThread.png)

***

## License

Copyright 2022 Matteo Tuzi & Davide Uccheddu

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.