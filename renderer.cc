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
#include <stdio.h>
#include <stdlib.h>
#include "color.h"
#include "mandelbox.h"
#include "camera.h"
#include "vector3d.h"
#include "3d.h"
#include <omp.h>

extern "C"{
  extern double getTime();
  extern void   printProgress( double perc, double time );
  extern int    UnProject(double winX, double winY, CameraParams camP, double *obj);

}

extern void rayMarch (const RenderParams &render_params, const vec3 &from, const vec3  &to, pixelData &pix_data);
extern vec3 getColour(const pixelData &pixData, const RenderParams &render_params,
											const vec3 &from, const vec3  &direction);

void renderFractal(const CameraParams &camera_params, const RenderParams &renderer_params, 
					unsigned char* image)
{
	printf("rendering fractal...\n");

	double farPoint[3];
	vec3 from;

	from.SetDoublePoint(camera_params.camPos);

	int height = renderer_params.height;
	int width  = renderer_params.width;



	#pragma omp parallel
	{
		vec3 to;
		vec3 *samples;
		pixelData pix_data;
		if( renderer_params.super_sampling == 1 )
			samples = (vec3*)malloc(sizeof(vec3)*9);
		double time = getTime();
		int i, j;

		int tid = omp_get_thread_num();
		int nthreads = omp_get_num_threads();
		for(j = tid; j < height; j+=nthreads)
		{
			//for each column pixel in the row
			for(i = 0; i <width; i++)
			{
				vec3 color;
				if( renderer_params.super_sampling == 1 )
				{
					int idx = 0;
					//TODO: Can be unrolled
					for(int ssj = -1; ssj < 2; ssj++){
						color = (samples[0]*0.05 + samples[1]*0.1 + samples[2]*0.05 + 
						samples[3]*0.1  + samples[4]*0.4 + samples[5]*0.1  + 
						samples[6]*0.05 + samples[7]*0.1 + samples[8]*0.05);
					}

					
					// color = (samples[0]*0.05 + samples[1]*0.1 + samples[2]*0.05 + 
					// samples[3]*0.1  + samples[4]*0.4 + samples[5]*0.1  + 
					// samples[6]*0.05 + samples[7]*0.1 + samples[8]*0.05);

					// color = (samples[0]*0.05 + samples[1]*0.1 + samples[2]*0.05 + 
					// samples[3]*0.1  + samples[4]*0.4 + samples[5]*0.1  + 
					// samples[6]*0.05 + samples[7]*0.1 + samples[8]*0.05);

					// color = (samples[0]*0.05 + samples[1]*0.1 + samples[2]*0.05 + 
					// samples[3]*0.1  + samples[4]*0.4 + samples[5]*0.1  + 
					// samples[6]*0.05 + samples[7]*0.1 + samples[8]*0.05);
				}
				else
				{
					// get point on the 'far' plane
					// since we render one frame only, we can use the more specialized method
					UnProject(i, j, camera_params, farPoint);

					// to = farPoint - camera_params.camPos
					to = SubtractDoubleDouble(farPoint,camera_params.camPos);
					to.Normalize();

					//render the pixel
					rayMarch(renderer_params, from, to, pix_data);

					//get the colour at this pixel
					color = getColour(pix_data, renderer_params, from, to);
				}

				//save colour into texture
				int k = (j * width + i)*3;
				image[k+2] = (unsigned char)(color.x * 255);
				image[k+1] = (unsigned char)(color.y * 255);
				image[k]   = (unsigned char)(color.z * 255);
			}
			if (tid == 0)
				printProgress((j+1)/(double)height,getTime()-time);
		}

		if( renderer_params.super_sampling == 1 )
			free(samples);
	}
	printf("\n rendering done:\n");
}
