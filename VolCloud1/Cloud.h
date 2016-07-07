#pragma once

#include "glm/glm.hpp"
#include <string>
#include "perlin.h"
#include <vector>

using namespace std; 
using namespace glm;

#define ABSORB_COEFF 1.0f
//The distance between the eye point and the world-coordinate pixels through
//which the rays are cast.  It's only there for calculation purposes
#define CUTOFF 0.0001f

#define DIAG_3_COEF 0.28868f
#define DIAG_2_COEF 0.35455f
#define DIAG_1_COEF 0.5f
#define DIAG_0_COEF 1.0f

class Cloud
{
  float delt;
  float stepLength;
  int cloudMaxX, cloudMaxY, cloudMaxZ;
  vec3 bgColor;
  vec3 matColor;
  string outName;
  int screenMaxX, screenMaxY;
  vec3 eyePos;
  vec3 viewDir;
  vec3 upDir;
  float fovYDeg;
  vec3 lightPos;
  vec3 lightColor;

  vec3 planeRightVec;
  vec3 planeUpVec;
  vec3 normalizedViewDir;
  
  float pixelSize;

  Perlin perlinCloud;
  Perlin perlinPuff;

  //A 3d array containing the cloud densities at each voxel.
	float *densBfr;

  //A 3d array.  This is the total amount of light absorbed and diffused
  //by each voxel.
	float *redBfr;
	float *greenBfr;
	float *blueBfr;

  vector<vec3> lightList;
  int lightCount;

  void light(vec3 pos, vec3 color);
  bool withinBounds(vec3 pos);
  float cloudSample(vec3 pos, float *whichArray);
  float cloudSampleSimple(int x, int y, int z, float *whichArray);
  float lightAt(vec3 pos, vec3 lightPos2);
  vec3 pixelVector(int x, int y);
  vec3 colorAt(int x, int y);
  void initializeConsts();
  int cloudIndex(int x, int y, int z);
  bool permanentlyMissed(vec3 pos, vec3 dir);
  vec3 compwise(vec3 a, vec3 b);
  float min(float a, float b);
  float max(float a, float b);
  void heatSwap(int x, int y, int z, int x1, int y1, int z1, float coef, float* target);

public:
  int main();
	Cloud(char *filename);
  void sphere(vec3 center, float r, float densInner, float densOuter);
  void noisySphere(vec3 center, float r, float densInner, float densOuter);
  void pPuff(vec3 center, float r);
  void cube(vec3 backBottomLeft, vec3 frontTopRight, float densInner, float densOuter);
  void noisyCube(vec3 backBottomLeft, vec3 frontTopRight, float densInner, float densOuter);
  void diffuseStep(float difCoef);
  void render(int iteration);
	~Cloud(void);
};

