#pragma once
#include "pch.h"
#include <vector>
#include <algorithm>
#include <string>

class Image;

struct Vec4
{
	float v[4]; // v: R, v: G, v: B, v: A
};

class Image
{
public:
	Image(int width, int height, int channels);

	// �޸� �� ������(�̹��� ������ ��ü ����Ʈ ������)�� �Է¹޾� ���� �迭�� ä��.
	// data�� stbi_load�� �Ҵ�� �޸𸮸� ����Ű��, �Լ� ���ο��� �����մϴ�.
	bool ReadFromMemory(const unsigned char* data);
	bool WritePNG(const std::string& filepath);
	bool BoxBlur5();
	bool BoxBlur5Multi();
	void ProcessBoxBlurRegion(int startX, int endX, int startY, int endY);

	int GetWidth() const { return width; }
	int GetHeight() const { return height; }

	// (i, j)�� ���� ��� ���� �ε��� (0 <= i < width, 0 <= j < height)
	Vec4& GetPixel(int i, int j);

	static UINT __stdcall ThreadFunc(void* pArg);

private:
	std::vector<Vec4> pixels;
	int width = 0;
	int height = 0;
	int channels = 0;
};

struct THREAD_ARG_IMAGE
{
	HANDLE hThread;
	HANDLE hCompleteEvent;
	int threadIndex;
	int startX;
	int endX;
	int startY;
	int endY;
	Image* pImage = nullptr;
};