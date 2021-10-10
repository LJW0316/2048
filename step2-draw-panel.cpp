#include <string>
#include <vector>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <curses.h>
using namespace std;

//格子数
#define N 4
//每个格子的字符长度
#define WIDTH 5

class Game2048
{
private:
    int data[N][N];

    //左上角为（0，0），在指定位置画一个字符
    void drawItem(int row, int col, char c);
    //游戏里数字是右对齐,row、col是数字最后一位所在的位置
    void drawNum(int row, int col, int num);

public:
    Game2048();

    //绘制游戏界面
    void draw();
    //测试数据
    void setTestData();
};

void Game2048::drawItem(int row, int col, char c)
{
    move(row, col);
    addch(c);
}

void Game2048::drawNum(int row, int col, int num)
{
    while (num > 0)
    {
        drawItem(row, col, num % 10 + '0');
        num /= 10;
        --col;
    }
}

Game2048::Game2048()
{
    setTestData();
}

void Game2048::draw()
{
    //清理屏幕
    clear();
    //居中偏移
    const int offset = 12;
    for (int i = 0; i <= N; i++)
    {
        for (int j = 0; j <= N; j++)
        {
            //画相交点‘+’
            drawItem(i * 2, 1 + j * WIDTH + offset, '+');

            //画竖线‘|’
            if (i != N)
            {
                drawItem(i * 2 + 1, 1 + j * WIDTH + offset, '|');
            }

            //画横线‘-’
            for (int k = 1; j != N && k < WIDTH; k++)
            {
                drawItem(i * 2, 1 + j * WIDTH + k + offset, '-');
            }

            //画每个格子里的数
            if (i != N && j != N)
            {
                drawNum(i * 2 + 1, (j + 1) * WIDTH + offset, data[i][j]);
            }

            //提示文字
            mvprintw(2 * N + 2, (5 * (N - 4) - 1) / 2, "W(UP),S(DOWN),A(LEFT),D(RIGHT),R(RESTART),Q(QUIT)");
        }
    }
}

void Game2048::setTestData()
{
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            data[i][j] = 16 << i << j;
        }
    }
}


void initiallize() {
    //ncurses初始化
    initscr();
    //按键不需要输入回车直接交互
    cbreak();
    //按键不显示
    noecho();
    //隐藏光标
    curs_set(0);
    //随机数
    srand(time(NULL));
}

void shutdown() {
    endwin();
}

int main() {
    initiallize();

    Game2048 game;
    char ch = 'n';
    do {
        game.draw();
        ch = getch();
    } while (ch != 'Q' && ch != 'q');

    shutdown();
    return 0;
}
