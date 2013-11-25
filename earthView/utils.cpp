#include <stdlib.h>
#include <sstream>
#include "utils.h"

float randFloat(float min, float max)
{
	float random = ((float) rand()) / (float) RAND_MAX;
	return min + (random * (max-min));
}

void generateSurfaceOrder( int x, int y, unsigned *order, int jump)
{
	int i = 0;
	int last = 0;
	for (int hor = 0;; hor+=jump)	//y/jump - 1
	{
		if (hor > y-jump-1) hor = y-jump-1;
		
		if ((hor/jump)%2==0)
		{
			for (int ver = 0;; ver+=jump)
			{
				if (ver >= x-1) {
					last = ver - jump;
					ver = x-1;
				}
				order[i++] = hor*y+ver;
				order[i++] = (hor+jump)*y+ver;
				if (ver == x-1) break;
			}
		}
		else
		{
			order[i++] = hor*y+x-1;
			order[i++] = (hor+jump)*y+x-1;
			for (int ver = last;; ver-=jump)
			{
				if (ver < 0) ver = 0;
				order[i++] = hor*y+ver;
				order[i++] = (hor+jump)*y+ver;
				if (ver == 0) break;
			}
		}
		if (hor == y-jump-1)
			break;
	}
}


template<class T>
std::string tostr<T>(T data)
{
	std::stringstream conv;
	conv << data;
	return conv.str();
}
