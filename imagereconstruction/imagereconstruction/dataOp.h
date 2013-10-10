#include <vector>
class angleSet {
	int projectionNum;
	std::vector<double> projection;
public:
	angleSet();
	angleSet(HANDLE angleFile);
	int getProjectionNum();
	float getProjection(int num);
	std::vector< std::vector<char *> > getProjectionList();
};