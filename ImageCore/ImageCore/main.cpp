#include "pch.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#include "image.h"
#include "QueryPerfCounter.h"

int main()
{
    QCInit();

    int imgWidth = 0, imgHeight = 0, imgChannels = 0;
    const char* filename = R"(E:\image-processing\ImageCore\image_2.jpg)";

    unsigned char* imgData = stbi_load(filename, &imgWidth, &imgHeight, &imgChannels, 0);
    assert(imgData);

    // �о�� �̹��� ������ �� ä�� ������ �̿��� Image ��ü ����
    Image image(imgWidth, imgHeight, imgChannels);

    // stbi_load�� ���� �̹��� �����͸� ���������� ��ȯ�ϴ� �Լ� ȣ��
    image.ReadFromMemory(imgData);

    LARGE_INTEGER	PrvCounter = {};
    float	fElapsedTick = 0.0f;
    DWORD	dwCount = 0;

    wprintf_s(L"<Single Thread>\n");
    dwCount = 0;
    PrvCounter = QCGetCounter();
    // image ������ �ڽ��� 10ȸ �׽�Ʈ
    for (int i = 0; i < 10; i++)
        image.BoxBlur5();

    fElapsedTick = QCMeasureElapsedTick(QCGetCounter(), PrvCounter);
    wprintf_s(L"Result: %u, SingleThread - %.2fms Elapsed.\n", dwCount, fElapsedTick);

    const char* outputDataPath = R"(C:\Users\hyoseon\Desktop\output.png)";
    image.WritePNG(outputDataPath);

    wprintf_s(L"<Multi Thread>\n");
    dwCount = 0;
    PrvCounter = QCGetCounter();
    // image ������ �ڽ��� 10ȸ ��Ƽ���������� �׽�Ʈ
    for (int i = 0; i < 10; i++)
        image.BoxBlur5Multi();
    fElapsedTick = QCMeasureElapsedTick(QCGetCounter(), PrvCounter);
    wprintf_s(L"Result: %u, MultiThread - %.2fms Elapsed.\n", dwCount, fElapsedTick);

    // ����ȭ�鿡 �̹��� ����
    outputDataPath = R"(C:\Users\hyoseon\Desktop\outputmul.png)";
    image.WritePNG(outputDataPath);
    
    return 0;
}