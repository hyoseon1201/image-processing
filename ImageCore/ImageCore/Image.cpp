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

    // pixels ���� ũ�� ����: width * height
    pixels.resize(width * height);

    // �� �ȼ� ������ ���� (�⺻������ RGB 3ä�� ����)
    for (int i = 0; i < width * height; i++)
    {
        pixels[i].v[0] = data[i * channels] / 255.0f;
        pixels[i].v[1] = data[i * channels + 1] / 255.0f;
        pixels[i].v[2] = data[i * channels + 2] / 255.0f;
        pixels[i].v[3] = 1.0f;
    }

    // stbi_load�� �Ҵ���� �޸��̹Ƿ�, �ݵ�� stbi_image_free�� �����մϴ�.
    stbi_image_free(const_cast<unsigned char*>(data));

    return true;
}

bool Image::WritePNG(const std::string& filepath)
{
    // pixels ������(0.0 ~ 1.0 ������ float)�� �ٽ� unsigned char ������(0~255)�� ��ȯ.
    std::vector<unsigned char> outBuffer(width * height * channels);

    for (int i = 0; i < width * height; i++)
    {
        // pixels�� ����Ǿ��ִ� RGB���� 0~1 ���̷� ����
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

#pragma omp parallel for
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

#pragma omp parallel for
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

Vec4& Image::GetPixel(int i, int j)
{
    // �迭 ������ ����� �ʵ��� ����
    i = std::clamp(i, 0, width - 1);
    j = std::clamp(j, 0, height - 1);
    return pixels[i + j * width];
}