#include <turboc.h>
#include <bits/stdc++.h>

#define BX 5
#define BY 1
#define BW 10
#define BH 20

using namespace std;

void drawScreen();
void printBrick(BOOL Show);
int getAround(int x, int y, int b, int r);
BOOL processKey();
BOOL moveDown();
void testFull();
void printBoard();
void printPoint();
void printNextBrick(BOOL Show);
int randomNum();

struct Point {
    int x,y;
};

struct Point Shape[][4][4]={
    { {0,0,1,0,2,0,-1,0}, {0,0,0,1,0,-1,0,-2},
    {0,0,1,0,2,0,-1,0}, {0,0,0,1,0,-1,0,-2} }, // 0번 벽돌
    { {0,0,1,0,0,1,1,1}, {0,0,1,0,0,1,1,1},
    {0,0,1,0,0,1,1,1}, {0,0,1,0,0,1,1,1} }, // 1번 벽돌
    { {0,0,-1,0,0,-1,1,-1}, {0,0,0,1,-1,0,-1,-1},
    {0,0,-1,0,0,-1,1,-1}, {0,0,0,1,-1,0,-1,-1} }, // 2번 벽돌
    { {0,0,-1,-1,0,-1,1,0}, {0,0,-1,0,-1,1,0,-1},
    {0,0,-1,-1,0,-1,1,0}, {0,0,-1,0,-1,1,0,-1} }, // 3번 벽돌
    { {0,0,-1,0,1,0,-1,-1}, {0,0,0,-1,0,1,-1,1},
    {0,0,-1,0,1,0,1,1}, {0,0,0,-1,0,1,1,-1} }, // 4번 벽돌
    { {0,0,1,0,-1,0,1,-1}, {0,0,0,1,0,-1,-1,-1},
    {0,0,1,0,-1,0,-1,1}, {0,0,0,-1,0,1,1,1} }, // 5번 벽돌
    { {0,0,-1,0,1,0,0,1}, {0,0,0,-1,0,1,1,0},
    {0,0,-1,0,1,0,0,-1}, {0,0,-1,0,0,-1,0,1} }, // 6번 벽돌
};

int nx, ny;
int brick, rot, color, point, high, nextbrick, nextcolor;
enum {EMPTY, BRICK, WALL, NONE};
string arTile[]={"\033[0;37m. ", "■", "\033[0;37m□", " "};
int board[BW+2][BH+2];
int rboard[BW+2][BH+2];
int gboard[BW+2][BH+2];
int bboard[BW+2][BH+2];
enum {BLACK, RED, GREEN, YELLOW, BLUE, PURPLE, CYAN, WHITE};
int boardColor[BW+2][BH+2];
int pointArray[4] = {10000, 20000, 20000, 30000};

int main() {
    FILE *fp = fopen("highscore.txt", "r");
    fscanf(fp, "%d", &high);
    fclose(fp);
    int x, y;
    setCursorType(NOCURSOR); // 커서를 안보이게 하는 함수
    clearScreen(); // 화면을 깨끗이 지우는 함수
    for(x=0;x<BW+2;x++) {  // board 배열 초기화
        for(y=0;y<BH+2;y++) {
            board[x][y] = (y == 0 || y == BH+1 || x == 0 || x == BW+1) ? WALL : EMPTY;
            rboard[x][y] = (y == 0 || y == BH+1 || x == 0 || x == BW+1) ? WALL : EMPTY;
            gboard[x][y] = (y == 0 || y == BH+1 || x == 0 || x == BW+1) ? WALL : EMPTY;
            bboard[x][y] = (y == 0 || y == BH+1 || x == 0 || x == BW+1) ? WALL : EMPTY;
        }
    }

    random_device rd;
    mt19937 mt(time(NULL));
    uniform_int_distribution<int> dis3(0, 2);
    uniform_int_distribution<int> dis7(0, 6);
    drawScreen(); // 게임화면 그리는 함수 호출
    int nFrame, nStay;
    nFrame = 20;
    nextbrick = dis7(mt);
    nextcolor = 1 << dis3(mt);
    for(;1;) {
        drawScreen();
        // 벽돌 선택하기
        brick = nextbrick;
        nextbrick = dis7(mt);
        color = nextcolor;
        nextcolor = 1 << dis3(mt);
        nx = BW / 2; // 벽돌의 초기 x좌표
        ny = 2; // 벽돌의 초기 y좌표
        rot = 0; // 벽돌의 회전모양 번호

        printBrick(TRUE); // 벽돌 그리기 함수 호출
        printNextBrick(TRUE);
        // 벽돌주변이 비었는지 체크
        if(getAround(nx, ny, brick, rot) != EMPTY) break;
        nStay = nFrame;
        for(;2;) {
            if(--nStay == 0) {
                nStay = nFrame;
                if(moveDown()) break;
            }
            if(processKey()) break;
            delay(1000/20);
        }
        printNextBrick(FALSE);
    }
    clearScreen();
    gotoXY(30, 12);
    cout << "G A M E O V E R";
    if(point > high) {
        FILE *fp = fopen("highscore.txt", "w");
        fprintf(fp, "%d", point);
        fclose(fp);
    }
    printPoint();
    setCursorType(NORMALCURSOR);
    getch();
}

