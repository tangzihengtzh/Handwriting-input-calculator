#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include "pt.h" // ����Ȩ������
#include <stdio.h>
#include <string.h>


#include "stdio.h"
#include "stdlib.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/TOUCH/touch.h"
#include "./BSP/24CXX/24cxx.h"
#include "./SYSTEM/delay/delay.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))

// ����� ReLU
void relu(float* data, int size) {
    for (int i = 0; i < size; i++) {
        data[i] = MAX(data[i], 0);
    }
}

// �������
void conv2d(const float* input, const float* kernel, const float* bias, float* output,
    int in_channels, int out_channels, int input_h, int input_w, int kernel_size, int stride, int padding) {
    int output_h = (input_h - kernel_size + 2 * padding) / stride + 1;
    int output_w = (input_w - kernel_size + 2 * padding) / stride + 1;

    // ��ʼ�����Ϊ��
    for (int oc = 0; oc < out_channels; oc++) {
        for (int i = 0; i < output_h * output_w; i++) {
            output[oc * output_h * output_w + i] = 0;
        }
    }

    // �������ͨ��
    for (int oc = 0; oc < out_channels; oc++) {
        for (int ic = 0; ic < in_channels; ic++) {
            for (int oh = 0; oh < output_h; oh++) {
                for (int ow = 0; ow < output_w; ow++) {
                    float value = 0;
                    for (int kh = 0; kh < kernel_size; kh++) {
                        for (int kw = 0; kw < kernel_size; kw++) {
                            int ih = oh * stride + kh - padding;
                            int iw = ow * stride + kw - padding;
                            if (ih >= 0 && ih < input_h && iw >= 0 && iw < input_w) {
                                value += input[ic * input_h * input_w + ih * input_w + iw] *
                                    kernel[oc * in_channels * kernel_size * kernel_size +
                                    ic * kernel_size * kernel_size + kh * kernel_size + kw];
                            }
                        }
                    }
                    output[oc * output_h * output_w + oh * output_w + ow] += value;
                }
            }
        }
        // ����ƫ��
        for (int i = 0; i < output_h * output_w; i++) {
            output[oc * output_h * output_w + i] += bias[oc];
        }
    }
}

// ���ػ�����
void max_pool2d(const float* input, float* output, int channels, int input_h, int input_w, int kernel_size, int stride) {
    int output_h = (input_h - kernel_size) / stride + 1;
    int output_w = (input_w - kernel_size) / stride + 1;

    for (int c = 0; c < channels; c++) {
        for (int oh = 0; oh < output_h; oh++) {
            for (int ow = 0; ow < output_w; ow++) {
                float max_value = -INFINITY;
                for (int kh = 0; kh < kernel_size; kh++) {
                    for (int kw = 0; kw < kernel_size; kw++) {
                        int ih = oh * stride + kh;
                        int iw = ow * stride + kw;
                        if (ih < input_h && iw < input_w) {
                            max_value = MAX(max_value, input[c * input_h * input_w + ih * input_w + iw]);
                        }
                    }
                }
                output[c * output_h * output_w + oh * output_w + ow] = max_value;
            }
        }
    }
}

// ȫ���Ӳ�
void fully_connected(const float* input, const float* weights, const float* bias, float* output, int input_size, int output_size) {
    for (int o = 0; o < output_size; o++) {
        output[o] = 0;
        for (int i = 0; i < input_size; i++) {
            output[o] += input[i] * weights[o * input_size + i];
        }
        output[o] += bias[o];
    }
}



#include <stdlib.h>

