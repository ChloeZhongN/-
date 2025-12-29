#include <graphics.h>
#include <conio.h>
#include <windows.h>
#include <time.h>
#include <stdio.h>
#include <tchar.h>
#include <math.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#include "raindrop.h"
#include "rainlist.h"

// ==================== 全局变量定义 ====================
int windX = 0;                    // 风向（-15到15）
int rainSpeed = 10;               // 雨速（1-30）
bool showControls = true;         // 是否显示控制面板

// 启动帮助显示
bool showStartupHelp = true;
int helpTimer = 100;

// 雨滴生成控制
static int generateCounter = 0;   // 生成计数器

// ==================== 函数实现 ====================

void DrawBackground() {
    // 夜空渐变
    for (int y = 0; y < WATER_LEVEL; y++) {
        int brightness = 10 + y / 25;
        setlinecolor(RGB(5, 10, brightness));
        line(0, y, SCREEN_WIDTH, y);
    }

    // 水面
    setfillcolor(RGB(15, 30, 60));
    solidrectangle(0, WATER_LEVEL, SCREEN_WIDTH, SCREEN_HEIGHT);

    // 水面波纹
    static float wave = 0;
    wave += 0.02f;
    setlinecolor(RGB(30, 60, 120));

    for (int i = 0; i < 2; i++) {
        int y = WATER_LEVEL + 20 + i * 15;
        for (int x = 0; x < SCREEN_WIDTH; x += 30) {
            int offset = (int)(4 * sin(x * 0.015 + wave + i * 1.2));
            line(x, y + offset, x + 15, y + offset);
        }
    }

    // 月亮
    setfillcolor(RGB(250, 240, 180));
    solidcircle(680, 100, 25);
}

void DrawControlPanel(RainList* list) {
    if (!showControls) return;

    settextcolor(WHITE);
    settextstyle(16, 0, _T("宋体"));

    setfillcolor(RGB(0, 0, 0, 180));
    solidrectangle(5, 5, 280, 160);

    TCHAR info[100];

    _stprintf_s(info, _T("=== 池塘夜降彩色雨 ==="));
    outtextxy(10, 10, info);

    _stprintf_s(info, _T("当前雨滴: %d 个"), GetRaindropCount(list));
    outtextxy(10, 35, info);

    _stprintf_s(info, _T("风向: %d (← →控制)"), windX);
    outtextxy(10, 55, info);

    _stprintf_s(info, _T("雨速: %d (↑ ↓控制)"), rainSpeed);
    outtextxy(10, 75, info);

    _stprintf_s(info, _T("F1: 显示/隐藏面板"));
    outtextxy(10, 95, info);

    _stprintf_s(info, _T("ESC: 退出程序"));
    outtextxy(10, 115, info);
}

void DrawWindIndicator() {
    if (windX == 0) return;

    int centerX = SCREEN_WIDTH / 2;

    if (windX > 0) {
        setlinecolor(RGB(255, 100, 100));
        line(centerX, 30, centerX + 40, 30);
        line(centerX + 40, 30, centerX + 30, 25);
        line(centerX + 40, 30, centerX + 30, 35);
    }
    else if (windX < 0) {
        setlinecolor(RGB(100, 100, 255));
        line(centerX, 30, centerX - 40, 30);
        line(centerX - 40, 30, centerX - 30, 25);
        line(centerX - 40, 30, centerX - 30, 35);
    }
}

// 添加单个雨滴（从屏幕上方开始）
void AddSingleRaindrop(RainList* list) {
    RainDrop* drop = CreateRainDrop(windX, rainSpeed);

    // 确保雨滴从屏幕上方开始（y坐标为负）
    // -400 到 -100 的范围，确保雨滴从屏幕外开始
    drop->y = -(rand() % 300 + 100);  // -100 到 -400
    
    // 确保入水位置固定在水面
    drop->endY = WATER_LEVEL + rand() % 100;

    AddRainDrop(list, drop);
}

// 最简单的雨滴补充 - 保证源源不断
void SimpleRaindropManagement(RainList* list) {
    generateCounter++;
    
    // 方法1：根据当前雨滴数量决定生成频率
    int currentCount = GetRaindropCount(list);
    
    // 基础生成逻辑：雨滴越少，生成越快
    int generateChance;
    if (currentCount < 20) {
        generateChance = 3;  // 雨滴少，1/3概率生成
    } else if (currentCount < 35) {
        generateChance = 5;  // 中等数量，1/5概率生成
    } else {
        generateChance = 8;  // 雨滴多，1/8概率生成
    }
    
    // 方法2：固定频率生成（更简单可靠）
    // 每10帧至少生成1个雨滴
    if (generateCounter % 10 == 0) {
        // 确保总是生成雨滴
        AddSingleRaindrop(list);
    }
    // 额外的随机生成
    else if (rand() % generateChance == 0) {
        AddSingleRaindrop(list);
    }
}

// 删除最旧的雨滴（避免链表过长）
void RemoveOldestRaindrop(RainList* list) {
    static int removeCounter = 0;
    removeCounter++;
    
    // 当雨滴数量超过一定值时，定期删除一些
    int currentCount = GetRaindropCount(list);
    if (currentCount > 40) {
        // 每20帧删除一个
        if (removeCounter % 20 == 0) {
            if (list->head != NULL) {
                RainDrop* toDelete = list->head;
                list->head = list->head->next;
                delete toDelete;
                list->count--;
            }
        }
    }
}

