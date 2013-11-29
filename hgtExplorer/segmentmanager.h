#include <hash_map>
#include "DTEDsegment.h"
#include <utility>
#include <glm/glm.hpp>

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


class segmentmanager
{
public:
	segmentmap segments;
	segmentmanager(void);
	~segmentmanager(void);

	void loadsegment(std::string filename, bool toGPU = true);
	void loadrange(int down, int up, int left, int right, std::string path = "",  bool toGPU = true);
	void drawAll(int LOD, GLuint *indices, GLuint parlocation, glm::vec3 center);
};

