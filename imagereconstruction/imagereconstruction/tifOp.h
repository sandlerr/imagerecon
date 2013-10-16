#include <tiffio.h>
class tifImage {
	TIFF* tif;
	int projectionCount;
public:
	tifImage();
	tifImage(TIFF* tifFile);
	int getProjectionNum();
	~tifImage();
};