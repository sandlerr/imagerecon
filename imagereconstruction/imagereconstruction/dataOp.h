class angleSet {
	int projectionNum;
	float* projection;
public:
	angleSet();
	angleSet(HANDLE angleFile);
	int getProjectionNum();
	float getProjection(int num);
	LPCWSTR getProjectionList();
};