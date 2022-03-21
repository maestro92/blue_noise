
#include <fstream>
#include "define.h"
#include "utility_math.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

struct ivec2
{
	int x;
	int y;
};

struct Image
{
	int w = 0;
	int h = 0;
	int channels = 0;
	uint8* pixels;
};

using namespace std;
// http://stackoverflow.com/questions/4845410/error-lnk2019-unresolved-external-symbol-main-referenced-in-function-tmainc
#undef main

string inputPath = "testImages/srcFolder/";
string outputPath = "testImages/myOutput/";

int pixelIndex2PixelStart(int width, int px, int py)
{
	return (py * width + px) * 4;
}


void createPNGImage(string filename, uint8* pixels, int width, int height)
{
	stbi_write_png(filename.c_str(), width, height, 4, pixels, 0);
}

void CopyPixelValue(uint8* dst, uint8* src, int pixelIndex)
{
	int byteIndex = pixelIndex * 4;
	memcpy((void*)(dst+byteIndex), (void*)(src+byteIndex), 4);
	/*
	newImgData[byteIndex] = ((uint8*)(srcImg->pixels))[byteIndex];
	newImgData[byteIndex + 1] = ((uint8*)(srcImg->pixels))[byteIndex + 1];
	newImgData[byteIndex + 2] = ((uint8*)(srcImg->pixels))[byteIndex + 2];
	newImgData[byteIndex + 3] = ((uint8*)(srcImg->pixels))[byteIndex + 3];
	*/
}

void ResetPixelValue(uint8* dst, int pixelIndex)
{
	int byteIndex = pixelIndex * 4;

	dst[byteIndex] = 0;
	dst[byteIndex + 1] = 0;
	dst[byteIndex + 2] = 0;
	dst[byteIndex + 3] = 255;
}


void GenerateRegularSampling(Image srcImg, int numSamplesX, int numSamplesY)
{
	uint8* srcImgData = (uint8*)(srcImg.pixels);

	int numBytes = srcImg.w * srcImg.h * 4;
	uint8* dstImgData = new uint8[numBytes];
	memset(dstImgData, 0, numBytes);

	int xStride = srcImg.w / numSamplesX;
	int yStride = srcImg.h / numSamplesY;

	for (int y = 0; y < srcImg.h; y++)
	{
		for (int x = 0; x < srcImg.w; x++)
		{
			// pixel index --> byte index
			int pixelIndex = y * srcImg.w + x;

			if (y % yStride == 0 && x % xStride == 0)
			{
				CopyPixelValue(dstImgData, srcImgData, pixelIndex);
			}
			else
			{
				ResetPixelValue(dstImgData, pixelIndex);
			}
		}
	}

	createPNGImage(outputPath + "regular_sampling.png", dstImgData, srcImg.w, srcImg.h);
}
#if 1
void ResetImage(uint8* imgData, int numPixels)
{
	for (int i = 0; i < numPixels; i++)
	{
		ResetPixelValue(imgData, i);
	}
}

void generateWhiteNoiseSampling(Image srcImg, int numSamples)
{
	uint8* srcImgData = srcImg.pixels;

	int numPixels = srcImg.w * srcImg.h;
	int numBytes = numPixels * 4;
	uint8* newImgData = new uint8[numBytes];
	memset(newImgData, 0, numBytes);

	ResetImage(newImgData, numPixels);

	bool* flags = new bool[numPixels];
	memset(flags, 0, numPixels);

	int count = numSamples;

	while (count)
	{
		int x = utl::randInt(0, srcImg.w - 1);
		int y = utl::randInt(0, srcImg.h - 1);

		int pixelIndex = y * srcImg.w + x;

		if (flags[pixelIndex])
		{
			continue;
		}
		else
		{
			CopyPixelValue(newImgData, srcImgData, pixelIndex);

			flags[pixelIndex] = true;
			count--;
		}
	}

	createPNGImage(outputPath + "white_noise_sampling.png", newImgData, srcImg.w, srcImg.h);
}

