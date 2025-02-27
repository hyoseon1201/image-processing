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

    // 읽어온 이미지 사이즈 및 채널 정보를 이용해 Image 객체 생성
    Image image(imgWidth, imgHeight, imgChannels);

    // stbi_load로 얻은 이미지 데이터를 내부적으로 변환하는 함수 호출
    image.ReadFromMemory(imgData);

    LARGE_INTEGER	PrvCounter = {};
    float	fElapsedTick = 0.0f;
    DWORD	dwCount = 0;

    wprintf_s(L"<Single Thread>\n");
    dwCount = 0;
    PrvCounter = QCGetCounter();
    // image 데이터 박스블러 10회 테스트
    for (int i = 0; i < 10; i++)
        image.BoxBlur5();

    fElapsedTick = QCMeasureElapsedTick(QCGetCounter(), PrvCounter);
    wprintf_s(L"Result: %u, SingleThread - %.2fms Elapsed.\n", dwCount, fElapsedTick);

    const char* outputDataPath = R"(C:\Users\hyoseon\Desktop\output.png)";
    image.WritePNG(outputDataPath);

    wprintf_s(L"<Multi Thread>\n");
    dwCount = 0;
    PrvCounter = QCGetCounter();
    // image 데이터 박스블러 10회 멀티쓰레딩으로 테스트
    for (int i = 0; i < 10; i++)
        image.BoxBlur5Multi();
    fElapsedTick = QCMeasureElapsedTick(QCGetCounter(), PrvCounter);
    wprintf_s(L"Result: %u, MultiThread - %.2fms Elapsed.\n", dwCount, fElapsedTick);

    // 바탕화면에 이미지 저장
    outputDataPath = R"(C:\Users\hyoseon\Desktop\outputmul.png)";
    image.WritePNG(outputDataPath);
    
    return 0;
}