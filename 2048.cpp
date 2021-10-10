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
//胜利条件
#define TARGET 2048

//游戏状态
#define S_FAIL 0
#define S_WIN 1
#define S_NORMAL 2
#define S_QUIT 3

class Game2048
{
private:
    int data[N][N];
    int status;

    //左上角为（0，0），在指定位置画一个字符
    void drawItem(int row, int col, char c);
    //游戏里数字是右对齐,row、col是数字最后一位所在的位置
    void drawNum(int row, int col, int num);
    //重新开始
    void restart();
    //随机生成一个新的数组，若失败返回false；
    bool randNew();
    //向左移动
    bool moveLeft();
    //矩阵逆时针旋转90°
    void rotate();
    //判断游戏已经结束
    bool isOver();

public:
    Game2048();

    //绘制游戏界面
    void draw();
    //测试数据
    void setTestData();
    //获取状态
    int getStatus();
    //处理按键
    void processInput();
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
    // setTestData()
    restart();
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

            if (status == S_WIN)
            {
                mvprintw(N, 5 * N / 2 - 1, " YOU Win, PRESS R TO CONTINUE ");
            }
            else if (status == S_FAIL)
            {
                mvprintw(N, 5 * N / 2 - 1, " YOU LOSE,PRESS R TO CONTINUE ");
            }
        }
    }
}

void Game2048::setTestData()
{
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            data[i][j] = 16 << (i + j);
            // data[i][j] = 2 << (1 + rand() % 7);
        }
    }
}

int Game2048::getStatus()
{
    return status;
}

void Game2048::restart()
{
    for (int i = 0; i < N; ++i)
    {
        for (int j = 0; j < N; ++j)
        {
            data[i][j] = 0;
        }
    }
    randNew();
    randNew();
    status = S_NORMAL;
}

bool Game2048::randNew()
{
    //存储空位置
    vector<int> emptyPos;
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            if (data[i][j] == 0)
            {
                //使用一个整数存储i和j
                emptyPos.push_back(i * N + j);
            }
        }
    }

    if (emptyPos.size() == 0)
    {
        return false;
    }

    //随机找一个空位
    int value = emptyPos[rand() % emptyPos.size()];
    //10%概率产生1
    data[value / N][value % N] = rand() % 10 == 1 ? 4 : 2;
    return true;
}

void Game2048::processInput()
{
    char ch = getch();
    //转换成大写
    if (ch >= 'a' && ch <= 'z')
    {
        ch -= 32;
    }

    if (status == S_NORMAL)
    {
        bool updated = false;
        if (ch == 'A')
        {
            moveLeft();
        }
        else if (ch == 'S')
        {
            rotate();
            rotate();
            rotate();
            updated = moveLeft();
            rotate();
        }
        else if (ch == 'D')
        {
            rotate();
            rotate();
            updated = moveLeft();
            rotate();
            rotate();
        }
        else if (ch == 'W')
        {
            rotate();
            updated = moveLeft();
            rotate();
            rotate();
            rotate();
        }

        if (updated)
        {
            randNew();
            if (isOver())
            {
                status = S_FAIL;
            }
        }
    }

    if (ch == 'Q')
    {
        status = S_QUIT;
    }
    else if (ch == 'R')
    {
        restart();
    }
}

bool Game2048::moveLeft()
{
    int temp[N][N];
    for (int i = 0; i < N; i++)
    {
        //逐行处理

        //当前可写入位置
        int currentWritePos = 0;
        //存放需要进行碰撞判断的数
        int lastValue = 0;
        for (int j = 0; j < N; j++)
        {
            temp[i][j] = data[i][j];
            //值为空不处理
            if (data[i][j] == 0)
            {
                continue;
            }
            //之前不需要判断碰撞，则取当前数
            if (lastValue == 0)
            {
                lastValue = data[i][j];
            }
            else
            {
                //需要进行碰撞合并
                if (lastValue == data[i][j])
                {
                    data[i][currentWritePos] = lastValue * 2;
                    lastValue = 0;
                    if (data[i][currentWritePos] == TARGET)
                    {
                        status = S_WIN;
                    }
                }
                else
                {
                    //不需要进行碰撞合并
                    data[i][currentWritePos] = lastValue;
                    lastValue = data[i][j];
                }
                ++currentWritePos;
            }
            //已经处理过的值清零
            data[i][j] = 0;
        }
        if (lastValue != 0)
        {
            data[i][currentWritePos] = lastValue;
        }
    }

    //检查是否发生变化
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            if (data[i][j] != temp[i][j])
            {
                return true;
            }
        }
    }
    return false;
}

void Game2048::rotate()
{
    int temp[N][N];
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            temp[i][j] = data[j][N - i - 1];
        }
    }
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            data[i][j] = temp[i][j];
        }
    }
}

bool Game2048::isOver()
{
    int tmp[N][N];
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            //有空位或相邻有一样都可以继续
            if ((j + 1 < N) && (data[i][j] * data[i][j + 1] == 0 || data[i][j] == data[i][j + 1]))
            {
                return false;
            }
            if ((i + 1 < N) && (data[i][j] * data[i + 1][j] == 0 || data[i][j] == data[i + 1][j]))
            {
                return false;
            }
        }
    }
    return true;
}

void initiallize()
{
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

void shutdown()
{
    endwin();
}

int main()
{
    initiallize();

    Game2048 game;
    char ch = 'n';
    do
    {
        game.draw();
        game.processInput();
    } while (S_QUIT != game.getStatus());

    shutdown();
    return 0;
}
