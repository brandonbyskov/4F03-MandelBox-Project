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
#include <assert.h>
#include <algorithm>
#include <stdio.h>
#include <math.h>

#include "color.h"
#include "renderer.h"

extern RenderParams renderer_params;
RenderParams& render_params = renderer_params;

extern double MandelBoxDE(const vec3 &p);
inline void normal (const vec3 & p, vec3 & normal, double dist);

//double eps = pow((float)10.0, render_params.detail);
double rayMarch(const vec3 &from, const vec3  &direction,
	      pixelData& pix_data)
{
  double dist = 0.0;
  double totalDist = 0.0;
  
  // We will adjust the minimum distance based on the current zoom
  //double eps = pow((float)10.0, render_params.detail);
  double eps = render_params.eps;
  double epsModified = 0.0;
  
  int steps;
  vec3 p;
  for ( steps = 0; steps < render_params.maxRaySteps; steps++) 
  {      
    p = from + direction * totalDist;
    dist = MandelBoxDE(p);
    totalDist += 0.9921875*dist;
    
    epsModified = totalDist*eps;
    if (dist < epsModified || totalDist > render_params.maxDistance) 
      break;
  }
  
  //vec3 hitNormal;
  if (dist < epsModified) 
  {
    //we didnt escape
    pix_data.escaped = false;
    
    // We hit something, or reached MaxRaySteps
    pix_data.hit = p;
    
    //figure out the normal of the surface at this point
    vec3 normPos = p - direction * epsModified;
    normal(normPos, pix_data.normal, MandelBoxDE(normPos));
  }
  else 
    //we have the background colour
    pix_data.escaped = true;

  return totalDist;
}

const double sqrt_mach_eps = 1.4901e-08;

void normal(const vec3 & p, vec3 & normal, double dist)
{
  // compute the normal at p
  

  double eps = std::max( p.Magnitude(), 1.0 )*sqrt_mach_eps;

  // vec3 e1(eps, 0,   0);
  // vec3 e2(0  , eps, 0);
  // vec3 e3(0  , 0, eps);
  
  normal = vec3(MandelBoxDE(p+vec3(eps,0,0))-dist, MandelBoxDE(p+vec3(0,eps,0))-dist, MandelBoxDE(p+vec3(0,0,eps))-dist);
  
  //normal = vec3(MandelBoxDE(p+e1)-MandelBoxDE(p-e1), MandelBoxDE(p+e2)-MandelBoxDE(p-e2), MandelBoxDE(p+e3)-MandelBoxDE(p-e3));
  normal.Normalize();
}
