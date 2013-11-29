#include <hash_map>
#include "DTEDsegment.h"
#include <utility>
#include <glm/glm.hpp>
#include <GL/gl.h>

struct pairhash{
	enum {              
		bucket_size = 4,  // 0 < bucket_size
		min_buckets = 8}; // min_buckets = 2 ^^ N, 0 < N

	size_t operator()(const intpair &p) const {
		return
			std::hash<int>()(p.first*100) ^ std::hash<int>()(p.second);
	}

	bool operator()(const intpair &left, const intpair &right)
	{
		if (left.first==right.first)
			return left.second < right.second;
		return left.first < right.first;
	}
};

typedef std::hash_map<intpair, DTEDsegment*, pairhash> segmentmap;

const float defaultData[] = {
	0, 0, -100,
	1200, 0, -100,
	1200, 1200, -100,
	0, 1200, -100
};

class segmentmanager
{
public:
	segmentmap segments;
	segmentmanager(void);
	~segmentmanager(void);

	GLuint defaultVertexBuffer;
	GLuint defaultArrayID;

	void loadsegment(std::string filename, bool toGPU = true);
	void loadrange(int down, int up, int left, int right, std::string path = "",  bool toGPU = true);
	void drawAll(int LOD, GLuint *indices, GLuint parlocation, glm::vec3 center, int ylow, int yhigh, int xlow, int xhigh);
};

