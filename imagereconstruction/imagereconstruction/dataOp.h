#include <vector>
class angleSet {
	int projectionNum;
	std::vector<double> projection;
	std::vector<char> angleList;
public:
	angleSet();
	angleSet(HANDLE angleFile);
	int getProjectionNum();
	float getProjection(int num);
	std::vector<char *> getProjectionList();
};