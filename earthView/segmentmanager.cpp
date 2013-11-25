#include "segmentmanager.h"
#include <string>
#include <sstream>
#include "utils.h"


segmentmanager::segmentmanager(void)
{
	
}


segmentmanager::~segmentmanager(void)
{
	for (segmentmap::iterator it = segments.begin(); it != segments.end(); ++it)
	{
		delete it->second;
	}
}

void segmentmanager::loadsegment( std::string filename, bool toGPU)
{
	std::string path(filename);
	std::string fname;

	size_t pos = path.find_last_of("\\");
	if(pos != std::string::npos)
		fname.assign(path.begin() + pos + 1, path.end());
	else
		fname = path;

	int updown, leftright;
	bool north, east;
	char c;

	std::stringstream conv(fname);
	conv >> c;
    north = c=='N' || c=='n';
	conv >> updown;
	conv >> c;
	east = c=='E' || c=='e';
	conv >> leftright;
	updown *= north?1:-1;
	leftright *= east?1:-1;	
	intpair position = intpair(updown, leftright);

	DTEDsegment* seg = new DTEDsegment(path, position, toGPU);
	if (seg->FileLoaded)
		segments[position] = seg;
	else delete seg;
}

void segmentmanager::drawAll( int LOD, GLuint *indices, GLuint parlocation, glm::vec3 center )
{
	for (segmentmap::iterator it = segments.begin(); it != segments.end(); ++it)
	{
		//if (abs(center.x - center.z)-2<it->second->coordinates.first && abs(center.x - center.z)+2>it->second->coordinates.first  && abs(center.y - center.z)-2<it->second->coordinates.second)
			it->second->Draw(LOD, indices, parlocation);
	}
}

void segmentmanager::loadrange(int down, int up, int left, int right, std::string path, bool toGPU /*= true*/ )
{
	for (int i = down; i <= up; i++)
	{
		for (int j = left; j <= right; j++)
		{
			std::string filename(path);
			filename += i>=0? 'n' : 's';
			if (abs(i) < 10)
				filename += '0';
			if (abs(i) == 0)
				filename += '0';
			filename += tostr(abs(i));
			filename += j >=0? 'e' : 'w';
			if (abs(j) < 100)
				filename += '0';
			if (abs(j) < 10)
				filename += '0';
			filename += tostr(abs(j));
			filename += ".hgt";
			loadsegment(filename, toGPU);
		}
	}
}
