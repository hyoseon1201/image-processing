#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#include "image.h"

int main()
{
    // stbi_load를 이용해 이미지 파일을 메모리로 읽어옵니다.
    int imgWidth = 0, imgHeight = 0, imgChannels = 0;
    const char* filename = R"(E:\image-processing\ImageCore\image_2.jpg)";

    unsigned char* imgData = stbi_load(filename, &imgWidth, &imgHeight, &imgChannels, 0);
    assert(imgData);

    // 읽어온 이미지 사이즈 및 채널 정보를 이용해 Image 객체 생성
    Image image(imgWidth, imgHeight, imgChannels);

    // stbi_load로 얻은 이미지 데이터를 내부적으로 변환하는 함수 호출
    assert(image.ReadFromMemory(imgData));

    // image 데이터 박스블러 10회 테스트
    for (int i = 0; i < 10; i++)
        image.BoxBlur5();

    // 바탕화면에 이미지 저장
    const char* outputDataPath = R"(C:\Users\hyoseon\Desktop\output.png)";
    assert(image.WritePNG(outputDataPath));
    
    return 0;
}