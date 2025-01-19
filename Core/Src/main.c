/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "gpio.h"
#include "fsmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include<stdio.h>
#include <stdlib.h>

//#include "model_weights.h"  // 包含模型权重文件
#include "pt.h"  // 包含模型权重文件
#define OUTPUT_SIZE 10

#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/KEY/key.h"
#include "./BSP/TOUCH/touch.h"

#define CANVAS_WIDTH (28 * 3)  // 画布宽度
#define CANVAS_HEIGHT 28       // 画布高度

/**
 * @brief 将触摸点映射到画布并扩展
 *
 * @param x 触摸屏画布的横坐标 (0-239)
 * @param y 触摸屏画布的纵坐标 (240-319)
 * @param input 输入画布数组 (28*3 x 28 的二值化数组)
 */
void expand_input(int x, int y, uint8_t* input, int thickness) {
    // 映射触摸屏坐标到28*3 x 28画布
    int mapped_x = x * CANVAS_WIDTH / 240;
    int mapped_y = (y - 240) * CANVAS_HEIGHT / 80;

    // 检查映射后的坐标是否在画布范围内
    if (mapped_x < 0 || mapped_x >= CANVAS_WIDTH || mapped_y < 0 || mapped_y >= CANVAS_HEIGHT) {
        return; // 坐标超出范围，直接返回
    }

    // 遍历扩展范围内的所有点并填充
    for (int dx = 0; dx < thickness; dx++) { // 横向扩展范围
        for (int dy = 0; dy < thickness+1; dy++) { // 纵向扩展范围
            // 计算扩展点的坐标
            int nx = mapped_x + dx;
            int ny = mapped_y + dy;

            // 检查扩展点是否在画布范围内
            if (nx >= 0 && nx < CANVAS_WIDTH && ny >= 0 && ny < CANVAS_HEIGHT) {
                input[ny * CANVAS_WIDTH + nx] = 1; // 在输入数组中填充 1
            }
        }
    }
}


/**
 * @brief       清空屏幕并在右上角显示"RST"
 * @param       无
 * @retval      无
 */
void load_draw_dialog(void)
{
    lcd_clear(WHITE);                                                /* 清屏 */
    lcd_show_string(lcddev.width - 24, 0, 200, 16, 16, "RST", BLUE); /* 显示清屏区域 */
}

/**
 * @brief       画粗线
 * @param       x1,y1: 起点坐标
 * @param       x2,y2: 终点坐标
 * @param       size : 线条粗细程度
 * @param       color: 线的颜色
 * @retval      无
 */
void lcd_draw_bline(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t size, uint16_t color)
{
    uint16_t t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, row, col;

    if (x1 < size || x2 < size || y1 < size || y2 < size)
        return;

    delta_x = x2 - x1; /* 计算坐标增量 */
    delta_y = y2 - y1;
    row = x1;
    col = y1;

    if (delta_x > 0)
    {
        incx = 1; /* 设置单步方向 */
    }
    else if (delta_x == 0)
    {
        incx = 0; /* 垂直线 */
    }
    else
    {
        incx = -1;
        delta_x = -delta_x;
    }

    if (delta_y > 0)
    {
        incy = 1;
    }
    else if (delta_y == 0)
    {
        incy = 0; /* 水平线 */
    }
    else
    {
        incy = -1;
        delta_y = -delta_y;
    }

    if (delta_x > delta_y)
        distance = delta_x; /* 选取基本增量坐标轴 */
    else
        distance = delta_y;

    for (t = 0; t <= distance + 1; t++) /* 画线输出 */
    {
        lcd_fill_circle(row, col, size, color); /* 画点 */
        xerr += delta_x;
        yerr += delta_y;

        if (xerr > distance)
        {
            xerr -= distance;
            row += incx;
        }

        if (yerr > distance)
        {
            yerr -= distance;
            col += incy;
        }
    }
}

/**
 * @brief       电阻触摸屏测试函数
 * @param       无
 * @retval      无
 */
static uint8_t input[28*3*28];

static int loc_mask[10]={0,0,0,0,0,0,0,0,0,0};

void draw_input_to_screen(uint8_t* input) {
    // 定义屏幕画布的起始位置和大小
    int screen_start_x = 0;         // 起始x坐标
    int screen_start_y = 120;       // 起始y坐标
    int screen_width = 28 * 3;      // 绘制区域宽度
    int screen_height = 28;         // 绘制区域高度
    lcd_draw_line(0,120,0+28*3,120,BLUE);
    lcd_draw_line(0,120+28,0+28*3,120+28,BLUE);
    lcd_draw_line(0+28*3,120,0+28*3,120+28,BLUE);

    // 遍历 input 数组并绘制到屏幕
    for (int y = 0; y < screen_height; y++) {
        for (int x = 0; x < screen_width; x++) {
            // 如果 input 数组中当前点值为 1，绘制点
            if (input[y * screen_width + x] == 1) {
                lcd_draw_point(screen_start_x + x, screen_start_y + y, RED); // 映射到屏幕上的对应位置
            }
        }
    }

    for (int i = 0; i < 9; i++)
    {
    	if(loc_mask[i]!=0)
    		lcd_draw_line(loc_mask[i],120,loc_mask[i],120+28,BLUE);
    }
    for (int i = 0; i < 9; i++)
    {
    	loc_mask[i] = 0;
    }
}

