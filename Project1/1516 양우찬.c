#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>
#include <windows.h>
#include <locale.h>


#define WIDTH 40             // 게임판 가로 크기
#define HEIGHT 25            // 게임판 세로 크기
int BULLET_SPEED = 1; // 총알 속도
#define MAX_BULLETS 40    // 동시에 발사되는 총알 개수
#define FRAME_DELAY 10       // 프레임 지연(단위: 밀리초)
int BULLET_DIR_CYCLE = 15;// 총알 방향 변경 주기(단위: 초)
#define LEVEL_UP_CYCLE 30    // 레벨 업 주기(단위: 초)

typedef struct {
    int x;
    int y;
    int direction; // 0: 왼쪽, 1: 오른쪽, 2: 위, 3: 아래
    int active;    // 총알 활성화 상태
} Bullet;

int playerX, playerY, currentDir, currentLevel, playerLife, isGameing;
char player_name[20];
double player_score = 0;
Bullet bullets[MAX_BULLETS];
time_t currentBulletTime;
time_t currentScoreTime;
time_t currentLevelTime;


// 커서를 특정 위치로 이동
void gotoxy(int x, int y) {
    COORD pos = { x, y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

// 게임 초기화
void initGame() {
    playerX = WIDTH / 2;
    playerY = HEIGHT / 2;
    currentBulletTime = time(NULL);
    currentScoreTime = time(NULL);
    currentLevelTime = time(NULL);
    currentDir = rand() % 4;

    player_score = 0;
    currentLevel = 1;
    playerLife = 3;

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

    gotoxy(0, HEIGHT+2);
    printf("플레이어: %s\n남은 생명: %5d     현재 점수: %5.1lf     현재 레벨: %5d", player_name, playerLife, player_score, currentLevel);
}

// 총알 생성
void spawnBullet() {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].active) {
            bullets[i].direction = currentDir;
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
void BulletGameOver() {
    isGameing = 0;

    gotoxy(0, HEIGHT+4);

    FILE* fp = fopen("score.txt", "r");

    // 파일이 존재하지 않으면 새로 생성
    if (fp == NULL) {
        fp = fopen("score.txt", "w");
        if (fp == NULL) {
            printf("파일을 생성할 수 없습니다.\n");
            exit(1);
        }
        fprintf(fp, "0\n");  // 초기 플레이어 수를 0으로 설정
    }

    int rank = 0;
    int playerNum = 0;
    fscanf(fp, "%d", &playerNum);

    struct Player {
        char name[20];
        double score;
    };

    struct Player* players = malloc(sizeof(struct Player) * (playerNum + 1));
    if (players == NULL) {
        printf("메모리 할당 실패!\n");
        fclose(fp);
        exit(1);
    }

    for (int i = 0; i < playerNum; i++) {
        fscanf(fp, "%s %lf", players[i].name, &players[i].score);

        if (player_score >= players[i].score) {
            rank = i + 1;

            players[i].score = player_score;
            strcpy(players[i].name, player_name);

            // 기존 점수들 이동
            for(int j = i; j < playerNum; j++) {
                fscanf(fp, "%s %lf", players[j+1].name, &players[j+1].score);
			}

            break;
        }
    }

    if (rank == 0)
        rank = playerNum+1;

    // 점수 추가 후 출력
    players[playerNum].score = player_score;
    strcpy(players[playerNum].name, player_name);

    fclose(fp);  // 파일 닫기 전에 점수 리스트를 다시 저장하려면 파일을 열어야 함

    fp = fopen("score.txt", "w");
    if (fp == NULL) {
        printf("파일을 다시 열 수 없습니다.\n");
        free(players);
        exit(1);
    }

    fprintf(fp, "%d\n", playerNum + 1);
    for (int i = 0; i < playerNum + 1; i++) {
        fprintf(fp, "%s %lf\n", players[i].name, players[i].score);
    }

    fclose(fp);  // 파일 닫기

    free(players);  // 메모리 해제

    printf("Game Over!\n%s님의 점수는 %.1lf점 입니다! (%d등)", player_name, player_score, rank);
    
    printf("\n\n뒤로 돌아가려면 Enter키를 누르시오..");

    getchar();
    getchar();

    main();
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
                playerLife--;

                if(playerLife <= 0)
                    BulletGameOver();
            }
        }
    }
}

