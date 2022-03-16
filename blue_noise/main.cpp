
#include <fstream>
#include "define.h"
#include "utility_sdl.h"
#include "utility_math.h"
#include <png++/png.hpp>

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
	png::image< png::rgba_pixel > image(width, height);

	for (int y = 0; y < image.get_height(); ++y)
	{
		for (int x = 0; x < image.get_width(); ++x)
		{
			int ps = pixelIndex2PixelStart(width, x, y);
			image[y][x] = png::rgba_pixel(pixels[ps+0], pixels[ps+1], pixels[ps+2], pixels[ps+3]);
		}
	}

	image.write(filename);
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

void GenerateRegularSampling(SDL_Surface* srcImg, int numSamplesX, int numSamplesY)
{
	uint8* srcImgData = (uint8*)(srcImg->pixels);

	int numBytes = srcImg->w * srcImg->h * 4;
	uint8* dstImgData = new uint8[numBytes];
	memset(dstImgData, 0, numBytes);

	int xStride = srcImg->w / numSamplesX;
	int yStride = srcImg->h / numSamplesY;

	for (int y = 0; y < srcImg->h; y++)
	{
		for (int x = 0; x < srcImg->w; x++)
		{
			// pixel index --> byte index
			int pixelIndex = y * srcImg->w + x;

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

	createPNGImage(outputPath + "regular_sampling.png", dstImgData, srcImg->w, srcImg->h);
}

void ResetImage(uint8* imgData, int numPixels)
{
	for (int i = 0; i < numPixels; i++)
	{
		ResetPixelValue(imgData, i);
	}
}

void generateWhiteNoiseSampling(SDL_Surface* srcImg, int numSamples)
{
	uint8* srcImgData = (uint8*)(srcImg->pixels);

	int numPixels = srcImg->w * srcImg->h;
	int numBytes = numPixels * 4;
	uint8* newImgData = new uint8[numBytes];
	memset(newImgData, 0, numBytes);

	ResetImage(newImgData, numPixels);

	bool* flags = new bool[numPixels];
	memset(flags, 0, numPixels);

	int count = numSamples;

	while (count)
	{
		int x = utl::randInt(0, srcImg->w - 1);
		int y = utl::randInt(0, srcImg->h - 1);

		int pixelIndex = y * srcImg->w + x;

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

	createPNGImage(outputPath + "white_noise_sampling.png", newImgData, srcImg->w, srcImg->h);
}

void generateBlueNoiseSampling(SDL_Surface* img, int numSamples)
{

}
/*
void runDXT3(string img)
{
	string core = "";

	for (int i = 0; i < img.size() - 4; i++)
	{
		core += img[i];
	}
	cout << core << endl;

	string image0Name = img;
	SDL_Surface* image0 = utl::loadSDLImage(inputPath + image0Name);
	createPNGImage(outputPath + image0Name, (uint8*)(image0->pixels), image0->w, image0->h);


	// assuming 4:1 compression ratio
	int numBytes = image0->w * image0->h * 4;
	int numCompressedBytes = numBytes / 4;
	uint8* compressedImage0Pixels = new uint8[numCompressedBytes];
	memset(compressedImage0Pixels, 0, numCompressedBytes);

	DXTConverter dxtConverter;
	dxtConverter.compressDXT3((uint8*)image0->pixels, (uint8*)compressedImage0Pixels, image0->w, image0->h);

	string binFilePath = outputPath + core + "_dxt3.bin";
	writeBinFile(binFilePath, compressedImage0Pixels, numCompressedBytes);

	streampos size;
	char* compressedImageBinaryData = NULL;
	ifstream readFile(binFilePath.c_str(), ios::in | ios::binary | ios::ate);
	if (readFile.is_open())
	{
		size = readFile.tellg();
		compressedImageBinaryData = new char[size];
		readFile.seekg(0, ios::beg);
		readFile.read(compressedImageBinaryData, size);
		readFile.close();
	}

	uint8* newImage0Pixels = new uint8[numBytes];
	memset(newImage0Pixels, 0, numBytes);
	dxtConverter.decompressDXT3((uint8*)compressedImageBinaryData, newImage0Pixels, image0->w, image0->h);

	string decompressFileName = core + "_dxt3_decompress.png";
	string decompressFilePath = outputPath + decompressFileName;
	createPNGImage(decompressFilePath.c_str(), newImage0Pixels, image0->w, image0->h);

	cout << "testDXT3 Done " << endl;
}


void runDXT5(string img)
{
	string core = "";

	for (int i = 0; i < img.size() - 4; i++)
	{
		core += img[i];
	}
	cout << core << endl;

	string image0Name = img;
	SDL_Surface* image0 = utl::loadSDLImage(inputPath + image0Name);
	createPNGImage(outputPath + image0Name, (uint8*)(image0->pixels), image0->w, image0->h);

	// assuming 4:1 compression ratio
	int numBytes = image0->w * image0->h * 4;
	int numCompressedBytes = numBytes / 4;
	uint8* compressedImage0Pixels = new uint8[numCompressedBytes];
	memset(compressedImage0Pixels, 0, numCompressedBytes);


	uint8* debugPixels = new uint8[numBytes];


	DXTConverter dxtConverter;
	dxtConverter.compressDXT5((uint8*)image0->pixels, (uint8*)compressedImage0Pixels, image0->w, image0->h);



	string binFilePath = outputPath + core + "_dxt5.bin";
	writeBinFile(binFilePath, compressedImage0Pixels, numCompressedBytes);

	streampos size;
	char* compressedImageBinaryData = NULL;
	ifstream readFile(binFilePath.c_str(), ios::in | ios::binary | ios::ate);
	if (readFile.is_open())
	{
		size = readFile.tellg();
		compressedImageBinaryData = new char[size];
		readFile.seekg(0, ios::beg);
		readFile.read(compressedImageBinaryData, size);
		readFile.close();
	}


	uint8* newImage0Pixels = new uint8[numBytes];
	memset(newImage0Pixels, 0, numBytes);
	dxtConverter.decompressDXT5((uint8*)compressedImageBinaryData, newImage0Pixels, image0->w, image0->h);

	string decompressFileName = core + "_dxt5_decompress.png";
	string decompressFilePath = outputPath + decompressFileName;
	createPNGImage(decompressFilePath.c_str(), newImage0Pixels, image0->w, image0->h);

	cout << "testDXT5 Done " << endl;
}
*/

/*

void TestGetAlphaBYBlock(string img)
{

	string core = "";

	for (int i = 0; i < img.size() - 4; i++)
	{
		core += img[i];
	}
	cout << core << endl;


	string image0Name = img;
	SDL_Surface* image0 = utl::loadSDLImage(inputPath + image0Name);


	copyImageAlphaChannel("alphaSample.bmp", (uint8*)image0->pixels, image0->w, image0->h);

	// assuming 8:1 compression ratio
	int numBytes = image0->w * image0->h * 4;
	int numCompressedBytes = numBytes / 2;
	uint8* compressedImage0Pixels = new uint8[numCompressedBytes];
	memset(compressedImage0Pixels, 0, numCompressedBytes);


	uint8* newImage0Pixels = new uint8[numBytes];
	memset(newImage0Pixels, 0, numBytes);

	DXTConverter dxtConverter;
	dxtConverter.DebugCompressTest((uint8*)image0->pixels, (uint8*)compressedImage0Pixels, newImage0Pixels, image0->w, image0->h);
	dxtConverter.DebugDecompressTest((uint8*)compressedImage0Pixels, newImage0Pixels, image0->w, image0->h);

	//	createImage(decompressFilePath.c_str(), newImage0Pixels, image0->w, image0->h);
	createPNGImage("alpha_smokeTest.png", newImage0Pixels, image0->w, image0->h);
//	copyImageAlphaChannel("alpha_smokeTest.bmp", newImage0Pixels, image0->w, image0->h);

	cout << "runDXT1 Done " << endl;
}
*/

struct ivec2
{
	int x;
	int y;
};

ivec2 GetNumSamples(SDL_Surface* img)
{
	int w = img->w / 4;
	int h = img->h / 4;
	
	return { w, h };
}


int main(int argc, char *argv[])
{	
	SDL_Surface* screen;
	utl::initSDL(SCREEN_WIDTH, SCREEN_HEIGHT, screen);
	
	string imgFile = "lena.png";
	SDL_Surface* image0 = utl::loadSDLImage(inputPath + imgFile);

	ivec2 samples = GetNumSamples(image0);
	int total = samples.x * samples.y;

	GenerateRegularSampling(image0, samples.x, samples.y);
	generateWhiteNoiseSampling(image0, total);
	generateBlueNoiseSampling(image0, total);

	return 0;
}






