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
#include <iostream>
#include <fstream>
#include <cstring>
#include "vector3d.h"
#include "color.h"
#include <random>
#include <time.h>
using namespace std;

#define PI 3.14159265

const double sqrt_mach_eps = 1.4901e-08;
const double sqrt_third = sqrt(1.0/(double)3.0);
const double MIN_DIST = 0.05;
// = new vec3[14];
vec3 camBody [14];



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
CameraParams    camera_params;

extern double rayMarch (const vec3 &from, const vec3  &to, pixelData &pix_data);
void printParams(int iteration);
vec3 getRandomPos(){
	return vec3((rand()/(double)RAND_MAX)*10.0-5.0, (rand()/(double)RAND_MAX)*10.0-5.0, (rand()/(double)RAND_MAX)*10.0-5.0);
}
bool avoidWall(vec3& posistion, vec3& movement, int& count);
//double getDistanceEstimate(vec3& from, vec3& direction,pixelData& pix_data);

typedef struct {
	vec3 pos;
	vec3 look;
	vec3 lookTarget;
	vec3 move;
	double moveLength;
} Camera;

int main(int argc, char** argv)
{
	srand (time(NULL));
	//RenderParams    renderer_params;
	
	camBody[0] = vec3(1,0,0);
	camBody[1] = vec3(-1,0,0);
	camBody[2] = vec3(0,1,0);
	camBody[3] = vec3(0,-1,0);
	camBody[4] = vec3(0,0,1);
	camBody[5] = vec3(0,0,-1);
	camBody[6] = vec3(sqrt_third, sqrt_third, sqrt_third);
	camBody[7] = vec3(-sqrt_third, sqrt_third, sqrt_third);
	camBody[8] = vec3(-sqrt_third, -sqrt_third, sqrt_third);
	camBody[9] = vec3(-sqrt_third, sqrt_third, -sqrt_third);
	camBody[10] = vec3(-sqrt_third, -sqrt_third, -sqrt_third);
	camBody[11] = vec3(sqrt_third, -sqrt_third, sqrt_third);
	camBody[12] = vec3(sqrt_third, -sqrt_third, -sqrt_third);
	camBody[13] = vec3(sqrt_third, sqrt_third, -sqrt_third);

	vec3 avoid;
	bool isTooClose = false;
	double distance;
	
	getParameters(argv[1], &camera_params, &renderer_params, &mandelBox_params);
	renderer_params.eps = pow((float)10.0, renderer_params.detail);
	int image_size = renderer_params.width * renderer_params.height;
	//unsigned char *image = (unsigned char*)malloc(3*image_size*sizeof(unsigned char));

	//initial set params
	//init3D(&camera_params, &renderer_params);
	//initDE(mandelBox_params);

	// for first 30 seconds, only look at image
	init3D(&camera_params, &renderer_params);
	initDE(mandelBox_params);
	int i;
	for (i = 0; i<900;i++){
		mandelBox_params.rMin = 0.5 + 0.25*sin((i/450.0)*PI);
		mandelBox_params.rFixed = 1.5 + 0.5*sin((i/900.0)*PI);
		
		printParams(i);
	}
	i=900;
	Camera cam;
	pixelData pix_data;

	cam.pos.x = camera_params.camPos[0];
	cam.pos.y = camera_params.camPos[1];
	cam.pos.z = camera_params.camPos[2];

	cam.look.x = camera_params.camTarget[0];
	cam.look.y = camera_params.camTarget[1];
	cam.look.z = camera_params.camTarget[2];

	cam.move.x = camera_params.camTarget[0];
	cam.move.y = camera_params.camTarget[1];
	cam.move.z = camera_params.camTarget[2];


	double min_dist = 0.1;
	cam.move = vec3(5,6,6);//vec3(7,6,5);
	//cam.move = getRandomPos();//SubractDoubleDouble(cam.pos, cam.move).Normalize()*SubractDoubleDouble(cam.pos, cam.move).Magnitude()/300;
	cam.move = cam.move - cam.pos;
	cam.move = cam.move/300.0;
	for (; i<1200;i++) {
		mandelBox_params.rMin = 0.5 + 0.25*sin((i/450.0)*PI);
		mandelBox_params.rFixed = 1.5 + 0.5*sin((i/900.0)*PI);
		cam.pos += cam.move;
		camera_params.fov -= 0.8/300.0;

		camera_params.camPos[0] = cam.pos.x;
		camera_params.camPos[1] = cam.pos.y;
		camera_params.camPos[2] = cam.pos.z;

		
		
		printParams(i);
	}
		//lower fov inside box
		//pick new params
			//if on camera dest, pick new camera dest, random, 
			//unless outside 6.0 to 6.0, then set to 0,0,0
			//rotate slowly to camera dest

			//change mandelbox
				//sin modify rMin and rFixed

			//Follow a ray
				//if none, pick a ray
				//if DE on ray < eps, mirror on surface of last DE, increase displacement on bounce until DE last ray > 2*eps and DE new ray > 2 * eps
				//if end of ray is reached, last 5%, pick new random ray within 5.0,5.0,5.0
		//generate string name for file
		//generate params file
			//cam.lookTarget = cam.look;
	camera_params.fov = 0.3;
	for (; i<7200;i++) {
		mandelBox_params.rMin = 0.5 + 0.25*sin((i/450.0)*PI);
		mandelBox_params.rFixed = 1.5 + 0.5*sin((i/900.0)*PI);

		if (i%300==0) {

			//cam.lookTarget = getRandomPos();

			cam.lookTarget = getRandomPos() - cam.lookTarget;//.Normalize();
			cam.lookTarget = cam.lookTarget/600.0;
			//cam.look.Normalize();
			if(i == 1200) cam.move = vec3(0,0,0);
			else cam.move = getRandomPos();
			cam.move = cam.move - cam.pos;
			cam.move.Normalize();
			cam.move = cam.move/600.0;
			//cam.moveLength = (cam.move).Magnitude();
		}
		cam.look+=cam.lookTarget;
		//cam.pos+=cam.move/300.0;
		camera_params.camTarget[0] = cam.look.x;
		camera_params.camTarget[1] = cam.look.y;
		camera_params.camTarget[2] = cam.look.z;
		
		int count = 1;
		while (!avoidWall(cam.pos, cam.move, count)) {}
		
		camera_params.camPos[0] = cam.pos.x;
		camera_params.camPos[1] = cam.pos.y;
		camera_params.camPos[2] = cam.pos.z;



		printParams(i);
	}

	//renderFractal(camera_params, renderer_params, image);
	
	//saveBMP(renderer_params.file_name, image, renderer_params.width, renderer_params.height);
	
	//free(image);

	return 0;
}

