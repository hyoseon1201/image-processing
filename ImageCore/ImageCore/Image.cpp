#include "pch.h"
#include "Image.h"
#include <iostream>
#include <stb_image.h>
#include <stb_image_write.h>
#include <algorithm>

Image::Image(int width, int height, int channels)
    : width(width), height(height), channels(channels)
{
}

bool Image::ReadFromMemory(const unsigned char* data)
{
    if (!data) return false;

    // pixels 벡터 크기 설정: width * height
    pixels.resize(width * height);

    // 각 픽셀 데이터 저장 (기본적으로 RGB 3채널 가정)
    for (int i = 0; i < width * height; i++)
    {
        pixels[i].v[0] = data[i * channels] / 255.0f;
        pixels[i].v[1] = data[i * channels + 1] / 255.0f;
        pixels[i].v[2] = data[i * channels + 2] / 255.0f;
        pixels[i].v[3] = 1.0f;
    }

    // stbi_load로 할당받은 메모리이므로, 반드시 stbi_image_free로 해제합니다.
    stbi_image_free(const_cast<unsigned char*>(data));

    return true;
}

bool Image::WritePNG(const std::string& filepath)
{
    // pixels 데이터(0.0 ~ 1.0 범위의 float)를 다시 unsigned char 데이터(0~255)로 변환.
    std::vector<unsigned char> outBuffer(width * height * channels);

    for (int i = 0; i < width * height; i++)
    {
        // pixels에 저장되어있는 RGB값을 0~1 사이로 보정
        outBuffer[i * channels] = static_cast<unsigned char>(std::clamp(pixels[i].v[0], 0.0f, 1.0f) * 255.0f);
        outBuffer[i * channels + 1] = static_cast<unsigned char>(std::clamp(pixels[i].v[1], 0.0f, 1.0f) * 255.0f);
        outBuffer[i * channels + 2] = static_cast<unsigned char>(std::clamp(pixels[i].v[2], 0.0f, 1.0f) * 255.0f);
    }

    int result = stbi_write_png(filepath.c_str(), width, height, channels, outBuffer.data(), width * channels);
    return (result != 0);
}

bool Image::BoxBlur5()
{
    std::vector<Vec4> pixelsBuffer(this->pixels.size());

    for (int j = 0; j < this->height; j++)
    {
        for (int i = 0; i < this->width; i++)
        {
            Vec4 neighborColorSum{ 0.0f, 0.0f, 0.0f, 1.0f };
            for (int si = 0; si < 5; si++)
            {
                Vec4 neighborColor = this->GetPixel(i + si - 2, j);
                neighborColorSum.v[0] += neighborColor.v[0];
                neighborColorSum.v[1] += neighborColor.v[1];
                neighborColorSum.v[2] += neighborColor.v[2];
            }

            pixelsBuffer[i + this->width * j].v[0] = neighborColorSum.v[0] * 0.2f;
            pixelsBuffer[i + this->width * j].v[1] = neighborColorSum.v[1] * 0.2f;
            pixelsBuffer[i + this->width * j].v[2] = neighborColorSum.v[2] * 0.2f;
        }
    }

    std::swap(this->pixels, pixelsBuffer);

    for (int j = 0; j < this->height; j++)
    {
        for (int i = 0; i < this->width; i++)
        {
            Vec4 neighborColorSum{ 0.0f, 0.0f, 0.0f, 1.0f };
            for (int sj = 0; sj < 5; sj++)
            {
                Vec4 neighborColor = this->GetPixel(i, j + sj - 2);
                neighborColorSum.v[0] += neighborColor.v[0];
                neighborColorSum.v[1] += neighborColor.v[1];
                neighborColorSum.v[2] += neighborColor.v[2];
            }

            pixelsBuffer[i + this->width * j].v[0] = neighborColorSum.v[0] * 0.2f;
            pixelsBuffer[i + this->width * j].v[1] = neighborColorSum.v[1] * 0.2f;
            pixelsBuffer[i + this->width * j].v[2] = neighborColorSum.v[2] * 0.2f;
        }
    }

    std::swap(this->pixels, pixelsBuffer);

    return true;
}

