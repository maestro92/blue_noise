
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
}

void SetPixelValue(uint8* dst, int pixelIndex, ivec4 color)
{
	int byteIndex = pixelIndex * 4;

	dst[byteIndex] = color.x;
	dst[byteIndex + 1] = color.y;
	dst[byteIndex + 2] = color.z;
	dst[byteIndex + 3] = color.w;
}

void ResetPixelValue(uint8* dst, int pixelIndex)
{
	SetPixelValue(dst, pixelIndex, { 0,0,0,255 });
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


void renderGridHorizontalLine(Image srcImg, int y, uint8* newImgData)
{
	for (int x = 0; x < srcImg.w; x++)
	{
		int pixelIndex = y * srcImg.w + x;

		SetPixelValue(newImgData, pixelIndex, { 255,255,255,255 });
	}
}

void renderGridVerticalLine(Image srcImg, int x, uint8* newImgData)
{
	for (int y = 0; y < srcImg.h; y++)
	{
		int pixelIndex = y * srcImg.w + x;

		SetPixelValue(newImgData, pixelIndex, { 255,255,255,255 });
	}
}


void renderGrid(FastPoisson::Grid grid, Image srcImg, uint8* newImgData)
{
	for (int y = 0; y < srcImg.h; y += grid.cellSize)
	{
		renderGridHorizontalLine(srcImg, y, newImgData);
	}

	for (int x = 0; x < srcImg.w; x += grid.cellSize)
	{
		renderGridVerticalLine(srcImg, x, newImgData);
	}
}


void AppendToVector(vector<ivec2>& list0, vector<ivec2>& list1)
{
	for (int i = 0; i < list1.size(); i++)
	{
		list0.push_back(list1[i]);
	}
}


// https://www.youtube.com/watch?v=flQgnCUxHlw&ab_channel=TheCodingTrain
void generateBlueNoiseSampling_RobertBridson(Image srcImg, int numSamples)
{
	// numSamples = 25;
	int r = 4; // 4 pixels apart
	int n = 2;	// this is the dimension, 2 for our case
	float sqrtN = sqrt(n);

	int k = 30;	// limit of samples before rejection in the algorithm
	float cellSize = r / sqrtN;

	// step 0
	FastPoisson::Grid grid;
	grid.Init(srcImg.w, srcImg.h, cellSize);

	utl::setRandSeed(0);

	// select a random point
	int x = utl::randInt(0, srcImg.w - 1);
	int y = utl::randInt(0, srcImg.h - 1);
	
	ivec2 point = { x, y };

	cout << "x " << x << " y " << y << ", cell size " << grid.cellSize << endl;

	vector<ivec2> activePoints;
	vector<ivec2> outputSamplePoints;

	// step 1
	outputSamplePoints.push_back(point);
	int index = activePoints.size();
	activePoints.push_back(point);
	grid.AddPoint(x, y, index);


	int i = 0;

	while(0 < activePoints.size() && outputSamplePoints.size() < numSamples)
	{
		int randIndex = utl::randInt(0, activePoints.size()-1);
		ivec2 point = activePoints[randIndex];
		
		cout << "i " << i << endl;

		cout << "			 randIndex " << randIndex << ": " << point << endl;
		
		ivec2 output;
		if (FastPoisson::TryGenerateNewPoints(point, r, r*2, k, grid, srcImg.w, srcImg.h, outputSamplePoints, output))
		{
			int index2 = outputSamplePoints.size();
			activePoints.push_back(output);
			outputSamplePoints.push_back(output);
			grid.AddPoint(output.x, output.y, index2);
		}
		else
		{
			activePoints.erase(activePoints.begin() + randIndex);
		}

		if (i == 578)
		{
		//	break;
		}

		i++;
	}


	uint8* srcImgData = (uint8*)(srcImg.pixels);

	int numPixels = srcImg.w * srcImg.h;
	int numBytes = numPixels * 4;
	uint8* newImgData = new uint8[numBytes];
	memset(newImgData, 0, numBytes);

	ResetImage(newImgData, numPixels);

	// renderGrid(grid, srcImg, newImgData);


	for (int i = 0; i < outputSamplePoints.size(); i++)
	{
		ivec2 point = outputSamplePoints[i];

		int pixelIndex = point.y * srcImg.w + point.x;

		CopyPixelValue(newImgData, srcImgData, pixelIndex);
	}

	createPNGImage(outputPath + "blue_noise_sampling_2.png", newImgData, srcImg.w, srcImg.h);
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
	// generateBlueNoiseSampling_BestCandidate(img, total);	too slow
	generateBlueNoiseSampling_RobertBridson(img, total);

	cout << "end of program" << endl;

	while (1)
	{

	}
	
	return 0;
}