bool avoidWall(vec3& position, vec3& movement, int& count) {
	vec3 avoid;
	bool isTooClose = false;
	double distance;
	pixelData pix_data;
	avoid = vec3(0,0,0);

	for (int r = 0; r < 14; r++) {
			distance = rayMarch(position, position+camBody[r], pix_data);
			if (!pix_data.escaped && distance < MIN_DIST) {
				isTooClose = true;
				avoid -= camBody[r];
			}
		}

		if (fabs(position.x) > 5.0) {movement.x = -movement.x; position.x = copysign(5.0, position.x); count = 1; return false;}
			if (fabs(position.y) > 5.0) {movement.y = -movement.y; position.y = copysign(5.0, position.y); count = 1; return false;}
			if (fabs(position.z) > 5.0) {movement.z = -movement.z; position.z = copysign(5.0, position.z); count = 1; return false;}
		if (isTooClose) {

			avoid.Normalize();
			avoid = avoid/600.0;

			
			if(( fabs(avoid.x) < sqrt_mach_eps && fabs(avoid.y) < sqrt_mach_eps && fabs(avoid.z) < sqrt_mach_eps)){if (count == 1){movement = -movement;} position +=movement;count++;}//movement*=2;}
			else{if (count ==1){movement = avoid;} position += movement; count++;}
			printf("X: %lf, Y: %lf, X %lf\n", movement.x, movement.y, movement.z);
			//position += movement;
			
			
			return false;//avoidWall(position, movement);
		} else {
			
			position += movement;
			return true;
		}
}

// double getDistanceEstimate(vec3& from, vec3& direction){
// 	double dist = 0.0;
//   double totalDist = 0.0;
  
//   // We will adjust the minimum distance based on the current zoom
//   //double eps = pow((float)10.0, render_params.detail);
//   double eps = render_params.eps;
//   double epsModified = 0.0;
  