// �Ż����ǰ�򴫲�����
void forward(float* input, float* output) {
    // ��ʱ�洢��������̬���䣩
    float* conv1_out = (float*)malloc(sizeof(float) * 4 * 28 * 28); // ��1����������
    float* pool1_out = (float*)malloc(sizeof(float) * 4 * 14 * 14); // ��1�γػ����

    // ��1������� + ReLU
    conv2d(input, conv1_weight, conv1_bias, conv1_out, 1, 4, 28, 28, 3, 1, 1);
    relu(conv1_out, 4 * 28 * 28);

    // ���ػ���
    max_pool2d(conv1_out, pool1_out, 4, 28, 28, 2, 2);

    // �ͷŵ�1�����������
    free(conv1_out);

    // ��2������� + ReLU
    float* conv2_out = (float*)malloc(sizeof(float) * 4 * 14 * 14); // ��2����������
    conv2d(pool1_out, conv2_weight, conv2_bias, conv2_out, 4, 4, 14, 14, 3, 1, 1);
    relu(conv2_out, 4 * 14 * 14);

    // ���ػ���
    float* pool2_out = (float*)malloc(sizeof(float) * 4 * 7 * 7); // ��2�γػ����
    max_pool2d(conv2_out, pool2_out, 4, 14, 14, 2, 2);

    // �ͷŵ�1�γػ��͵�2�����������
    free(pool1_out);
    free(conv2_out);

    // ȫ���Ӳ�1 + ReLU
    float* fc1_out = (float*)malloc(sizeof(float) * 16); // ȫ���Ӳ�1���
    fully_connected(pool2_out, fc1_weight, fc1_bias, fc1_out, 4 * 7 * 7, 16);
    relu(fc1_out, 16);

    // �ͷŵ�2�γػ������
    free(pool2_out);

    // ȫ���Ӳ�2������㣩
    fully_connected(fc1_out, fc2_weight, fc2_bias, output, 16, 14);

    // �ͷ�ȫ���Ӳ�1�����
    free(fc1_out);
}


/**
 * @brief �ָ��ַ�����
 *
 * @param canvas     ���뻭������СΪ 28*3*28 �� uint8 ����
 * @param start_mask ����������ʼ����
 * @param char_slice ����ַ���Ƭ��28*28 �� float ���飩
 * @param next_mask  �����һ����������
 */
int segment_character(
    const uint8_t* canvas,
    int start_mask,
    float char_slice[CHAR_HEIGHT][CHAR_WIDTH],
    int* next_mask
) {

    // ��ʼ����Ƭ�������Ϊ 0
    memset(char_slice, 0, sizeof(float) * CHAR_HEIGHT * CHAR_WIDTH);

    // �ҵ��ַ�����ʼ��
    int start_col = -1;
    for (int col = start_mask; col < CANVAS_WIDTH; col++) {
        for (int row = 0; row < CANVAS_HEIGHT; row++) {
            if (canvas[row * CANVAS_WIDTH + col] > 0) { // ����Ƿ�Ϊ��������
                start_col = col;
                break;
            }
        }
        if (start_col != -1) break; // �ҵ���ʼ��
    }

    // ���û���ҵ���Ч����
    if (start_col == -1) {
        *next_mask = CANVAS_WIDTH; // ����Ϊ������Χ
        return 0;                  // û���µ���Ƭ
    }

    // �ҵ��ַ��Ľ�����
    int end_col = start_col;
    for (int col = start_col; col < CANVAS_WIDTH; col++) {
        int is_valid_column = 0;
        for (int row = 0; row < CANVAS_HEIGHT; row++) {
            if (canvas[row * CANVAS_WIDTH + col] > 0) { // ����Ƿ�Ϊ��������
                is_valid_column = 1;
                break;
            }
        }
        if (is_valid_column) {
            end_col = col; // ���½�����
        }
        else {
            break; // ��������ֹͣ
        }
    }

    // ��ȡ�ַ����
    int char_width = end_col - start_col + 1;

    // ����������䣨ˮƽ���У�
    int pad_left = (CHAR_WIDTH - char_width) / 2;
    int pad_right = CHAR_WIDTH - char_width - pad_left;

    // ��ȡ�ַ�������䵽 char_slice
    for (int row = 0; row < CANVAS_HEIGHT; row++) {
        // ����ַ����ݵ�ˮƽ����λ��
        for (int col = start_col; col <= end_col && (col - start_col) < CHAR_WIDTH; col++) {
            char_slice[row][pad_left + (col - start_col)] = (float)canvas[row * CANVAS_WIDTH + col]; // �����ֵ������
        }
    }

    // ������һ����������
    *next_mask = end_col + 1;

    return 1; // �ҵ���һ���µ���Ƭ
}



