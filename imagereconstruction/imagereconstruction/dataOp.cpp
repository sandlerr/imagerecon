#include "stdafx.h"
#include "dataOp.h"
#include <iostream>
#include <string>
#include <cmath>

#define NUMPROJLENGTH 3
#define READSIZE 5000
using namespace std;

angleSet::angleSet()
{

}
angleSet::angleSet(HANDLE angleFile)
{
	DWORD bytesRead = NULL;
	char numProjChar[NUMPROJLENGTH] = "\0\0";
	char buffer[READSIZE]={0};
	string str,substring;
	size_t index;
	size_t lasthit=0;
	int thisproj=0;

	ReadFile(angleFile,numProjChar,NUMPROJLENGTH,&bytesRead,NULL);
	projectionNum = atoi(numProjChar);
	//while(ReadFile(angleFile,buffer,READSIZE,&bytesRead,NULL))
	//{
	ReadFile(angleFile,buffer,READSIZE,&bytesRead,NULL);
		str = string(buffer);
		index=0;
		lasthit=0;
		while(index!=string::npos)
		{
			index = str.find("\r",lasthit+1);
			if (index==string::npos)
				break;
			else
			{
				projection.resize(projection.size()+1);
				substring = str.substr(lasthit+1,index-lasthit-1);
				cout << substring;
				projection.at(thisproj) = atof(substring.c_str());
				cout << projection[thisproj];
				thisproj++;
				lasthit=index;
			}
		}
	//}
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
vector<char *> angleSet::getProjectionList()
{
	vector<char *> angleList(projection.size());
	char temp[6];
	double dtemp;
	for(int i=0;i<projection.size();i++)
	{
		dtemp=projection.at(i);
		sprintf(temp,"%4.3f",dtemp);
		printf("%4.3f\n",dtemp);
		memcpy(&angleList.at(i),temp,5);
		//angleList.at(i) = temp;
	}
	return angleList;
}