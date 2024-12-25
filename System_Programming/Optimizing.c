#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bmplib.h"
#include "hw2.h"

void filter_optimized(void* args[]) { // inline
    unsigned int width = *(unsigned int*)args[0];
    unsigned int height = *(unsigned int*)args[1];
    Pixel* input = (Pixel*)args[2];
    Pixel* output = (Pixel*)args[3];
    float* filter = (float*)args[4];

    float filter_var0 = filter[0];
    float filter_var1 = filter[1];
    float filter_var2 = filter[2];
    float filter_var3 = filter[3];
    float filter_var4 = filter[4];
    float filter_var5 = filter[5];
    float filter_var6 = filter[6];
    float filter_var7 = filter[7];
    float filter_var8 = filter[8]; // var 미리 할당

    int y_width = -width;
    for (int y = 0; y < height; ++y) {
        y_width += width; // Code Motion + dynamic programming
        
        int y_condition1 = y + -1; // Code Motion
        int y_condition2 = y; // Code Motion
        int y_condition3 = y + 1; // Code Motion
        int y_condition_width1 = y_width - width; // Code Motion
        int y_condition_width2 = y_width; // Code Motion
        int y_condition_width3 = y_width + width; // Code Motion, Remov mul

        for (int x = 0; x < width; x+=2) { // Loop unrolling
            float r1 = 0, g1 = 0, b1 = 0;
            float r2 = 0, g2 = 0, b2 = 0;

            int x_condition1_1 = x + -1; // Code Motion
            int x_condition2_1 = x; // Code Motion
            int x_condition3_1 = x + 1; // Code Motion

            int x_condition1_2 = x; // Code Motion
            int x_condition2_2 = x_condition3_1; // Code Motion
            int x_condition3_2 = x + 2; // Code Motion

            // Unrolling - First Pixel
            if (y_condition1 >= 0 && y_condition1 < height) {
                if (x_condition1_1 >= 0 && x_condition1_1 < width) { // Cache friendly
                    int input_index = x_condition1_1 + y_condition_width1;
                    r1 += input[input_index].r * filter_var0; // Mem Aliasing
                    g1 += input[input_index].g * filter_var0;
                    b1 += input[input_index].b * filter_var0;
                }
                if (x_condition2_1 >= 0 && x_condition2_1 < width) {
                    int input_index = x_condition2_1 + y_condition_width1;
                    r1 += input[input_index].r * filter_var1; // Mem Aliasing
                    g1 += input[input_index].g * filter_var1;
                    b1 += input[input_index].b * filter_var1;
                }
                if (x_condition3_1 >= 0 && x_condition3_1 < width) {
                    int input_index = x_condition3_1 + y_condition_width1;
                    r1 += input[input_index].r * filter_var2; // Mem Aliasing
                    g1 += input[input_index].g * filter_var2;
                    b1 += input[input_index].b * filter_var2;
                }
            }

            // 현재 픽셀에 대한 연산 - First Pixel
            if (y_condition2 >= 0 && y_condition2 < height) {
                if (x_condition1_1 >= 0 && x_condition1_1 < width) {
                    int input_index = x_condition1_1 + y_condition_width2;
                    r1 += input[input_index].r * filter_var3; // Mem Aliasing
                    g1 += input[input_index].g * filter_var3;
                    b1 += input[input_index].b * filter_var3;
                }
                if (x_condition2_1 >= 0 && x_condition2_1 < width) {
                    int input_index = x_condition2_1 + y_condition_width2;
                    r1 += input[input_index].r * filter_var4; // Mem Aliasing
                    g1 += input[input_index].g * filter_var4;
                    b1 += input[input_index].b * filter_var4;
                }
                if (x_condition3_1 >= 0 && x_condition3_1 < width) {
                    int input_index = x_condition3_1 + y_condition_width2;
                    r1 += input[input_index].r * filter_var5; // Mem Aliasing
                    g1 += input[input_index].g * filter_var5;
                    b1 += input[input_index].b * filter_var5;
                }
            }

            // 두 번째 픽셀에 대한 연산 - First Pixel
            if (y_condition3 >= 0 && y_condition3 < height) {
                if (x_condition1_1 >= 0 && x_condition1_1 < width) {
                    int input_index = x_condition1_1 + y_condition_width3;
                    r1 += input[input_index].r * filter_var6; // Mem Aliasing
                    g1 += input[input_index].g * filter_var6;
                    b1 += input[input_index].b * filter_var6;
                }
                if (x_condition2_1 >= 0 && x_condition2_1 < width) {
                    int input_index = x_condition2_1 + y_condition_width3;
                    r1 += input[input_index].r * filter_var7; // Mem Aliasing
                    g1 += input[input_index].g * filter_var7;
                    b1 += input[input_index].b * filter_var7;
                }
                if (x_condition3_1 >= 0 && x_condition3_1 < width) {
                    int input_index = x_condition3_1 + y_condition_width3;
                    r1 += input[input_index].r * filter_var8; // Mem Aliasing
                    g1 += input[input_index].g * filter_var8;
                    b1 += input[input_index].b * filter_var8;
                }
            }

            // Unrolling - Second Pixel
            if (y_condition1 >= 0 && y_condition1 < height) {
                if (x_condition1_2 >= 0 && x_condition1_2 < width) { // Cache friendly
                    int input_index = x_condition1_2 + y_condition_width1;
                    r2 += input[input_index].r * filter_var0; // Mem Aliasing
                    g2 += input[input_index].g * filter_var0;
                    b2 += input[input_index].b * filter_var0;
                }
                if (x_condition2_2 >= 0 && x_condition2_2 < width) {
                    int input_index = x_condition2_2 + y_condition_width1;
                    r2 += input[input_index].r * filter_var1; // Mem Aliasing
                    g2 += input[input_index].g * filter_var1;
                    b2 += input[input_index].b * filter_var1;
                }
                if (x_condition3_2 >= 0 && x_condition3_2 < width) {
                    int input_index = x_condition3_2 + y_condition_width1;
                    r2 += input[input_index].r * filter_var2; // Mem Aliasing
                    g2 += input[input_index].g * filter_var2;
                    b2 += input[input_index].b * filter_var2;
                }
            }

            // 현재 픽셀에 대한 연산 - Second Pixel
            if (y_condition2 >= 0 && y_condition2 < height) {
                if (x_condition1_2 >= 0 && x_condition1_2 < width) {
                    int input_index = x_condition1_2 + y_condition_width2;
                    r2 += input[input_index].r * filter_var3; // Mem Aliasing
                    g2 += input[input_index].g * filter_var3;
                    b2 += input[input_index].b * filter_var3;
                }
                if (x_condition2_2 >= 0 && x_condition2_2 < width) {
                    int input_index = x_condition2_2 + y_condition_width2;
                    r2 += input[input_index].r * filter_var4; // Mem Aliasing
                    g2 += input[input_index].g * filter_var4;
                    b2 += input[input_index].b * filter_var4;
                }
                if (x_condition3_2 >= 0 && x_condition3_2 < width) {
                    int input_index = x_condition3_2 + y_condition_width2;
                    r2 += input[input_index].r * filter_var5; // Mem Aliasing
                    g2 += input[input_index].g * filter_var5;
                    b2 += input[input_index].b * filter_var5;
                }
            }

            // 두 번째 픽셀에 대한 연산 - Second Pixel
            if (y_condition3 >= 0 && y_condition3 < height) {
                if (x_condition1_2 >= 0 && x_condition1_2 < width) {
                    int input_index = x_condition1_2 + y_condition_width3;
                    r2 += input[input_index].r * filter_var6; // Mem Aliasing
                    g2 += input[input_index].g * filter_var6;
                    b2 += input[input_index].b * filter_var6;
                }
                if (x_condition2_2 >= 0 && x_condition2_2 < width) {
                    int input_index = x_condition2_2 + y_condition_width3;
                    r2 += input[input_index].r * filter_var7; // Mem Aliasing
                    g2 += input[input_index].g * filter_var7;
                    b2 += input[input_index].b * filter_var7;
                }
                if (x_condition3_2 >= 0 && x_condition3_2 < width) {
                    int input_index = x_condition3_2 + y_condition_width3;
                    r2 += input[input_index].r * filter_var8; // Mem Aliasing
                    g2 += input[input_index].g * filter_var8;
                    b2 += input[input_index].b * filter_var8;
                }
            }

            // 조건문 좀 더 간결하게 -> but 영향 거의 없을 듯,,

            int output_index = x + y_width;
            output[output_index].r = (unsigned char)(r1 < 0 ? 0 : (r1 > 255 ? 255 : r1));
            output[output_index].g = (unsigned char)(g1 < 0 ? 0 : (g1 > 255 ? 255 : g1));
            output[output_index].b = (unsigned char)(b1 < 0 ? 0 : (b1 > 255 ? 255 : b1));

            output_index = x_condition3_1 + y_width;
            output[output_index].r = (unsigned char)(r2 = r2 < 0 ? 0 : (r2 > 255 ? 255 : r2));
            output[output_index].g = (unsigned char)(g2 = g2 < 0 ? 0 : (g2 > 255 ? 255 : g2));
            output[output_index].b = (unsigned char)(b2 = b2 < 0 ? 0 : (b2 > 255 ? 255 : b2));
        }
    }
}
