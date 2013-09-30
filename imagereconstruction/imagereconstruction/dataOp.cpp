#include "stdafx.h"
#include "dataOp.h"
#include <string>
#define NUMPROJLENGTH 3
#define READSIZE 4096

angleSet::angleSet()
{

}
angleSet::angleSet(HANDLE angleFile)
{
	DWORD bytesRead = NULL;
	char numProjChar[NUMPROJLENGTH] = "\0\0";
	char buffer[READSIZE]={0};
	std::string str,substring;
	int lasthit=0;
	int thisproj=0;

	ReadFile(angleFile,numProjChar,NUMPROJLENGTH,&bytesRead,NULL);
	projectionNum = atoi(numProjChar);
	projection = new float[projectionNum];
	ReadFile(angleFile,buffer,READSIZE,&bytesRead,NULL);
	str = std::string(buffer);
	for (int i=1;i<(int)bytesRead;i++)
	{
		if (buffer[i] == '\r')
		{
			substring = str.substr(lasthit+1,i-lasthit-1);
			projection[thisproj] = atof(substring.c_str());
			thisproj++;
			lasthit=i;
		}
	}
	CloseHandle(angleFile);
}
int angleSet::getProjectionNum()
{
	return projectionNum;
}
float angleSet::getProjection(int num)
{
	return projection[num];
}