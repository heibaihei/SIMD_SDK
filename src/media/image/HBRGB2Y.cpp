//
//  HBRGB2Y.cpp
//  SIMD_SDK
//
//  Created by zj-db0519 on 2018/7/17.
//  Copyright © 2018年 meitu. All rights reserved.
//

#include "HBRGB2Y.h"

/**
 *  @func Plain_RGB2Y 将 RGB 图像数据转换成 Y 灰度图输出
 *  @param [in] Src 输入的RGB数据流
 *  @param [out] Dest 输出的灰度图, 外部创建空间
 *  @param [in] Width RGB数据流宽
 *  @param [in] Height RGB数据流高
 *  @param [in] Stride RGB数据流步长
 */
void Plain_RGB2Y(unsigned char *Src, unsigned char *Dest, int Width, int Height, int Stride)
{
    /** 计算系数 */
    const int B_WT = int(0.114 * 256 + 0.5);
    const int G_WT = int(0.587 * 256 + 0.5);
    const int R_WT = 256 - B_WT - G_WT;            //     int(0.299 * 256 + 0.5);
    
    for (int Y = 0; Y < Height; Y++)
    {
        unsigned char *LinePS = Src + Y * Stride;
        unsigned char *LinePD = Dest + Y * Width;
        for (int X = 0; X < Width; X++, LinePS += 3)
        {
            /**
             * 逐行根据系数进行计算，R/G/B分量分别乘以各自的系数得到亮度值，注意这个系数是归一化的
             */
            LinePD[X] = (B_WT * LinePS[0] + G_WT * LinePS[1] + R_WT * LinePS[2]) >> 8;
        }
    }
}

/**
 *  @func Plain_RGB2Y 将 RGB 图像数据转换成 Y 灰度图输出
 *  @param [in] Src 输入的RGB数据流
 *  @param [out] Dest 输出的灰度图, 外部创建空间
 *  @param [in] Width RGB数据流宽
 *  @param [in] Height RGB数据流高
 *  @param [in] Stride RGB数据流步长
 *  @descript: 采用4路并行
 */
void Optimize_M_RGB2Y(unsigned char *Src, unsigned char *Dest, int Width, int Height, int Stride)
{
    const int B_WT = int(0.114 * 256 + 0.5);
    const int G_WT = int(0.587 * 256 + 0.5);
    const int R_WT = 256 - B_WT - G_WT;            //     int(0.299 * 256 + 0.5);
    
    for (int Y = 0; Y < Height; Y++)
    {
        unsigned char *LinePS = Src + Y * Stride;
        unsigned char *LinePD = Dest + Y * Width;
        int X = 0;
        for (; X < Width - 4; X += 4, LinePS += 12)
        {
            LinePD[X + 0] = (B_WT * LinePS[0] + G_WT * LinePS[1] + R_WT * LinePS[2]) >> 8;
            LinePD[X + 1] = (B_WT * LinePS[3] + G_WT * LinePS[4] + R_WT * LinePS[5]) >> 8;
            LinePD[X + 2] = (B_WT * LinePS[6] + G_WT * LinePS[7] + R_WT * LinePS[8]) >> 8;
            LinePD[X + 3] = (B_WT * LinePS[9] + G_WT * LinePS[10] + R_WT * LinePS[11]) >> 8;
        }
        for (; X < Width; X++, LinePS += 3)
        {
            LinePD[X] = (B_WT * LinePS[0] + G_WT * LinePS[1] + R_WT * LinePS[2]) >> 8;
        }
    }
}

