#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>
#include <windows.h>

#define WIDTH 40             // 게임판 가로 크기
#define HEIGHT 25            // 게임판 세로 크기
#define BULLET_SPEED 1       // 총알 속도
#define MAX_BULLETS 40       // 동시에 발사되는 총알 개수
#define FRAME_DELAY 10       // 프레임 지연(단위: 밀리초)

typedef struct {
    int x;
    int y;
    int direction; // 0: 왼쪽, 1: 오른쪽, 2: 위, 3: 아래
    int active;    // 총알 활성화 상태
} Bullet;

int playerX, playerY;
Bullet bullets[MAX_BULLETS];

// 커서를 특정 위치로 이동
void gotoxy(int x, int y) {
    COORD pos = { x, y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

// 게임 초기화
void initGame() {
    playerX = WIDTH / 2;
    playerY = HEIGHT / 2;
    for (int i = 0; i < MAX_BULLETS; i++) {
        bullets[i].active = 0; // 총알 비활성화
    }
}

// 화면 그리기
void drawGame(char screen[HEIGHT][WIDTH]) {
    // 화면 초기화
    for (int y = 0; y < HEIGHT; y++)
        for (int x = 0; x < WIDTH; x++)
            screen[y][x] = ' ';

    // 플레이어 위치 표시
    screen[playerY][playerX] = 'O';

    // 활성화된 총알 표시
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            screen[bullets[i].y][bullets[i].x] = '-';
        }
    }

    // 화면 출력
    gotoxy(0, 0);
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            putchar(screen[y][x]);
        }
        putchar('\n');
    }
}

// 총알 생성
void spawnBullet() {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].active) {
            bullets[i].direction = rand() % 4;
            bullets[i].active = 1;

            // 총알 초기 위치 설정
            switch (bullets[i].direction) {
            case 0: bullets[i].x = WIDTH - 1; bullets[i].y = rand() % HEIGHT; break;
            case 1: bullets[i].x = 0; bullets[i].y = rand() % HEIGHT; break;
            case 2: bullets[i].x = rand() % WIDTH; bullets[i].y = 0; break;
            case 3: bullets[i].x = rand() % WIDTH; bullets[i].y = HEIGHT - 1; break;
            }
            break;
        }
    }
}

// 총알 업데이트
void updateBullets() {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            switch (bullets[i].direction) {
            case 0: bullets[i].x -= BULLET_SPEED; break;
            case 1: bullets[i].x += BULLET_SPEED; break;
            case 2: bullets[i].y += BULLET_SPEED; break;
            case 3: bullets[i].y -= BULLET_SPEED; break;
            }

            // 벽에 닿으면 비활성화
            if (bullets[i].x < 0 || bullets[i].x >= WIDTH || bullets[i].y < 0 || bullets[i].y >= HEIGHT) {
                bullets[i].active = 0;
            }

            // 플레이어와 충돌 체크
            if (bullets[i].x == playerX && bullets[i].y == playerY) {
                gotoxy(0, HEIGHT);
                printf("Game Over!\n");
                exit(0);
            }
        }
    }
}

// 플레이어 업데이트
void updatePlayer() {
    if (_kbhit()) {
        char input = _getch();
        if (input == 'w' && playerY > 0) playerY--;
        else if (input == 's' && playerY < HEIGHT - 1) playerY++;
        else if (input == 'a' && playerX > 0) playerX--;
        else if (input == 'd' && playerX < WIDTH - 1) playerX++;
    }
}

// 게임 업데이트
void updateGame() {
    updatePlayer();
    updateBullets();

    // 일정 확률로 총알 생성
    if (rand() % 10 < 3) {
        spawnBullet();
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

    char screen[HEIGHT][WIDTH];

    // 게임 루프
    while (1) {
        drawGame(screen);
        updateGame();
        Sleep(FRAME_DELAY); // 게임 루프 속도 조절
    }

    return 0;
}