/**
 * @brief �������뻭����ʶ����ʽ��������
 *
 * @param canvas �����28*3 x 28��С�Ķ�ֵ������
 * @param output_string ����ַ����飬����ʶ��ı��ʽ�ͽ������ "2+1=3"��
 */
void process_expression(const uint8_t* canvas, char* output_string ,int *loc_mask) {
    // ����ָ�������
    float char_slice[CHAR_HEIGHT][CHAR_WIDTH];
    int next_mask = 0; // ��һ�ε�������ʼλ��
    int start_mask = 0; // ��ʼ����
    float output[14];   // ǰ�򴫲���������
    int predicted_class; // �洢Ԥ�����
    char expression[MAX_EXPRESSION_LENGTH] = ""; // �洢ʶ��ı��ʽ
    int expression_index = 0; // ���ʽ����
    int result = 0; // ���ڴ洢���ռ�����
    int current_number = 0; // ��ǰ����
    char current_operator = 0; // ��ǰ������

    int loc_idx = 0;
    lcd_show_string(120, 100, 200, 16, 16, "segment_character", BLUE);
    while (start_mask < CANVAS_WIDTH) {
        // ���÷ָ��
        int seg_result = segment_character(canvas, start_mask, char_slice, &next_mask);
        loc_mask[loc_idx] = next_mask;
        loc_idx ++;
        // ���û���ҵ����ַ����˳�ѭ��
        if (seg_result == 0) {
        	lcd_show_string(120, 100, 200, 16, 16, "no input", BLUE);
            break;
        }

        // ����ǰ�򴫲�����ʶ���ַ�
        forward((float*)char_slice, output);
        lcd_show_string(120, 120, 200, 16, 16, "detect_character", BLUE);

        // ��ȡԤ�����
        float max_prob = output[0];
        predicted_class = 0;
        for (int i = 1; i < 14; i++) {
            if (output[i] > max_prob) {
                max_prob = output[i];
                predicted_class = i;
            }
        }

        // ��Ԥ�����תΪ�ַ��������
        if (predicted_class >= 0 && predicted_class <= 9) {
            // ʶ��Ϊ����
            current_number = current_number * 10 + predicted_class; // �����λ��
            expression[expression_index++] = '0' + predicted_class; // ������ʽ
        }
        else if (predicted_class >= 10 && predicted_class <= 13) {
            // ʶ��Ϊ������
            if (current_operator == 0) {
                // ��һ������������ʼ�����
                result = current_number;
            }
            else {
                // ������һ��������
                switch (current_operator) {
                case 10: result += current_number; break; // �ӷ�
                case 11: result -= current_number; break; // ����
                case 12: result *= current_number; break; // �˷�
                case 13: result /= current_number; break; // ����
                }
            }

            // ���µ�ǰ����������յ�ǰ����
            current_operator = predicted_class;
            current_number = 0;

            // ��������������ʽ
            switch (predicted_class) {
            case 10: expression[expression_index++] = '+'; break;
            case 11: expression[expression_index++] = '-'; break;
            case 12: expression[expression_index++] = 'x'; break;
            case 13: expression[expression_index++] = '/'; break;
            }
        }

        // ��������λ��
        start_mask = next_mask;
    }

    // �������һ������
    lcd_show_string(120, 140, 200, 16, 16, "get_result", BLUE);
    if (current_operator != 0) {
        switch (current_operator) {
        case 10: result += current_number; break; // �ӷ�
        case 11: result -= current_number; break; // ����
        case 12: result *= current_number; break; // �˷�
        case 13: result /= current_number; break; // ����
        }
    }
    else {
        // ���û�в����������������������
        result = current_number;
    }

    // �������ʽ�ַ���
    expression[expression_index] = '\0';

    // �����ʽ�ͽ����������ַ���
    lcd_show_string(120, 100, 200, 16, 16, "                ", BLUE);
    lcd_show_string(120, 120, 200, 16, 16, "                ", BLUE);
    lcd_show_string(120, 140, 200, 16, 16, "                ", BLUE);
    lcd_show_string(120, 160, 200, 16, 16, "finished", BLUE);
    sprintf(output_string, "%s=%d", expression, result);
}
