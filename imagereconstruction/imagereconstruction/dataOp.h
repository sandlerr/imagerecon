class angleSet {
public:
	angleSet();
	angleSet(HANDLE angleFile);
	int getProjectionNum();
	float getProjection(int num);

private:
	int projectionNum;
	float* projection;
};