// 플레이어 업데이트
void updatePlayer() {
    if (_kbhit()) {
        player_score += 0.1;

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

    time_t newTime = time(NULL);
    if (difftime(newTime, currentBulletTime) >= BULLET_DIR_CYCLE){
        currentBulletTime = newTime;
        currentDir = rand() % 4;
	}

    if(difftime(newTime, currentScoreTime) >= 1){
		currentScoreTime = newTime;
		player_score++;
	}

    if(difftime(newTime, currentLevelTime) >= LEVEL_UP_CYCLE){
		currentLevelTime = newTime;
		BULLET_SPEED++;

        if(BULLET_DIR_CYCLE > 3)
            BULLET_DIR_CYCLE -= 1;
        currentLevel++;
	}
}

void GameStart() {

    printf("플레이어 이름을 띄어쓰기 없이 입력하세요: ");
	scanf("%s", player_name);

    system("cls");

    for (int i = 0; i < 3; i++) {
        printf("게임 시작...%d", 3-i);
		Sleep(1000);
		system("cls");
    }

    system("cls");

	srand(time(NULL));
	initGame();

	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cursorInfo;
	GetConsoleCursorInfo(consoleHandle, &cursorInfo);
	cursorInfo.bVisible = 0;
	SetConsoleCursorInfo(consoleHandle, &cursorInfo);

	char screen[HEIGHT][WIDTH];

	// 게임 루프
    isGameing = 1;
	while (isGameing) {
		drawGame(screen);
		updateGame();
		Sleep(FRAME_DELAY); // 게임 루프 속도 조절
	}
}

void Title() {

    printf(" ____  _    _ _      _      ______ _______ \n");
    printf("|  _ \| |  | | |    | |    |  ____|__   __|\n");
    printf("| |_) | |  | | |    | |    | |__     | |   \n");
    printf("|  _ <| |  | | |    | |    |  __|    | |   \n");
    printf("| |_) | |__| | |____| |____| |____   | |   \n");
    printf("|____/_\____/|______|______|______|  |_|   \n");
    printf(" / ____|   /\   |  \/  |  ____|            \n");
    printf("| |  __   /  \  | \  / | |__               \n");
    printf("| | |_ | / /\ \ | |\/| |  __|              \n");
    printf("| |__| |/ ____ \| |  | | |____             \n");
    printf(" \_____/_/    \_\_|  |_|______|            \n\n\n\n");

    printf("게임 규칙\n상하좌우로 날아오는 총알을 WASD키를 이용해 피하세요.\n총알에 맞으면 게임이 종료됩니다.\n점수는 1초에 1점씩 오르며, 한 번 이동할 때마다 0.1점이 추가됩니다.\n30초마다 레벨이 올라가며, 총알 속도가 상승하고 방향 변환 주기가 감소합니다.\n\n\n");

    printf("-----  MENU  -----\n");
    
    printf("1. 게임 시작\n");
    printf("2. 스코어보드\n");
    printf("3. 게임 종료\n\n");

    printf("메뉴를 선택하세요: ");
}

void ShowScoreBoard() {
    FILE *fp = fopen("score.txt", "r");

    if (fp == NULL) {
		printf("파일을 열 수 없습니다.\n");
		exit(1);
	}

    int playerNum;
	fscanf(fp, "%d", &playerNum);

	struct Player {
		char name[20];
		double score;
	};

	struct Player* players = malloc(sizeof(struct Player) * playerNum);
	if (players == NULL) {
		printf("메모리 할당 실패!\n");
		fclose(fp);
		exit(1);
	}

	for (int i = 0; i < playerNum; i++) {
		fscanf(fp, "%s %lf", players[i].name, &players[i].score);
	}

	fclose(fp);

	printf("-----  SCORE BOARD  -----\n");
	for (int i = 0; i < playerNum; i++) {
		printf("%d등: %s - %.1lf점\n", i+1, players[i].name, players[i].score);
	}

    printf("\n\n뒤로 돌아가려면 Enter키를 누르시오..");

    getchar();
    getchar();

    main();

	free(players);
}



int main() {
    int menu;

    system("cls");

	Title();
	scanf("%d", &menu);

    system("cls");

	switch (menu) {
	case 1:
		GameStart();
		break;
	case 2:
        ShowScoreBoard();
		break;
	case 3:
        exit(0);
	default:
		printf("잘못된 입력입니다.\n");
		break;
	}

	return 0;
}