int main() {
    initgraph(SCREEN_WIDTH, SCREEN_HEIGHT);
    setbkcolor(BLACK);
    cleardevice();

    srand((unsigned int)time(NULL));

    RainList rainList;
    InitRainList(&rainList);

    BeginBatchDraw();

    bool running = true;
    int frameCount = 0;

    // 初始生成一些雨滴
    for (int i = 0; i < 15; i++) {  // 先少量生成
        AddSingleRaindrop(&rainList);
    }

    while (running) {
        frameCount++;

        // ========== 处理键盘输入 ==========
        if (_kbhit()) {
            int ch = _getch();

            if (ch == 27) {  // ESC键
                running = false;
                continue;
            }

            if (ch == 0 || ch == 224) {  // 扩展键
                ch = _getch();

                if (ch == 59) {  // F1键
                    showControls = !showControls;
                }
                else if (ch == 75) {  // 左箭头
                    if (windX > -15) windX--;
                }
                else if (ch == 77) {  // 右箭头
                    if (windX < 15) windX++;
                }
                else if (ch == 72) {  // 上箭头
                    if (rainSpeed < 30) rainSpeed++;
                }
                else if (ch == 80) {  // 下箭头
                    if (rainSpeed > 1) rainSpeed--;
                }
            }

            switch (ch) {
            case ' ':  // 空格键：重置
                windX = 0;
                rainSpeed = 10;
                
                // 清空当前雨滴
                ClearRainList(&rainList);
                
                // 重新生成少量雨滴
                for (int i = 0; i < 15; i++) {
                    AddSingleRaindrop(&rainList);
                }

                Beep(1200, 100);
                break;

            case 'h':  // H键：显示帮助
            case 'H':
                showStartupHelp = true;
                helpTimer = 100;
                break;
                
            case '+':  // 加号：立即添加一些雨滴
            case '=':
                for (int i = 0; i < 5; i++) {
                    AddSingleRaindrop(&rainList);
                }
                Beep(800, 30);
                break;
                
            case '-':  // 减号：删除一些雨滴
            case '_':
                for (int i = 0; i < 3 && GetRaindropCount(&rainList) > 0; i++) {
                    if (rainList.head != NULL) {
                        RainDrop* toDelete = rainList.head;
                        rainList.head = rainList.head->next;
                        delete toDelete;
                        rainList.count--;
                    }
                }
                Beep(600, 30);
                break;
            }
        }

        // ========== 简单的雨滴管理 ==========
        // 1. 持续补充新雨滴
        SimpleRaindropManagement(&rainList);
        
        // 2. 定期清理旧雨滴（防止内存泄漏）
        RemoveOldestRaindrop(&rainList);

        // ========== 清屏 ==========
        cleardevice();

        // ========== 绘制背景 ==========
        DrawBackground();

        // ========== 更新并绘制所有雨滴 ==========
        RainDrop* current = rainList.head;
        while (current) {
            UpdateRainDrop(current, windX);
            DrawRainDrop(current);
            current = current->next;
        }

        // ========== 清理完成的雨滴（入水后消失） ==========
        CleanupRainDrops(&rainList);

        // ========== 绘制各种界面元素 ==========
        DrawWindIndicator();
        DrawControlPanel(&rainList);

        // ========== 底部状态栏 ==========
        settextcolor(LIGHTCYAN);
        settextstyle(14, 0, _T("宋体"));
        TCHAR stats[100];
        _stprintf_s(stats, _T("雨滴: %d | 风速: %d | 雨速: %d | 帧: %d"),
            GetRaindropCount(&rainList), windX, rainSpeed, frameCount);
        outtextxy(10, SCREEN_HEIGHT - 25, stats);

        // ========== 启动帮助显示 ==========
        if (showStartupHelp && helpTimer > 0) {
            helpTimer--;
            if (helpTimer <= 0) showStartupHelp = false;

            settextcolor(YELLOW);
            settextstyle(16, 0, _T("宋体"));

            setfillcolor(RGB(0, 0, 0, 200));
            solidrectangle(SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT / 2 - 100,
                SCREEN_WIDTH / 2 + 200, SCREEN_HEIGHT / 2 + 100);

            outtextxy(SCREEN_WIDTH / 2 - 180, SCREEN_HEIGHT / 2 - 80, _T("=== 持续降雨模式 ==="));
            outtextxy(SCREEN_WIDTH / 2 - 180, SCREEN_HEIGHT / 2 - 50, _T("欢迎观看"));
            outtextxy(SCREEN_WIDTH / 2 - 180, SCREEN_HEIGHT / 2 - 20, _T("以下是使用说明"));
            outtextxy(SCREEN_WIDTH / 2 - 180, SCREEN_HEIGHT / 2 + 10, _T("方向键: 控制风向和雨速"));
            outtextxy(SCREEN_WIDTH / 2 - 180, SCREEN_HEIGHT / 2 + 40, _T("空格键: 重置"));
            outtextxy(SCREEN_WIDTH / 2 - 180, SCREEN_HEIGHT / 2 + 70, _T("ESC键: 退出程序"));
        }

        // ========== 交换双缓冲区 ==========
        FlushBatchDraw();

        // ========== 控制帧率 ==========
        Sleep(40);
    }

    // ==================== 程序结束清理 ====================
    ClearRainList(&rainList);
    EndBatchDraw();
    closegraph();

    return 0;
}