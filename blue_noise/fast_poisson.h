#pragma once
#include <vector>
#include <math.h>

using namespace std;

#define PI 3.14159265

namespace FastPoisson
{
	struct Grid
	{
		vector< vector<int>> data;
		float cellSize;
		int worldW;
		int worldH;

		int gridW;
		int gridH;

		// grid w and grid h, not domian w and domian h
		void Init(int domainW, int domainH, float cellSizeIn)
		{
			cellSize = cellSizeIn;

			worldW = domainW;
			worldH = domainH;

			// floor or ceil?
			gridW = ceil(domainW / cellSize);
			gridH = ceil(domainH / cellSize);

			data.resize(gridH);

			// y x
			for (int y = 0; y < gridH; y++)
			{
				data[y].resize(gridW, -1);
			}
		}

		void AddPoint(ivec2 p, int index)
		{
			AddPoint(p.x, p.y, index);
		}

		void AddPoint(int x, int y, int index)
		{
			int gx = floor(x / cellSize);
			int gy = floor(y / cellSize);

			data[gy][gx] = index;
		}

		void RemovePoint(int x, int y, int index)
		{
			int gx = floor(x / cellSize);
			int gy = floor(y / cellSize);

			if (data[gy][gx] == index)
			{
				data[gy][gx] = -1;
			}
			else
			{
				assert(false);
			}
		}


		ivec2 worldPosToGridCoord(ivec2 point)
		{
			int gx = floor(point.x / cellSize);
			int gy = floor(point.y / cellSize);

			return { gx, gy };
		}

		ivec2 clampGridCoord(ivec2 gc)
		{
			int gx = max(0, gc.x);
			gx = min(gx, (int)data[0].size() - 1);

			int gy = max(0, gc.y);
			gy = min(gy, (int)data.size() - 1);

			return { gx, gy };
		}

		ivec2 clampWorldPos(ivec2 wp)
		{
			int wx = max(0, wp.x);
			wx = min(wx, worldW - 1);

			int wy = max(0, wp.y);
			wy = min(wy, worldH - 1);

			return { wx, wy };
		}
	};



	bool IsInsideRing(ivec2 center, ivec2 point, int r, int r2)
	{
		int rSquared = r * r;
		int r2Squared = r2 * r2;

		int distSquared = utl::GetDistSquared(center, point);

		return rSquared <= distSquared && distSquared < r2Squared;
	}

	bool CheckAgainstActivePoints(vector<ivec2> existingPoints, ivec2 point, int r, Grid grid, int imgW, int imgH)
	{
		int rSquared = r * r;

		// find point within the gird
		ivec2 gc = grid.worldPosToGridCoord(point);

		// grab all neighboring cells
		ivec2 minGc = { gc.x - 1, gc.y - 1 };
		minGc = grid.clampGridCoord(minGc);

		ivec2 maxGc = { gc.x + 1, gc.y + 1 };
		maxGc = grid.clampGridCoord(maxGc);

		// check with each one 

		for (int y = minGc.y; y < maxGc.y + 1; y++)
		{
			for (int x = minGc.x; x < maxGc.x + 1; x++)
			{
				int index = grid.data[y][x];
				if (index != -1)
				{
					ivec2 neighbor = existingPoints[index];
					int distSquared = utl::GetDistSquared(neighbor, point);

					if (distSquared < rSquared)
					{
						return false;
					}
				}
			}
		}

		return true;
		/*
		for (int i = 0; i < activePoints.size(); i++)
		{
			int distSquared = utl::GetDistSquared(activePoints[i], point);

			if (distSquared < rSquared)
			{
				return false;
			}
		}
		return true;
		*/
	}

	ivec2 GenerateRandomPolarPoint(int r, int r2, ivec2 center)
	{
		float randR = utl::randFloat(r, r2);
		float angle = utl::randFloat(0, 360);

		float x = cos(angle * PI / 180);
		float y = sin(angle * PI / 180);

		float rx = randR * x;
		float ry = randR * y;

		float px = (float)center.x + rx;
		float py = (float)center.y + ry;

		return { (int)px, (int)py };
	}


	bool TryGenerateNewPoints(ivec2 center, int r, int r2, int k, Grid& grid, int imgW, int imgH, 
		vector<ivec2>& existingPoints, ivec2& output)
	{
		vector<ivec2> results;

		ivec2 min{ center.x - r2, center.y - r2 };
		ivec2 max{ center.x + r2, center.y + r2 };

		min = grid.clampWorldPos(min);
		max = grid.clampWorldPos(max);

	//	cout << min << endl;
	//	cout << max << endl;

		bool found = false;

		vector<ivec2> candidates;
		// try k times
		int count = k;
		while (count > 0)
		{
			int x = utl::randInt(min.x, max.x);
			int y = utl::randInt(min.y, max.y);
			ivec2 point{ x, y };
	//		cout << "x " << x << " y " << y << endl;

	//		ivec2 point = GenerateRandomPolarPoint(r, r2, center);
			point = grid.clampWorldPos(point);

			if (IsInsideRing(center, point, r, r2))
			{
				if (CheckAgainstActivePoints(existingPoints, point, r, grid, imgW, imgH))
				{
					output = point;
					found = true;
				}

			}

			count--;
		}

		return found;
	}
};