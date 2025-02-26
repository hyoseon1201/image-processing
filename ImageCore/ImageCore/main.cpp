#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#include "image.h"

int main()
{
    // stbi_load�� �̿��� �̹��� ������ �޸𸮷� �о�ɴϴ�.
    int imgWidth = 0, imgHeight = 0, imgChannels = 0;
    const char* filename = R"(E:\image-processing\ImageCore\image_2.jpg)";

    unsigned char* imgData = stbi_load(filename, &imgWidth, &imgHeight, &imgChannels, 0);
    assert(imgData);

    // �о�� �̹��� ������ �� ä�� ������ �̿��� Image ��ü ����
    Image image(imgWidth, imgHeight, imgChannels);

    // stbi_load�� ���� �̹��� �����͸� ���������� ��ȯ�ϴ� �Լ� ȣ��
    assert(image.ReadFromMemory(imgData));

    // image ������ �ڽ��� 10ȸ �׽�Ʈ
    for (int i = 0; i < 10; i++)
        image.BoxBlur5();

    // ����ȭ�鿡 �̹��� ����
    const char* outputDataPath = R"(C:\Users\hyoseon\Desktop\output.png)";
    assert(image.WritePNG(outputDataPath));
    
    return 0;
}