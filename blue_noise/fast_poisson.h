#pragma once
#include <vector>

using namespace std;

namespace FastPoisson
{
	struct Grid
	{
		vector< vector<int>> grid;
		float cellSize;

		// grid w and grid h, not domian w and domian h
		void Init(int domainW, int domainH, float cellSizeIn)
		{
			cellSize = cellSizeIn;

			// floor or ceil?
			int gridW = ceil(domainW / cellSize);
			int gridH = ceil(domainH / cellSize);

			vector< vector<int>> grid(gridH);

			// y x
			for (int y = 0; y = gridH; y++)
			{
				grid[y].resize(gridW, -1);
			}
		}


		void AddPoint(int x, int y, int index)
		{
			int gx = floor(x / cellSize);
			int gy = floor(y / cellSize);

			grid[gy][gx] = index;
		}
	};

	bool IsInsideRing(ivec2 center, ivec2 point, int r, int r2)
	{
		int rSquared = r * r;
		int r2Squared = r2 * r2;

		int distSquared = utl::GetDistSquared(center, point);

		return rSquared <= distSquared && distSquared < r2Squared;
	}

	bool CheckAgainstActivePoints(vector<ivec2> activePoints, ivec2 point, int r)
	{
		int rSquared = r * r;
		for (int i = 0; i < activePoints.size(); i++)
		{
			int distSquared = utl::GetDistSquared(activePoints[i], point);

			if (distSquared > rSquared)
			{
				return true;
			}
		}
		return false;
	}

	bool TryGenerateNextPoint(ivec2 center, int r, int r2, int k, Grid grid, int imgW, int imgH, 
		vector<ivec2> activePoints, ivec2& output)
	{
		vector<ivec2> results;

		ivec2 min{ center.x - r2, center.y - r2 };
		ivec2 max{ center.x + r2, center.y + r2 };

		vector<ivec2> candidates;
		// try k times
		int count = k;
		while (count > 0)
		{
			int x = utl::randInt(min.x, max.x);
			int y = utl::randInt(min.y, max.y);

			ivec2 point{ x, y };
			if (IsInsideRing(center, point, r, r2))
			{
				if (CheckAgainstActivePoints(activePoints, point, r))
				{
					output = point;
					return true;
				}

				count--;
			}
		}

		return false;
	}
};