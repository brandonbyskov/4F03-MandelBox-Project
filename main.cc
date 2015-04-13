/*
   This file is part of the Mandelbox program developed for the course
    CS/SE  Distributed Computer Systems taught by N. Nedialkov in the
    Winter of 2015 at McMaster University.

    Copyright (C) 2015 T. Gwosdz and N. Nedialkov

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <stdlib.h>
#include <stdio.h>
#include "camera.h"
#include "renderer.h"
#include "mandelbox.h"
#include <math.h>
#include <cstring>
#include <time.h>

extern "C"{
void getParameters(char *filename, CameraParams *camera_params, RenderParams *renderer_params,
		   MandelBoxParams *mandelBox_paramsP);

void init3D       (CameraParams *camera_params, const RenderParams *renderer_params);
void saveBMP      (const char* filename, const unsigned char* image, int width, int height);
}
void renderFractal(const CameraParams &camera_params, const RenderParams &renderer_params, unsigned char* image);
void initDE       (MandelBoxParams &params);

MandelBoxParams mandelBox_params;
RenderParams renderer_params;

int main(int argc, char** argv)
{
  CameraParams    camera_params;
  //RenderParams    renderer_params;
  int start, end;
  int width, height;
  int start_time;
  char infile [128];
  char outfile [128];
  strcpy(infile, argv[1]);
  strcpy(outfile, argv[2]);
  start = atoi(argv[3]);
  end = atoi(argv[4]);
  char paramsPath [128];
  char imagePath [128];
  width = atoi(argv[5]);
  height = atoi(argv[6]);
  int image_size;
  unsigned char *image;
  for (int i = start; i <= end; i++) {
    sprintf(paramsPath, infile, i);
    getParameters(paramsPath, &camera_params, &renderer_params, &mandelBox_params);
    renderer_params.eps = pow((float)10.0, renderer_params.detail);
    renderer_params.width = width;
    renderer_params.height = height;
    if (i == start) {
      image_size = renderer_params.width * renderer_params.height;
      image = (unsigned char*)malloc(3*image_size*sizeof(unsigned char));
    }
    start_time = clock();
    init3D(&camera_params, &renderer_params);
    initDE(mandelBox_params);

    renderFractal(camera_params, renderer_params, image);
    sprintf(imagePath, outfile, i);
    saveBMP(imagePath, image, renderer_params.width, renderer_params.height);
    
  
    printf("%s\t%f\n",imagePath, (clock() - start_time)/(double)CLOCKS_PER_SEC);
  }
free(image);

  return 0;
}

