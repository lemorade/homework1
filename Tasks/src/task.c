/**
 * @file    task.c
 * @brief   电控第一次作业 —— 第 1/2/3 题（GPIO / 定时器 / 看门狗）的业务代码全部写在本文件。
 * @note    tick 只由定时器更新中断累加，不使用 HAL_Delay 或空循环。
 */
#include "main.h"
#include "tasks.h"

extern IWDG_HandleTypeDef hiwdg;
extern TIM_HandleTypeDef htim2;

/* 全局 tick，必须是全局 volatile uint32_t，Ozone 的 Watched Data 才能看到 */
volatile uint32_t tick = 0;

void Tasks_Init(void)
{
    /* ===== 第 1 题 GPIO =====
     * PC13 已在 CubeMX 里配成推挽输出，这里把它写成表中的电平：
     * 低电平 -> 最小系统板板载 LED 点亮。
     * 注意：只在这里写一次，不要在 while (1) 里翻转。 */
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

    /* ===== 第 2 题 定时器 =====
     * 启动 TIM2 的更新中断。漏掉这一句，tick 永远是 0。
     * 周期计算（TIM2 挂在 APB1 上）：
     *   CubeMX 时钟树：SYSCLK = HSI = 8 MHz（未使用 PLL）
     *                   HCLK = 8 MHz，APB1 分频 = 1 -> PCLK1 = 8 MHz
     *   APB1 分频为 1 时不乘 2，所以定时器时钟 = PCLK1 = 8 MHz
     *   (PSC + 1) * (ARR + 1) / 定时器时钟 = (7 + 1) * (999 + 1) / 8000000
     *                                      = 8000 / 8000000 = 0.001 s = 1 ms
     * 也就是说 PSC = 7、ARR = 999（已在 CubeMX 中配置）。 */
    HAL_TIM_Base_Start_IT(&htim2);
}

/* 定时器更新回调，全工程只写这一份。
 * CubeMX 若在 main.c 或中断文件里也生成了同名函数，要删掉那一份，否则链接报重复定义。 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM2)
    {
        tick++;

        /* ================= 两题分两次下载，只改这一行 =================
         * 第 2 题（定时器）：取消下面一行的注释 -> 重新编译、下载，
         *                    然后在 Ozone 里看 tick 每秒增加约 1000。
         * 第 3 题（看门狗）：保持注释（即不喂狗）-> 重新编译、下载，
         *                    然后在 Ozone 里看 tick 涨到约 2000 后复位归零。
         * ============================================================ */
        // HAL_IWDG_Refresh(&hiwdg);
    }
}