void drawScreen() {
    int x, y;
    for(x=0;x<BW+2;x++) {
        for(y=0;y<BH+2;y++) {
            gotoXY(BX+x*2, BY+y);
            cout << "\033[0;3"+to_string(boardColor[x][y])+"m"+arTile[board[x][y]];
        }
    }
    gotoXY(50, 3);
    cout << "\033[0;31mR\033[0;32mG\033[0;34mB\033[0;38m Tetris";
    gotoXY(50, 5);
    cout << "\033[0;38mTOP\t" << setw(6) << setfill('0') << max(high, point);
    gotoXY(50, 6);
    cout << "\033[0;38mSCORE\t" << setw(6) << setfill('0') << point;
    gotoXY(50, 8);
    cout << "White Block\t: +100 points";
    gotoXY(50, 9);
    cout << "White Line\t: +10000 points";
}

void printBrick(BOOL Show) {
    int i;
    for(i=0;i<4;i++) {
        int brickx = Shape[brick][rot][i].x+nx, bricky = Shape[brick][rot][i].y+ny;
        gotoXY(BX+brickx*2, BY+bricky);
        if(Show) {
            cout << "\033[0;3"+to_string(color | boardColor[brickx][bricky])+"m"+arTile[BRICK];
        }
        else {
            cout << "\033[0;3"+to_string(boardColor[brickx][bricky])+"m"+arTile[boardColor[brickx][bricky] ? BRICK : EMPTY];
        }
    }
}

void printNextBrick(BOOL Show) {
    int i;
    gotoXY(33, 17);
    cout << "\033[0;38mNEXT BRICK";
    for(i=0;i<4;i++) {
        gotoXY(1+(18+Shape[nextbrick][0][i].x)*2, 20+Shape[nextbrick][0][i].y);
        cout << "\033[0;3"+to_string(nextcolor)+"m"+arTile[Show ? BRICK : NONE];
    }
}


void printPoint() {
    gotoXY(50, 5);
    cout << "\033[0;38mTOP\t" << setw(6) << setfill('0') << max(high, point);
    gotoXY(50, 6);
    cout << "\033[0;38mScore\t" << setw(6) << setfill('0') << point;
}

int getAround(int x, int y, int b, int r) {
    int i, k = EMPTY;
    if(color & 4) {
        for(i=0;i<4;i++) {
            k = max(k, bboard[x+Shape[b][r][i].x][y+Shape[b][r][i].y]);
        }
    }
    if(color & 2) {
        for(i=0;i<4;i++) {
            k = max(k, gboard[x+Shape[b][r][i].x][y+Shape[b][r][i].y]);
        }
    }
    if(color & 1) {
        for(i=0;i<4;i++) {
            k = max(k, rboard[x+Shape[b][r][i].x][y+Shape[b][r][i].y]);
        }
    }
    return k;
}

BOOL processKey() {
    int ch, trot;
    if(kbhit()) {
        ch = getch();
        if(ch == 0xE0 || ch == 0) {
            ch = getch();
            switch (ch) {
                case 75:
                    if(getAround(nx-1, ny, brick, rot) == EMPTY) {
                        printBrick(FALSE);
                        nx--;
                        printBrick(TRUE); // 벽돌 그리기
                    }
                    break;
                case 77: // 오른쪽 키 눌렀을 때
                    if(getAround(nx+1, ny, brick, rot) == EMPTY) {
                        printBrick(FALSE);
                        nx++;
                        printBrick(TRUE);
                    }
                    break;
                case 72: // 벽돌 회전
                    trot = (rot == 0 ? 3 : rot-1);
                    if(getAround(nx, ny, brick, trot) == EMPTY) {
                        printBrick(FALSE);
                        rot = trot;
                        printBrick(TRUE);
                    }

                    break;
                case 80: // 벽돌 한 번에 내리기
                    if(moveDown()) {
                        return TRUE;
                    }
                    point += 1;
                    printPoint();
                    break;
            }
        }
        else {
            switch (ch) {
                case ' ': // 스페이스바 키 입력시
                    while(moveDown() == FALSE) point += 2;
                    printPoint();
                    return TRUE;
            }
        }
    }
    return FALSE;
}

BOOL moveDown() {
    if(getAround(nx, ny+1, brick, rot) != EMPTY) {
        testFull();
        return TRUE;
    }
    printBrick(FALSE);
    ny++;
    printBrick(TRUE);
    return FALSE;
}

void testFull() {
    int i,x,y,ty;

    for(i=0;i<4;i++) {
        board[nx+Shape[brick][rot][i].x][ny+Shape[brick][rot][i].y] = BRICK;
        boardColor[nx+Shape[brick][rot][i].x][ny+Shape[brick][rot][i].y] |= color;
        if(boardColor[nx+Shape[brick][rot][i].x][ny+Shape[brick][rot][i].y] == WHITE) {
            point += 100;
        }
        if(color & 4) {
            bboard[nx+Shape[brick][rot][i].x][ny+Shape[brick][rot][i].y] = BRICK;
        }
        if(color & 2) {
            gboard[nx+Shape[brick][rot][i].x][ny+Shape[brick][rot][i].y] = BRICK;
        }
        if(color & 1) {
            rboard[nx+Shape[brick][rot][i].x][ny+Shape[brick][rot][i].y] = BRICK;
        }
    }

    int cnt = 0;
    for(y=1;y<BH+1;y++) {
        for(x=1;x<BW+1;x++) {
            if(board[x][y] != BRICK || boardColor[x][y] != WHITE) break;
        }
        if(x == BW+1) {
            point += pointArray[cnt];
            cnt++;
            for(ty=y;ty>1;ty--) {
                for(x=1;x<BW+1;x++) {
                    board[x][ty] = board[x][ty-1];
                    rboard[x][ty] = rboard[x][ty-1];
                    gboard[x][ty] = gboard[x][ty-1];
                    bboard[x][ty] = bboard[x][ty-1];
                    boardColor[x][ty] = boardColor[x][ty-1];
                }
            }
            drawScreen();
            delay(200);
        }
    }
}