//   int steps;
//   vec3 p;
//   for ( steps = 0; steps < render_params.maxRaySteps; steps++) 
//   {      
//     p = from + direction * totalDist;
//     dist = MandelBoxDE(p);
//     totalDist += 0.9921875*dist;
    
//     epsModified = totalDist*eps;
//     if (dist < epsModified || totalDist > render_params.maxDistance) 
//       break;
//   }
  
//   //vec3 hitNormal;
//   if (dist < epsModified) 
//   {
//     //we didnt escape
//     pix_data.escaped = false;
    
//     // We hit something, or reached MaxRaySteps
//     pix_data.hit = p;
    
//     //figure out the normal of the surface at this point
//     vec3 normPos = p - direction * epsModified;
//     normal(normPos, pix_data.normal, MandelBoxDE(normPos));
//   }
//   else 
//     //we have the background colour
//     pix_data.escaped = true;
// }


void printParams(int iteration) {
	FILE * myfile;
	char paramsPrefix [15] = "params/params";
	char paramsSuffix [5]= ".dat";
	char imagePrefix [9] = "images/f";
	char imageSuffix [5]= ".bmp";
	char num[5];
	char path [30];
	char imagePath[30];


	//set number char[]
	if (iteration<10) sprintf(num,"000%d", iteration);
	else if (iteration<100) sprintf(num,"00%d", iteration);
	else if (iteration<1000) sprintf(num,"0%d", iteration);
	else sprintf(num,"%d", iteration);
	//set path
	strcpy(path, paramsPrefix);
	strcat(path,num);
	strcat(path,paramsSuffix);
	puts(path);

	strcpy(imagePath, imagePrefix);
	strcat(imagePath, num);
	strcat(imagePath, imageSuffix);

	myfile=fopen(path,"w");
	//fprintf(myfile, "# CAMERA\n");
	fprintf(myfile, "# location  x,y,z\n%lf %lf %lf\n", camera_params.camPos[0], camera_params.camPos[1], camera_params.camPos[2]);
	fprintf(myfile, "# look_at (target)  x,y,z\n%lf %lf %lf\n", camera_params.camTarget[0], camera_params.camTarget[1], camera_params.camTarget[2]);
	fprintf(myfile, "# up vector x,y,z\n%lf %lf %lf\n", camera_params.camUp[0], camera_params.camUp[1], camera_params.camUp[2]);
	fprintf(myfile, "# field of view\n%lf\n", camera_params.fov);
	//fprintf(myfile, "# IMAGE\n");
	fprintf(myfile, "# width height\n%d %d\n", renderer_params.width, renderer_params.height);
	fprintf(myfile, "# detail level\n%lf\n", renderer_params.detail);
	//fprintf(myfile, "# MANDELBOX\n");
	fprintf(myfile, "# scale, rMin, rFixed\n%lf %lf %lf\n", mandelBox_params.scale, mandelBox_params.rMin, mandelBox_params.rFixed);
	fprintf(myfile, "# max number of iterations, escape time\n%d %lf\n", mandelBox_params.num_iter, mandelBox_params.escape_time);
	//fprintf(myfile, "# COLORING\n");
	fprintf(myfile, "# type 0 or 1\n%d\n", renderer_params.colourType);
	fprintf(myfile, "# brightness\n%lf\n", renderer_params.brightness);
	fprintf(myfile, "# super sampling\n%d\n", renderer_params.super_sampling);
	//fprintf(myfile, "# IMAGE FILE NAME\n");
	fprintf(myfile, imagePath);


  fclose(myfile);

}
// # CAMERA
// # location  x,y,z   (7,7,7)
// 14.0 8.0 10.0
// # look_at (target)  x,y,z
// 0 0 0 
// # up vector x,y,z;  (0, 1, 0)
// 0 1 0
// # field of view     (1)
// 1.1  
// # IMAGE
// # width height
// 3840 2160
// # detail level, the smaller the more detailed (-3)
// -3.5
// # MANDELBOX
// # scale, rMin, rFixed  (2 0.5 1)
// 2.0 0.5 1.0
// # max number of iterations, escape time
// 18 100
// # COLORING
// # type 0 or 1
// 1
// # brightness
// 1.2
// # super sampling anti aliasing 0 = off, 1 = on
// 0
// # IMAGE FILE NAME
// 2.0_0.5_1.0.bmp