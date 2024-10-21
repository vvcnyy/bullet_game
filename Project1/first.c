#include <stdio.h>
#include <stdlib.h>
#include <conio.h> 
#include <time.h>  
#include <windows.h> 

#define WIDTH 30 
#define HEIGHT 10 
#define BULLET_SPEED 1 

int playerY;
int bulletX, bulletY;

void gotoxy(int x, int y) {
    COORD pos = { x, y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void initGame() {
    playerY = HEIGHT / 2; 
    bulletX = WIDTH - 1; 
    bulletY = rand() % HEIGHT;
}

void drawGame() {
    gotoxy(0, 0);
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (x == 0 && y == playerY) {
                printf("P");
            }
            else if (x == bulletX && y == bulletY) {
                printf("B");
            }
            else {
                printf(" ");
            }
        }
        printf("\n");
    }
}

void updateGame() {
    if (_kbhit()) {
        char input = _getch();
        if (input == 'w' && playerY > 0) {
            playerY--;
        }
        else if (input == 's' && playerY < HEIGHT - 1) {
            playerY++;
        }
    }

    bulletX -= BULLET_SPEED;

    if (bulletX < 0) {
        bulletX = WIDTH - 1;
        bulletY = rand() % HEIGHT;
    }

    if (bulletX == 0 && bulletY == playerY) {
        gotoxy(0, HEIGHT);
        printf("Game Over!\n");
        exit(0);
    }
}

int main() {
    srand(time(NULL));
    initGame();

    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(consoleHandle, &cursorInfo);
    cursorInfo.bVisible = 0;
    SetConsoleCursorInfo(consoleHandle, &cursorInfo);

    while (1) {
        drawGame();
        updateGame();
        Sleep(10);
    }

    return 0;
}
