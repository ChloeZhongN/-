#include "raindrop.h"
#include <stdlib.h>
#include <time.h>
#include <mmsystem.h>
#include <Windows.h>
#pragma comment(lib, "winmm.lib")

// 入水时播放声音
void PlaySplashSound(int type) {
    switch (type % 3) {
        case 0: Beep(800, 50); break;
        case 1: Beep(600, 40); break;
        case 2: Beep(1000, 30); break;
    }
}

// 创建雨滴
RainDrop* CreateRainDrop(int windX, int rainSpeed) {
    RainDrop* drop = new RainDrop;

    drop->x = rand() % SCREEN_WIDTH;
    drop->y = -(rand() % 100);
    drop->endY = WATER_LEVEL - (rand() % 100);

    // 雨速影响下落速度（rainSpeed是1-30）
    drop->speed = 3 + rainSpeed / 6;  // 3-8像素/帧

    drop->color = RGB(rand() % 256, rand() % 256, rand() % 256);
    drop->status = STATE_FALLING;  // 下落阶段
    drop->circleR = 0;
    drop->maxCircleR = 8 + rand() % 15;
    drop->fade = 255;

    // 初始化闪烁效果
    drop->visible = true;      // 初始可见
    drop->blinkTimer = rand() % 100;  // 随机化闪烁时机

    drop->next = NULL;

    // 初始位置受风向影响
    drop->x += windX * 2;
    if (drop->x < 0) drop->x = 0;
    if (drop->x >= SCREEN_WIDTH) drop->x = SCREEN_WIDTH - 1;

    return drop;
}

// 更新雨滴
void UpdateRainDrop(RainDrop* drop, int windX) {
    if (drop->status == STATE_FALLING) {
        // 闪烁效果：控制闪烁频率
        drop->blinkTimer++;

        // 控制闪烁速度：每4-8帧切换一次可见性（更慢的闪烁）
        int blinkThreshold = 10 + (rand() % 10);  // 5-8帧切换一次

        if (drop->blinkTimer >= blinkThreshold) {
            drop->blinkTimer = 0;
            drop->visible = !drop->visible;  // 切换可见性
        }

        // 无论是否可见都下落，这样位置更连贯
        drop->y += drop->speed;
        drop->x += windX;  // 应用风力

        // 边界检查
        if (drop->x < 0) drop->x = 0;
        if (drop->x >= SCREEN_WIDTH) drop->x = SCREEN_WIDTH - 1;

        // 检查是否到达水面
        if (drop->y >= drop->endY) {
            drop->status = STATE_SPLASH;
            drop->splashTimer = 0;
            PlaySplashSound(drop->soundType);
        }
    }
    else if (drop->status == STATE_SPLASH) {
        // 入水溅起效果（短暂停留）
        drop->splashTimer++;
        if (drop->splashTimer >= 3) {  // 3帧后进入水圈阶段
            drop->status = STATE_RIPPLE;
            drop->circleR = 1;
        }
    }
    else if (drop->status == STATE_RIPPLE) {
        // 水圈扩散
        drop->circleR += 1;
        drop->fade -= 10;

        if (drop->fade <= 0 || drop->circleR > drop->maxCircleR) {
            drop->fade = 0;
        }
    }
}

// 绘制雨滴
void DrawRainDrop(RainDrop* drop) {
    if (drop->status == STATE_FALLING) {
        // 闪烁效果：只在可见时绘制
        if (drop->visible) {
            // 主雨线（亮色）
            setlinecolor(drop->color);
            int lineLength = 8 + drop->speed;
            int topY = drop->y - lineLength;
            if (topY < 0) topY = 0;

            // 绘制雨线
            line(drop->x, topY, drop->x, drop->y);

            // 添加拖尾效果（更淡的线条）
            COLORREF trailColor = RGB(
                GetRValue(drop->color) * 0.6,
                GetGValue(drop->color) * 0.6,
                GetBValue(drop->color) * 0.6
            );
            setlinecolor(trailColor);
            line(drop->x, topY - 3, drop->x, topY);
        }
        else {
            // 不可见时，有30%的概率绘制一个很淡的雨滴
            if (rand() % 3 == 0) {
                COLORREF faintColor = RGB(
                    GetRValue(drop->color) * 0.3,
                    GetGValue(drop->color) * 0.3,
                    GetBValue(drop->color) * 0.3
                );
                setlinecolor(faintColor);
                int lineLength = 6;
                int topY = drop->y - lineLength;
                if (topY < 0) topY = 0;
                line(drop->x, topY, drop->x, drop->y);
            }
        }
    }
    else if (drop->status == STATE_SPLASH) {
        // 入水瞬间：一个小圆点
        setfillcolor(drop->color);
        solidcircle(drop->x, drop->endY, 3);
    }
    else if (drop->status == STATE_RIPPLE && drop->fade > 0) {
        // 多层水圈效果
        for (int i = 0; i < 2; i++) {
            int radius = drop->circleR - i * 4;
            if (radius > 0) {
                int alpha = drop->fade - i * 40;
                if (alpha > 0) {
                    COLORREF circleColor = RGB(
                        GetRValue(drop->color) * alpha / 255,
                        GetGValue(drop->color) * alpha / 255,
                        GetBValue(drop->color) * alpha / 255
                    );
                    setlinecolor(circleColor);
                    circle(drop->x, drop->endY, radius);
                }
            }
        }
    }
}