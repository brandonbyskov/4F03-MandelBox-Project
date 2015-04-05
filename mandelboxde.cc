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
#include <cmath>
#include <cstdio>
#include <algorithm>
#include <assert.h>
#include "color.h"
#include "mandelbox.h"

using namespace std;
extern MandelBoxParams mandelBox_params;
MandelBoxParams& params = mandelBox_params;
inline double SQR( double x) { return x*x; }

inline double component_fold(double x) 
{
	if (x>1.0) x = 2.0-x;
	else if (x<-1.0) x = -2.0-x;
	return x;
}

inline static void boxFold(vec3 &v)
{
	v.x =  component_fold(v.x);
	v.y =  component_fold(v.y);
	v.z =  component_fold(v.z);
}

static void sphereFold(vec3 &v, double r2, double rMin2, double rFixed2)
{
	if (r2 < rMin2) 
		v = v*(rFixed2/rMin2);
	else if (r2 < rFixed2)
		v = v*(rFixed2/r2);
}


double MandelBoxDE(const vec3 &p0)
{
	vec3 p = p0;
	//printf("here");
	double rMin2   = params.rMin2;
	double rFixed2 = params.rFixed2;
	double escape  = params.escape;
	float scale   = params.scale;
	double dfactor = 1.0; 
	double r2=-1.0;

	// double c1 = params.c1;
	// double c2 = params.c2;

	float fabs_scale = params.fabs_scale;
	double scale1_a = params.scale1_a;
	double scale1_b = params.scale1_b;
	double scale2;

	for (int i=0; i < params.num_iter && r2 <= escape; i++) 
	{
		// box fold
		boxFold(p);
		r2 = p.Dot(p);      

		// sphere fold and estimate distance
		if (r2 < rMin2) {
			p.FMA(scale1_a,p0);
			dfactor = dfactor * scale1_b + 1.0;
		}
		else if (r2 < rFixed2) {
			scale2 = (rFixed2/r2);
			p.FMA(scale2*scale,p0);
			dfactor = dfactor*scale2*fabs_scale+1.0;
		}
		else {
			p.FMA(scale,p0);
			dfactor = dfactor*fabs_scale+1.0;
		}
		//assert(dfactor>0);

		// scale
		//p = p*scale+p0;
		
		//if ( r2 > escape ) break;		
	}
	//r2 = p.Magnitude();
	//assert(r2>=0);
	return (p.Magnitude() - params.c1) / dfactor - params.c2;
}


void initDE(MandelBoxParams &params) {
  params.rMin2   = SQR(params.rMin);
  params.rFixed2 = SQR(params.rFixed);
  params.escape  = SQR(params.escape_time);
  //params.scale   = params.scale;
  //params.dfactor = 1; 
  //params.r2=-1;

  params.c1 = fabs(params.scale - 1.0);
  params.c2 = pow( fabs(params.scale), 1 - params.num_iter);

  params.fabs_scale = fabs(params.scale);
  params.scale1_a = (params.rFixed2/params.rMin2)*params.scale;
  params.scale1_b = (params.rFixed2/params.rMin2) * params.fabs_scale;
  //params.scale2;
}