{// 参考链接：http://www.cnblogs.com/Imageshop/p/6261719.html
    __m128i p1aL = _mm_mullo_epi16(_mm_cvtepu8_epi16(_mm_loadu_si128((__m128i *)(LinePS + 0))), _mm_setr_epi16(B_WT, G_WT, R_WT, B_WT, G_WT, R_WT, B_WT, G_WT));
    __m128i p2aL = _mm_mullo_epi16(_mm_cvtepu8_epi16(_mm_loadu_si128((__m128i *)(LinePS + 1))), _mm_setr_epi16(G_WT, R_WT, B_WT, G_WT, R_WT, B_WT, G_WT, R_WT));
    __m128i p3aL = _mm_mullo_epi16(_mm_cvtepu8_epi16(_mm_loadu_si128((__m128i *)(LinePS + 2))), _mm_setr_epi16(R_WT, B_WT, G_WT, R_WT, B_WT, G_WT, R_WT, B_WT));
    
    __m128i p1aH = _mm_mullo_epi16(_mm_cvtepu8_epi16(_mm_loadu_si128((__m128i *)(LinePS + 8))), _mm_setr_epi16(R_WT, B_WT, G_WT, R_WT, B_WT, G_WT, R_WT, B_WT));
    __m128i p2aH = _mm_mullo_epi16(_mm_cvtepu8_epi16(_mm_loadu_si128((__m128i *)(LinePS + 9))), _mm_setr_epi16(B_WT, G_WT, R_WT, B_WT, G_WT, R_WT, B_WT, G_WT));
    __m128i p3aH = _mm_mullo_epi16(_mm_cvtepu8_epi16(_mm_loadu_si128((__m128i *)(LinePS + 10))), _mm_setr_epi16(G_WT, R_WT, B_WT, G_WT, R_WT, B_WT, G_WT, R_WT));
    
    __m128i p1bL = _mm_mullo_epi16(_mm_cvtepu8_epi16(_mm_loadu_si128((__m128i *)(LinePS + 18))), _mm_setr_epi16(B_WT, G_WT, R_WT, B_WT, G_WT, R_WT, B_WT, G_WT));
    __m128i p2bL = _mm_mullo_epi16(_mm_cvtepu8_epi16(_mm_loadu_si128((__m128i *)(LinePS + 19))), _mm_setr_epi16(G_WT, R_WT, B_WT, G_WT, R_WT, B_WT, G_WT, R_WT));
    __m128i p3bL = _mm_mullo_epi16(_mm_cvtepu8_epi16(_mm_loadu_si128((__m128i *)(LinePS + 20))), _mm_setr_epi16(R_WT, B_WT, G_WT, R_WT, B_WT, G_WT, R_WT, B_WT));
    
    __m128i p1bH = _mm_mullo_epi16(_mm_cvtepu8_epi16(_mm_loadu_si128((__m128i *)(LinePS + 26))), _mm_setr_epi16(R_WT, B_WT, G_WT, R_WT, B_WT, G_WT, R_WT, B_WT));
    __m128i p2bH = _mm_mullo_epi16(_mm_cvtepu8_epi16(_mm_loadu_si128((__m128i *)(LinePS + 27))), _mm_setr_epi16(B_WT, G_WT, R_WT, B_WT, G_WT, R_WT, B_WT, G_WT));
    __m128i p3bH = _mm_mullo_epi16(_mm_cvtepu8_epi16(_mm_loadu_si128((__m128i *)(LinePS + 28))), _mm_setr_epi16(G_WT, R_WT, B_WT, G_WT, R_WT, B_WT, G_WT, R_WT));
    
    __m128i sumaL = _mm_add_epi16(p3aL, _mm_add_epi16(p1aL, p2aL));
    __m128i sumaH = _mm_add_epi16(p3aH, _mm_add_epi16(p1aH, p2aH));
    __m128i sumbL = _mm_add_epi16(p3bL, _mm_add_epi16(p1bL, p2bL));
    __m128i sumbH = _mm_add_epi16(p3bH, _mm_add_epi16(p1bH, p2bH));
    __m128i sclaL = _mm_srli_epi16(sumaL, 8);
    __m128i sclaH = _mm_srli_epi16(sumaH, 8);
    __m128i sclbL = _mm_srli_epi16(sumbL, 8);
    __m128i sclbH = _mm_srli_epi16(sumbH, 8);
    __m128i shftaL = _mm_shuffle_epi8(sclaL, _mm_setr_epi8(0, 6, 12, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1));
    __m128i shftaH = _mm_shuffle_epi8(sclaH, _mm_setr_epi8(-1, -1, -1, 18, 24, 30, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1));
    __m128i shftbL = _mm_shuffle_epi8(sclbL, _mm_setr_epi8(-1, -1, -1, -1, -1, -1, 0, 6, 12, -1, -1, -1, -1, -1, -1, -1));
    __m128i shftbH = _mm_shuffle_epi8(sclbH, _mm_setr_epi8(-1, -1, -1, -1, -1, -1, -1, -1, -1, 18, 24, 30, -1, -1, -1, -1));
    __m128i accumL = _mm_or_si128(shftaL, shftbL);
    __m128i accumH = _mm_or_si128(shftaH, shftbH);
    __m128i h3 = _mm_or_si128(accumL, accumH);
    //__m128i h3 = _mm_blendv_epi8(accumL, accumH, _mm_setr_epi8(0, 0, 0, -1, -1, -1, 0, 0, 0, -1, -1, -1, 1, 1, 1, 1));
    _mm_storeu_si128((__m128i *)(LinePD + X), h3);
    
}

