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

	// 메모리 내 데이터(이미지 파일의 전체 바이트 데이터)를 입력받아 내부 배열에 채움.
	// data는 stbi_load로 할당된 메모리를 가리키며, 함수 내부에서 해제합니다.
	bool ReadFromMemory(const unsigned char* data);
	bool WritePNG(const std::string& filepath);
	bool BoxBlur5();

	int GetWidth() const { return width; }
	int GetHeight() const { return height; }

	// (i, j)는 좌측 상단 기준 인덱스 (0 <= i < width, 0 <= j < height)
	Vec4& GetPixel(int i, int j);
private:
	std::vector<Vec4> pixels;
	int width = 0;
	int height = 0;
	int channels = 0;
};