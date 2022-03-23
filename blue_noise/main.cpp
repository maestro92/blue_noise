
#include <fstream>
#include "define.h"
#include "utl_math.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

#include "fast_poisson.h"



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


// consider wrap around
int GetClosestDistSquared(vector<ivec2> existingPoints, ivec2 point, int maxW, int maxH)
{
	int curMinDistSquared = INT_MAX;

	int halfWidth = maxW / 2;
	int halfHeight = maxH / 2;

	for (int i = 0; i < existingPoints.size(); i++)
	{
		int distSquared = utl::GetDistSquared(existingPoints[i], point, maxW, maxH, true);

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

// Mitchell's best candidate N^2
void generateBlueNoiseSampling_BestCandidate(Image srcImg, int numSamples)
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


void generateBlueNoiseSampling_DartThrowing(Image srcImg, int numSamples)
{
	float sqrtN = sqrt(2);
	float cellSize = 4 / sqrtN;


}

// https://www.youtube.com/watch?v=flQgnCUxHlw&ab_channel=TheCodingTrain
void generateBlueNoiseSampling_RobertBridson(Image srcImg, int numSamples)
{
	int r = 4; // 4 pixels apart
	int n = 2;	// this is the dimension, 2 for our case
	float sqrtN = sqrt(n);

	int k = 30;	// limit of samples before rejection in the algorithm
	float cellSize = r / sqrtN;

	// step 0
	FastPoisson::Grid grid;
	grid.Init(srcImg.w, srcImg.h, cellSize);

	// select a random point
	int x = utl::randInt(0, srcImg.w - 1);
	int y = utl::randInt(0, srcImg.h - 1);
	
	vector<ivec2> activePoints;

	// step 1
	int index = activePoints.size();
	activePoints.push_back({x, y});
	grid.AddPoint(x, y, index);


	// step 2
	for (int i = 0; i < 100; i++)
	{
		int randIndex = utl::randInt(0, activePoints.size()-1);
		ivec2 point = activePoints[randIndex];

		ivec2 newPoint;
		if (TryGenerateNextPoint(point, r, r*2, k, grid, srcImg.w, srcImg.h, activePoints, newPoint))
		{
			int index2 = activePoints.size();
			activePoints.push_back(newPoint);
			grid.AddPoint(newPoint.x, newPoint.y, index2);
		}
		else
		{
			activePoints.erase(activePoints.begin() + randIndex);
		}

	}
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
	generateBlueNoiseSampling_BestCandidate(img, total);

	cout << "end of program" << endl;

	while (1)
	{

	}
	
	return 0;
}