void draw_input_to_screen_with_border(uint8_t* input) {
    // 定义屏幕画布的起始位置和大小
    int screen_start_x = 0;         // 起始x坐标
    int screen_start_y = 120;       // 起始y坐标
    int screen_width = 28 * 3;      // 绘制区域宽度
    int screen_height = 28;         // 绘制区域高度

    // 绘制边界
    uint16_t border_color = BLUE;   // 边框颜色
    lcd_draw_line(screen_start_x, screen_start_y, screen_start_x + screen_width - 1, screen_start_y, border_color); // 上边框
    lcd_draw_line(screen_start_x, screen_start_y, screen_start_x, screen_start_y + screen_height - 1, border_color); // 左边框
    lcd_draw_line(screen_start_x, screen_start_y + screen_height - 1, screen_start_x + screen_width - 1, screen_start_y + screen_height - 1, border_color); // 下边框
    lcd_draw_line(screen_start_x + screen_width - 1, screen_start_y, screen_start_x + screen_width - 1, screen_start_y + screen_height - 1, border_color); // 右边框

    // 遍历 input 数组并绘制到屏幕
    for (int y = 0; y < screen_height; y++) {
        for (int x = 0; x < screen_width; x++) {
            // 如果 input 数组中当前点值为 1，绘制点
            if (input[y * screen_width + x] == 1) {
                lcd_draw_point(screen_start_x + x, screen_start_y + y, RED); // 映射到屏幕上的对应位置
            }
        }
    }
}


void rtp_test(void)
{
    uint8_t key;
    uint8_t i = 0;

    while (1)
    {
        key = key_scan(0);
        tp_dev.scan(0);
        lcd_show_string(0, 0, 200, 16, 16, "made by TZH", RED);
        lcd_show_string(0, 20, 200, 16, 16, "predict", RED);
        lcd_draw_hline(0, 80, 240, BLACK);
        lcd_draw_hline(0, 40, 180, BLACK);
        lcd_draw_line(180, 0, 180, 80, BLACK);
        lcd_draw_line(0, 240, 240, 240, BLACK);
        lcd_show_string(0, 220, 200, 16, 16, "write here", BLUE);
        if (tp_dev.sta & TP_PRES_DOWN)  /* 触摸屏被按下 */
        {
            if (tp_dev.x[0] < lcddev.width && tp_dev.y[0] < lcddev.height)
            {
                if (tp_dev.x[0] > (lcddev.width - 60) && tp_dev.y[0] < 80)
                {
                    load_draw_dialog(); /* 清除 */
                    for(int i=0;i<28*3*28;i++) //清空画布
                    {
                    	input[i]=0;
                    }
                }
                else
                {
                	if(tp_dev.y[0] > 240)
                	{
                        int idx = tp_draw_big_point(tp_dev.x[0], tp_dev.y[0], RED);   /* 画点 */

//                		int idx = my_lcd_draw_point(tp_dev.x[0], tp_dev.y[0], RED);   /* 画点 */

                        //坐标为(tp_dev.x[0], tp_dev.y[0])
                        //input[28*3*28]
                        // 映射并扩展到input画布
                        expand_input(tp_dev.x[0], tp_dev.y[0], input,1);

                	}

					if(tp_dev.x[0] < 120 && tp_dev.y[0] < 80) //预测按钮
					{

					    // 定义输出字符串
					    char output_string[20];

					    // 处理表达式
					    process_expression(input, output_string,loc_mask);

					    lcd_show_string(0, 40, 200, 16, 16, output_string, RED);
					    lcd_show_string(0, 80, 200, 16, 16, "Sample results:", BLUE);
					    lcd_show_string(0, 160, 200, 16, 16, output_string, RED);
					    draw_input_to_screen(input);
//					    lcd_draw_point(x,y, RED);

					}

                }
            }
        }
        else
        {

        }

        if (key == KEY0_PRES)   /* KEY0按下,则执行校准程序 */
        {
            lcd_clear(WHITE);   /* 清屏 */
            tp_adjust();        /* 屏幕校准 */
            tp_save_adjust_data();
            load_draw_dialog();
        }

        i++;

        if (i % 2000 == 0)LED0_TOGGLE();
    }
}



/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */



int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_FSMC_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
	lcd_init();
    key_init();                         /* 初始化按键 */
    tp_dev.init();                      /* 触摸屏初始化 */
    delay_pre = 0;
    for(int i=0;i<28*3*28;i++)
    {
    	input[i]=0;
    }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */



    if (tp_dev.touchtype != 0xFF)
    {
        lcd_show_string(30, 110, 200, 16, 16, "Press KEY0 to Adjust", RED); /* 电阻屏才显示 */
    }

    delay_ms(1500);
    load_draw_dialog();

    if (tp_dev.touchtype & 0x80)
    {
        //ctp_test(); /* 电容屏测试 */
    }
    else
    {
        rtp_test(); /* 电阻屏测试 */
    }

	while (1) {
    /* USER CODE END WHILE */
//		HAL_I2C
		lcd_show_string(30, 0, 240, 30, 24, "initing", 0xF800);

    /* USER CODE BEGIN 3 */
	}
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
	}
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
