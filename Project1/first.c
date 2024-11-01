#include <stdio.h>
#include <stdlib.h>
#include <conio.h> 
#include <time.h>  
#include <windows.h> 

#define WIDTH 30 
#define HEIGHT 10 
#define BULLET_SPEED 1 
#define CYCLE_SPEED 15

int playerX, playerY;
int bulletX, bulletY;
int cycleNum = 0;
time_t LastCycleTime;

void gotoxy(int x, int y) {
    COORD pos = { x, y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void initGame() {
    playerX = WIDTH / 2; // Initialize player's X position
    playerY = HEIGHT / 2; 
    bulletX = WIDTH - 1;  // 기본은 Cycle 0으로 설정
    bulletY = rand() % HEIGHT;

    LastCycleTime = time(NULL);

    // 15초가 지날 때까지 반복
    while (1) {
        
    }
}

void drawGame() {
    gotoxy(0, 0);
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (x == playerX && y == playerY) {
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
        else if (input == 'a' && playerX > 0) {
            playerX--;
        }
        else if (input == 'd' && playerX < WIDTH - 1) {
            playerX++;
        }
    }

    // 현재 시간을 얻어서 15초 경과 여부 확인
    time_t currentTime = time(NULL);
    
    // 현재 시간이 시작 시간 + 15초 이상이면 종료
    if (difftime(currentTime, LastCycleTime) >= CYCLE_SPEED) {
        cycleNum = rand() % 4; // 사이클 설정
        LastCycleTime = currentTime;

        switch (cycleNum) {
            case 0: // 오른쪽에서 발사
                bulletX = WIDTH - 1;
                bulletY = rand() % HEIGHT;
                break;
            case 1: // 왼쪽에서 발사
                bulletX = 0;
                bulletY = rand() % HEIGHT;
                break;
            case 2: // 위쪽에서 발사
                bulletX = rand() % WIDTH;
                bulletY = 0;
                break;
            case 3: // 아래쪽에서 발사
                bulletX = rand() % WIDTH;
                bulletY = HEIGHT - 1;
                break;
        }
    }

    switch (cycleNum) {
        case 0: // 오른쪽에서 발사
            bulletX -= BULLET_SPEED;

            if (bulletX < 0) {
                bulletX = WIDTH - 1;
                bulletY = rand() % HEIGHT;
            }
            break;
        case 1: // 왼쪽에서 발사
            bulletX += BULLET_SPEED;

            if (bulletX >= WIDTH) {
                bulletX = 0;
                bulletY = rand() % HEIGHT;
            }
            break;
        case 2: // 위쪽에서 발사
            bulletY += BULLET_SPEED;

            if(bulletY >= HEIGHT) {
                bulletX = rand() % WIDTH;
                bulletY = 0;
            }
            break;
        case 3: // 아래쪽에서 발사
            bulletY -= BULLET_SPEED;

            if (bulletY < 0) {
                bulletX = rand() % WIDTH;
                bulletY = HEIGHT - 1;
            }
            break;
    }

    if (bulletX == playerX && bulletY == playerY)
        {
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