bool Image::BoxBlur5Multi()
{
    DWORD dwWorkerThreadCount = 0;

    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    DWORD dwLogicalCores = sysInfo.dwNumberOfProcessors;

    dwWorkerThreadCount = dwLogicalCores;

    if (dwLogicalCores >= 128)
        dwWorkerThreadCount = 256;
    else if (dwLogicalCores >= 32)
        dwWorkerThreadCount = 64;
    else if (dwLogicalCores >= 8)
        dwWorkerThreadCount = 16;
    else
        dwWorkerThreadCount = 4;

    // dwWorkerThreadCount는 4의 제곱수여야 함 (4, 16, 64, 256)
    int divisions = static_cast<int>(sqrt(static_cast<double>(dwWorkerThreadCount)));
    if (divisions * divisions != static_cast<int>(dwWorkerThreadCount))
    {
        divisions = 2; // 기본적으로 4분할
        dwWorkerThreadCount = 4;
    }

    // 각 영역의 크기를 계산
    int regionWidth = width / divisions;
    int regionHeight = height / divisions;

    THREAD_ARG_IMAGE* pArgList = new THREAD_ARG_IMAGE[dwWorkerThreadCount];
    HANDLE* phCompleteEventList = new HANDLE[dwWorkerThreadCount];

    int threadIndex = 0;
    for (int r = 0; r < divisions; r++)
    {
        for (int c = 0; c < divisions; c++)
        {
            THREAD_ARG_IMAGE& arg = pArgList[threadIndex];
            arg.threadIndex = threadIndex;
            arg.startX = c * regionWidth;
            arg.endX = (c == divisions - 1) ? width : (c + 1) * regionWidth;
            arg.startY = r * regionHeight;
            arg.endY = (r == divisions - 1) ? height : (r + 1) * regionHeight;
            arg.pImage = this;
            arg.hCompleteEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
            phCompleteEventList[threadIndex] = arg.hCompleteEvent;

            UINT ThreadID = 0;
            arg.hThread = (HANDLE)_beginthreadex(nullptr, 0, ThreadFunc, &arg, 0, &ThreadID);

            threadIndex++;
        }
    }

    // 모든 스레드의 완료 이벤트를 기다림
    WaitForMultipleObjects(dwWorkerThreadCount, phCompleteEventList, TRUE, INFINITE);

    // 핸들 정리
    for (DWORD i = 0; i < dwWorkerThreadCount; i++)
    {
        CloseHandle(pArgList[i].hThread);
        CloseHandle(pArgList[i].hCompleteEvent);
    }
    delete[] phCompleteEventList;
    delete[] pArgList;

    return true;
}

void Image::ProcessBoxBlurRegion(int startX, int endX, int startY, int endY)
{
    int regionW = endX - startX;
    int regionH = endY - startY;
    std::vector<Vec4> regionBuffer(regionW * regionH);

    for (int j = startY; j < endY; j++)
    {
        for (int i = startX; i < endX; i++)
        {
            Vec4 neighborSum{ 0.0f, 0.0f, 0.0f, 1.0f };
            // 5픽셀 간 평균; 중앙을 기준으로 좌우 2픽셀씩
            for (int si = 0; si < 5; si++)
            {
                Vec4 neighbor = GetPixel(i + si - 2, j);
                neighborSum.v[0] += neighbor.v[0];
                neighborSum.v[1] += neighbor.v[1];
                neighborSum.v[2] += neighbor.v[2];
            }
            int index = (i - startX) + (j - startY) * regionW;
            regionBuffer[index].v[0] = neighborSum.v[0] * 0.2f;
            regionBuffer[index].v[1] = neighborSum.v[1] * 0.2f;
            regionBuffer[index].v[2] = neighborSum.v[2] * 0.2f;
            regionBuffer[index].v[3] = 1.0f;
        }
    }

    // 결과를 원본 이미지에 복사
    for (int j = startY; j < endY; j++)
    {
        for (int i = startX; i < endX; i++)
        {
            int index = (i - startX) + (j - startY) * regionW;
            pixels[i + j * width] = regionBuffer[index];
        }
    }
}

Vec4& Image::GetPixel(int i, int j)
{
    // 배열 범위를 벗어나지 않도록 보정
    i = std::clamp(i, 0, width - 1);
    j = std::clamp(j, 0, height - 1);
    return pixels[i + j * width];
}

UINT __stdcall Image::ThreadFunc(void* pArg)
{
    THREAD_ARG_IMAGE* pThreadArg = static_cast<THREAD_ARG_IMAGE*>(pArg);
    pThreadArg->pImage->ProcessBoxBlurRegion(pThreadArg->startX, pThreadArg->endX, pThreadArg->startY, pThreadArg->endY);
    SetEvent(pThreadArg->hCompleteEvent);
    return 0;
}