// consider Wrap around
int GetDist(ivec2 p0, ivec2 p1, int maxW, int maxH)
{
	int halfW = maxW / 2;
	int halfH = maxH / 2;

	int dx = abs(p0.x - p1.x);
	int dy = abs(p0.y - p1.y);

	if (dx > halfW)
		dx = maxW - dx;

	if (dy > halfH)
		dy = maxH - dy;

	return dx * dx + dy * dy;
}

// consider wrap around
int GetClosestDistSquared(vector<ivec2> existingPoints, ivec2 point, int maxW, int maxH)
{
	int curMinDistSquared = INT_MAX;

	int halfWidth = maxW / 2;
	int halfHeight = maxH / 2;

	for (int i = 0; i < existingPoints.size(); i++)
	{
		int distSquared = GetDist(existingPoints[i], point, maxW, maxH);

		if (distSquared < curMinDistSquared)
		{
			curMinDistSquared = distSquared;
		}
	}

	return curMinDistSquared;
}


ivec2 ChooseNextBlueNoisePoint(vector<ivec2> existingPoints, vector<ivec2> candidatePoints, int maxW, int maxH)
{
	int curMaxDistSquared = -1;
	ivec2 bestCandidate = { -1, -1 };
	for (int i = 0; i < candidatePoints.size(); i++)
	{
		// closest dist from existing point
		int distSquared = GetClosestDistSquared(existingPoints, candidatePoints[i], maxW, maxH);

		if (distSquared > curMaxDistSquared)
		{
			curMaxDistSquared = distSquared;
			bestCandidate = candidatePoints[i];
		}
	}

	return bestCandidate;
}


void generateBlueNoiseSampling(Image srcImg, int numSamples)
{
	uint8* srcImgData = (uint8*)(srcImg.pixels);

	int numPixels = srcImg.w * srcImg.h;
	int numBytes = numPixels * 4;
	uint8* newImgData = new uint8[numBytes];
	memset(newImgData, 0, numBytes);

	ResetImage(newImgData, numPixels);

	int count = numSamples;

	vector<ivec2> existingPoints;
	vector<ivec2> candidatePoints;


	int generationConstant = 1;

	while (count)
	{
		if (count % 10 == 0)
		{
			cout << "count is " << count << endl;
		}
		// first point
		if (count == numSamples)
		{
			int x = utl::randInt(0, srcImg.w - 1);
			int y = utl::randInt(0, srcImg.h - 1);

			int pixelIndex = y * srcImg.w + x;

			CopyPixelValue(newImgData, srcImgData, pixelIndex);
			existingPoints.push_back({x, y});
			count--;
		}
		else
		{
			int numCandidates = generationConstant * existingPoints.size();

			for (int i = 0; i < numCandidates; i++)
			{
				int x = utl::randInt(0, srcImg.w - 1);
				int y = utl::randInt(0, srcImg.h - 1);
			
				candidatePoints.push_back({ x, y });
			}
			ivec2 candidate = ChooseNextBlueNoisePoint(existingPoints, candidatePoints, srcImg.w, srcImg.h);

			assert(candidate.x != -1);

			int pixelIndex = candidate.y * srcImg.w + candidate.x;
			CopyPixelValue(newImgData, srcImgData, pixelIndex);
			existingPoints.push_back(candidate);
			count--;

			candidatePoints.clear();
		}

	}

	createPNGImage(outputPath + "blue_noise_sampling.png", newImgData, srcImg.w, srcImg.h);
}


ivec2 GetNumSamples(Image img)
{
	int w = img.w / 4;
	int h = img.h / 4;
	
	return { w, h };
}



#endif

int main(int argc, char *argv[])
{	

	
	string imgFile = "lena.png";	
	string fullImgFile = inputPath + imgFile;
	Image img;
	unsigned char* data = stbi_load(fullImgFile.c_str(), &img.w, &img.h, &img.channels, 4);
	img.pixels = (uint8*)data;

	ivec2 samples = GetNumSamples(img);
	int total = samples.x * samples.y;

	GenerateRegularSampling(img, samples.x, samples.y);
	generateWhiteNoiseSampling(img, total);
	generateBlueNoiseSampling(img, total);

	cout << "end of program" << endl;

	while (1)
	{

	}
	
	return 0;
}






