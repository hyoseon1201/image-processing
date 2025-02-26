#pragma once
#include <vector>
#include <algorithm> // std::clamp
#include <string>

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

	int GetWidth() const { return width; }
	int GetHeight() const { return height; }

	// (i, j)�� ���� ��� ���� �ε��� (0 <= i < width, 0 <= j < height)
	Vec4& GetPixel(int i, int j);
private:
	std::vector<Vec4> pixels;
	int width = 0;
	int height = 0;
	int channels = 0;
};