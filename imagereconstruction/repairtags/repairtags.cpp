// repairtags.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <stdio.h>
#include <stdlib.h>

#define BUFELEM 40
#define FILENAME "fbpatlas.tif"

int _tmain(int argc, _TCHAR* argv[])
{
  size_t num;
  unsigned long count;
  unsigned char buf[BUFELEM];
  FILE *fp;

  if ((fp = fopen(FILENAME, "rb")) != NULL)
  {
    count = 0;
    fread(buf, 1, 4, fp);
    if (buf[0] == 0x49 && buf[1] == 0x49)
    {
      if (buf[2] == 42)
      {
        if (buf[3]==0)
      }
    }
    do
    {
      num = fread(buf, 1, BUFELEM, fp);
      ++count;
      /* do sth. with buffer contents here [1] */
    } while (num == BUFELEM);

    /* Error checking snipped for brevity */

    fclose(fp);
    printf("calls to fread: %ld\n", count);
  }
  else
  {
    fprintf(stderr, "Error opening file %s\n", FILENAME);
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
