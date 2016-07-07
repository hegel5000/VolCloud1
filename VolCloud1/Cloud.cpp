#include "stdafx.h"
#include "Cloud.h"
#include "EasyBMP.h"
#include <sstream>
#include <fstream>
#include <iostream>

int main(int argc, char* argv[]) 
{
    Cloud* cloudy = new Cloud(argv[1]);


    /*j
    //cloudy->sphere(vec3(50, 50, 0), 100, 1.0, -1.0); 
    //cloudy->sphere(vec3(0, 100, 0), 42, (1), (1)); 
    //cloudy->sphere(vec3(30, 100, 0), 30, (-1), (-1)); 
    //cloudy->noisySphere(vec3(0, 100, 0), 100, 1.0, 1.0); 
    //cloudy->cube(vec3(0, 0, 42), vec3(42, 42, 45), 1.0, 0); 
    //cloudy->cube(vec3(0, 0, 0), vec3(42, 42, 3), 1.0, 0); 
    //cloudy->cube(vec3(25, 25, 25), vec3(75, 75, 75), 0.5, 0.5); 
    //cloudy->cube(vec3(26, 26, 26), vec3(74, 74, 74), -1, -1); 
    //cloudy->noisySphere(vec3(50, 50, 50), 25, 1.0, 1.0); 
    //cloudy->cube(vec3(15, 15, 1), vec3(25, 25, 99), -1, -1); 
    cloudy->sphere(vec3(70, 70, 30), 40, 1, 1);
    cloudy->cube(vec3(30, 30, 30), vec3(70, 70, 70), 1.0, 0); 
    cloudy->cube(vec3(31, 31, 31), vec3(69, 69, 69), -1.0, -1.0); 
    cloudy->noisyCube(vec3(30, 30, 30), vec3(70, 70, 70), 1.0, -1.0); 
    */
    

    cloudy->render(0);
 
    delete cloudy;

    return 0;
}

Cloud::Cloud(char *filename)
  // Perlin::Perlin(int octaves,float freq,float amp,int seed)
  //next is a list of working values
	//: perlin(10, 0.3, 1, 2)
	//: perlin(10, 0.3, 1, 19191919)
  //: perlin(4, 0.3, 1.2, 19)
  : perlinCloud(10, 0.06, 1.0, 19) //definitely working values for cloud
  , perlinPuff(4, 0.01, 0.42, 19) //possible value for pyroclastic puff
{ 
  string line;
  ifstream myfile (filename);
  lightCount = 1;

  if (myfile.is_open())
    { 
      while ( getline (myfile,line) )
      {
        istringstream lineStream (line);
        
        string token;
        lineStream >> token;
        //cout << token;
        
        if (token == "DELT") {
          lineStream >> delt;
		    } else
        if (token == "STEP") {
          lineStream >> stepLength;
		    } else
        if (token == "XYZC") {
          lineStream >> cloudMaxX >> cloudMaxY >> cloudMaxZ;
          //cout << cloudMaxX << cloudMaxY << cloudMaxZ;
          if (densBfr) {
            free(densBfr);
		      } else {
            densBfr = new float[cloudMaxX * cloudMaxY * cloudMaxZ];
            redBfr = new float[cloudMaxX * cloudMaxY * cloudMaxZ];
            greenBfr = new float[cloudMaxX * cloudMaxY * cloudMaxZ];
            blueBfr = new float[cloudMaxX * cloudMaxY * cloudMaxZ];
			    }
		    } else
        if (token == "BRGB") {
          float fst, snd, thd;
          lineStream >> fst >> snd >> thd;
          bgColor = vec3(fst, snd, thd);
		    } else
        if (token == "MRGB") {
          float fst, snd, thd;
          lineStream >> fst >> snd >> thd;
          matColor = vec3(fst, snd, thd);
		    } else
        if (token == "FILE") {
          lineStream >> outName;
		    } else
        if (token == "RESO") {
          lineStream >> screenMaxX >> screenMaxY;
		    } else
        if (token == "EYEP") {
          float fst, snd, thd;
          lineStream >> fst >> snd >> thd;
          eyePos = vec3(fst, snd, thd);
		    } else
        if (token == "VDIR") {
          float fst, snd, thd;
          lineStream >> fst >> snd >> thd;
          viewDir = vec3(fst, snd, thd);
		    } else
        if (token == "UVEC") {
          float fst, snd, thd;
          lineStream >> fst >> snd >> thd;
          upDir = vec3(fst, snd, thd);
		    } else
        if (token == "FOVY") {
          lineStream >> fovYDeg;
		    } else
        if (token == "LPOS") {
          float fst, snd, thd;
          lineStream >> fst >> snd >> thd;
          lightPos = vec3(fst, snd, thd);
		    } else
        if (token == "LCOL") {
          float fst, snd, thd;
          lineStream >> fst >> snd >> thd;
          lightColor = vec3(fst, snd, thd);
		    } else { 
          int shapeCount;
          myfile >> shapeCount;
          cout <<"("<<"shapeCount = "<<shapeCount<<")";
          for (int i = 0; i < shapeCount; i++) {
            string shapeType;
            cout << shapeType;
            float fst, snd, thd;

            myfile >> shapeType; 

            myfile >> fst >> snd >> thd;

            if (shapeType == "sphere") {
              int r;
              myfile >> r;
              this->sphere(vec3(fst, snd, thd), r, 1, 0);
            }
            if (shapeType == "cloud") {
              int r;
              myfile >> r;
              this->noisySphere(vec3(fst, snd, thd), r, 1, 1); 
			      }
            if (shapeType == "cloudGeneral") {
              int r;
              float mult1, mult2;
              myfile >> r >> mult1 >> mult2;
              this->noisySphere(vec3(fst, snd, thd), r, mult1, mult2); 
			      }
            if (shapeType == "pyroclastic") {
              int r;
              myfile >> r;
              this->pPuff(vec3(fst, snd, thd), r);
			      }
            if (shapeType == "sphereGeneral") {
              int r, densInner, densOuter;

              myfile >> r >> densInner >> densOuter;

              this->sphere(vec3(fst, snd, thd), r, densInner, densOuter);
			      }
            if (shapeType == "negaSphereSolid") {
              int r;
              myfile >> r;
              this->noisySphere(vec3(fst, snd, thd), r, -1, -1);
			      }
            if (shapeType == "cubeGeneral") {
              int x2, y2, z2; 
              float densInner, densOuter;
              myfile >> x2 >> y2 >> z2 >> densInner >> densOuter;
              this->cube(vec3(fst, snd, thd), vec3(x2, y2, z2), densInner, densOuter);
			      }
            if (shapeType == "cubeFuzzy") {
              int x2, y2, z2;
              float densInner, densOuter;
              myfile >> x2 >> y2 >> z2 >> densInner >> densOuter;
              this->noisyCube(vec3(fst, snd, thd), vec3(x2, y2, z2), densInner, densOuter);
			      }
            if (shapeType == "light") {
              float r, g, b;
              myfile >> r >> g >> b;

              this->lightList.push_back(vec3(fst, snd, thd));
              this->lightList.push_back(vec3(r, g, b));
              lightCount++;
			      }
		      }
			  } //no line label case
      } //parameter parsing loop
      myfile.close();
    } //if file is open

  else cout << "Unable to open file";

  lightList.push_back(lightPos);
  lightList.push_back(lightColor);

  initializeConsts();
}

void Cloud::initializeConsts() {
  planeRightVec = (cross(viewDir, upDir));
  planeUpVec = (cross(cross(viewDir, upDir), viewDir));
  normalizedViewDir = normalize(viewDir);
  pixelSize = abs(tan(fovYDeg * 3.14159265f / 180)) * 2.0f / screenMaxY;

  eyePos = eyePos - (normalizedViewDir * 100.0f);
}

void Cloud::render(int iteration) {
  for (int i = 0; i < lightCount; i++) {
      light(lightList[2*i], lightList[2*i + 1]); 
  }
  
  cout << "rendering";
  BMP output;
  output.SetSize(screenMaxX, screenMaxY);
  output.SetBitDepth(24);

  for (int i = 0; i < screenMaxX; i++) {
    for (int j = 0; j < screenMaxY; j++) {
      int jRev = screenMaxY - j - 1;
      const vec3 colorAtPos = colorAt(i, jRev);
      output(i, j)->Red = (int)(colorAtPos.x * 255);
      output(i, j)->Green = (int)(colorAtPos.y * 255);
      output(i, j)->Blue = (int)(colorAtPos.z * 255);
	  }
	cout << i;
  }

  const char * c = outName.c_str();
  output.WriteToFile(c);
}

void Cloud::light(vec3 pos, vec3 color)
{
  cout << "lighting: ";
  for (int i = 0; i < cloudMaxX; i++) {
    cout << i;
    for (int j = 0; j < cloudMaxY; j++) {
      for (int k = 0; k < cloudMaxZ; k++) {
        float brightness = lightAt(vec3(i, j, k), pos);
        int index = cloudIndex(i, j, k);
        redBfr[index] = min(1, color.x * brightness + redBfr[index]);
        greenBfr[index] = min(1, color.y * brightness + greenBfr[index]);
        blueBfr[index] = min(1, color.z * brightness + blueBfr[index]);
      } // for k
    } //for j
  } //for i
}

int Cloud::cloudIndex(int x, int y, int z) {
  //return x + (cloudMaxY - y) * cloudMaxX + z * cloudMaxX * cloudMaxY;
  return x + y * cloudMaxX + z * cloudMaxX * cloudMaxY;
}

float Cloud::lightAt(vec3 pos, vec3 lightPos2)
{
  //also note that I am editing the input copy of vec3 pos
  const vec3 fullRay = lightPos2 - pos;
  const vec3 normalizedRay = normalize(fullRay);
  const int stepCount = (int) (length(fullRay) / stepLength);

  float transmittanceProduct = 1.0;
  float localDens = 0.0f;

  for (int i = 0; i < stepCount; i++) {
    pos += normalizedRay * stepLength;
    if (withinBounds(pos)) {
      localDens = cloudSample(pos, densBfr);
      if (localDens >= CUTOFF) {
        float transMult = exp(-(ABSORB_COEFF * stepLength * localDens));
        transmittanceProduct *= transMult;
        if (transmittanceProduct < CUTOFF) {
            return CUTOFF;
        } // if transmittance is low enough
      } //if density is high enough
    } //if within bounds
  }
  return transmittanceProduct;
}

bool Cloud::withinBounds(vec3 pos)
{
  /*
  int cI = cloudIndex((int) pos.x, (int) pos.y, (int) pos.z);
  return cI >= 0 && cI < (cloudMaxX * cloudMaxY * cloudMaxZ);
  */

  return (((int) pos.x) >= 0 && ((int) pos.y) >= 0 && ((int) pos.z) >= 0
      && (((int) pos.x) + 1) < cloudMaxX
      && (((int) pos.y) + 1) < cloudMaxY
      && (((int) pos.z) + 1) < cloudMaxZ
	  );

      /*
  return (pos.x > 0 && pos.y > 0 && pos.z > 0
        && pos.x < cloudMaxX && pos.y < cloudMaxY && pos.z < cloudMaxZ);
        */
}

float Cloud::cloudSampleSimple(int x, int y, int z, float *whichArray) {
  if (withinBounds(vec3(x, y, z))) {
    return whichArray[cloudIndex(x, y, z)];
  } else {
    return 0;
  }
 }

float Cloud::cloudSample(vec3 pos, float *whichArray) 
{
  //uncomment this to turn off trilinear interpolation
  //return cloudSampleSimple((int) pos.x, (int) pos.y, (int) pos.z, whichArray);

  if (withinBounds(pos)) {
    int x0 = (int) pos.x; 
    int x1 = ((int) pos.x) + 1;
    int y0 = (int) pos.y; 
    int y1 = ((int) pos.y) + 1;
    int z0 = (int) pos.z; 
    int z1 = ((int) pos.z) + 1;

    float xd = (pos.x - x0)/(x1 - x0);
    float yd = (pos.y - y0)/(y1 - y0);
    float zd = (pos.z - z0)/(z1 - z0);

    float c00 = cloudSampleSimple(x0, y0, z0, whichArray) * (1 - xd) + cloudSampleSimple(x1, y0, z0, whichArray) * xd;
    float c10 = cloudSampleSimple(x0, y1, z0, whichArray) * (1 - xd) + cloudSampleSimple(x1, y1, z0, whichArray) * xd;
    float c01 = cloudSampleSimple(x0, y0, z1, whichArray) * (1 - xd) + cloudSampleSimple(x1, y0, z1, whichArray) * xd;
    float c11 = cloudSampleSimple(x0, y1, z1, whichArray) * (1 - xd) + cloudSampleSimple(x1, y1, z1, whichArray) * xd;

    float c0 = c00 * (1 - yd) + c10 * yd;
    float c1 = c01 * (1 - yd) + c11 * yd;
    
    return c0 * (1 - zd) + c1 * zd;

  } else {
    return 0;
  }
}

vec3 Cloud::pixelVector(int x, int y) { 
  vec3 center = eyePos + normalizedViewDir;
  vec3 displacX = planeRightVec * (float)(x - screenMaxX / 2) * pixelSize;
  vec3 displacY = planeUpVec * (float)(y - screenMaxY / 2) * pixelSize;
  
  return (center + displacX + displacY - eyePos);
}

bool Cloud::permanentlyMissed(vec3 pos, vec3 dir) { 
	return (
		   (pos.x < 0 && dir.x < 0)
		|| (pos.y < 0 && dir.y < 0)
		|| (pos.z < 0 && dir.z < 0)
		|| (pos.x > cloudMaxX && dir.x > 0)
		|| (pos.y > cloudMaxY && dir.y > 0)
		|| (pos.z > cloudMaxZ && dir.z > 0)
		);
}

vec3 Cloud::colorAt(int x, int y) {
  vec3 pvNormalized = normalize(pixelVector(x, y));

  vec3 colorTotal = vec3(0, 0, 0);
  float transProd = 1;
  vec3 pos = eyePos;
  
  while (!permanentlyMissed(pos, pvNormalized) && !withinBounds(pos)) {
    pos += pvNormalized * stepLength;
  }

  while (withinBounds(pos) && transProd > CUTOFF) {
    pos += pvNormalized * stepLength;

    float currentDensity = cloudSample(pos, densBfr);
    if (currentDensity > CUTOFF) {
      float transFac = exp(-(ABSORB_COEFF * stepLength * currentDensity));
      transProd *= transFac;
      colorTotal 
		    += (1 - transFac) 
          * transProd
      //    * currentDensity
          * compwise(matColor
		          , vec3(cloudSample(pos, redBfr), cloudSample(pos, greenBfr), cloudSample(pos, blueBfr))
				    );
	  }
  }
  if (!withinBounds(pos)) {
    colorTotal += bgColor * transProd;
  }

  //return vec3(min(colorTotal.x, 1), min(colorTotal.y, 1), min(colorTotal.z, 1));
  return colorTotal;
}

float Cloud::min(float a, float b) {
    return a < b ? a : b;
}

float Cloud::max(float a, float b) {
    return a > b ? a : b;
}

vec3 Cloud::compwise(vec3 a, vec3 b) {
	return vec3(a.x * b.x, a.y * b.y, a.z * b.z);
}

//I'm sorry.  I'm just not quite sure how I should separate out my files :(
//It's not _that_ bad having everything in the same file, though, is it?
//In minimizes inter-file dependencies, I guess.
void Cloud::sphere(vec3 center, float r, float densInner, float densOuter) {
  int xbegin  = (int) max(center.x - r, 0);
  int xend    = (int) min(((int) (center.x + r)) + 1, cloudMaxX);
  int ybegin  = (int) max(center.y - r, 0);
  int yend    = (int) min(((int) (center.y + r)) + 1, cloudMaxY);
  int zbegin  = (int) max(center.z - r, 0);
  int zend    = (int) min(((int) (center.z + r)) + 1, cloudMaxZ);
 
  for (int i = xbegin; i < xend; i++) {
    for (int j = ybegin; j < yend; j++) {
      for (int k = zbegin; k < zend; k++) {
        float currentR = length(vec3(i, j, k) - center);
        if (currentR < r) {
          float fracFromCenter = min(1, currentR / r);
			    densBfr[cloudIndex(i, j, k)] = max(0
				      , densBfr[cloudIndex(i, j, k)]
					      + (1 - fracFromCenter) * densInner + fracFromCenter * densOuter 
					  );
		    }
      } // for k
    } //for j
  } //for i
}

void Cloud::noisySphere(
	  vec3 center, float r, float densInner, float densOuter) { 
  for (int i = 0; i < cloudMaxX; i++) {
    for (int j = 0; j < cloudMaxY; j++) {
      for (int k = 0; k < cloudMaxZ; k++) {
        float currentR = length(vec3(i, j, k) - center);
        densBfr[cloudIndex(i, j, k)] = max(0
            , densBfr[cloudIndex(i, j, k)]
              + max(0, perlinCloud.Get(i, j, k) + densInner - abs(currentR/r)) * densOuter
          );
      } // for k
    } //for j
  } //for i
}

void Cloud::pPuff(vec3 center, float r) {
  for (int i = 0; i < cloudMaxX; i++) {
    for (int j = 0; j < cloudMaxY; j++) {
      for (int k = 0; k < cloudMaxZ; k++) {
        float currentR = length(vec3(i, j, k) - center);
        densBfr[cloudIndex(i, j, k)] 
		      //+= max(0, perlinPuff.Get(i, j, k) + 1 - abs(currentR/r));
		      += max(0, r - abs(currentR/r) + abs(perlinCloud.Get(i, j, k)));
      } // for k
    } //for j
  } //for i
}

void Cloud::cube(vec3 bTL, vec3 fTR, float densInner, float densOuter) {
  vec3 center = vec3((bTL.x + fTR.x) / 2, (bTL.y + fTR.y) / 2, (bTL.z + fTR.z) / 2);
	for (int i = bTL.x; i <= fTR.x; i++) {
    for (int j = bTL.y; j <= fTR.y; j++) {
      for (int k = bTL.z; k <= fTR.z; k++) {
        float currentR = max(i - center.x, max(j - center.y, k - center.z));
        float r = max(fTR.x - bTL.x, max(fTR.y - bTL.y, fTR.z - bTL.z));
        if (currentR < r) {
          float fracFromCenter = min(1, currentR / r);
			    densBfr[cloudIndex(i, j, k)] = max(0
				      , densBfr[cloudIndex(i, j, k)]
					      + (1 - fracFromCenter) * densInner + fracFromCenter * densOuter 
					  );
		    }
      } 
    } 
	}
}

void Cloud::noisyCube(vec3 bTL, vec3 fTR, float densInner, float densOuter) {
  vec3 center = vec3((bTL.x + fTR.x) / 2, (bTL.y + fTR.y) / 2, (bTL.z + fTR.z) / 2);
	for (int i = bTL.x; i <= fTR.x; i++) {
    for (int j = bTL.y; j <= fTR.y; j++) {
      for (int k = bTL.z; k <= fTR.z; k++) {
        float currentR = max(i - center.x, max(j - center.y, k - center.z));
        float r = max(fTR.x - bTL.x, max(fTR.y - bTL.y, fTR.z - bTL.z));
        if (currentR < r) {
          float fracFromCenter = min(1, currentR / r);
		      densBfr[cloudIndex(i, j, k)] = max(0
              , densBfr[cloudIndex(i, j, k)]
                + max(0, perlinCloud.Get(i, j, k) + densInner - abs(currentR/r)) * densOuter
            );
		    }
      } 
    } 
	}
}

void Cloud::heatSwap(int x0, int y0, int z0, int x1, int y1, int z1, float coef, float* target) {
  target[cloudIndex(x0, y0, z0)] 
    = (densBfr[cloudIndex(x0, y0, z0)] - densBfr[cloudIndex(x1, y1, z1)]) * coef;

  target[cloudIndex(x1, y1, z1)] 
    = (densBfr[cloudIndex(x1, y1, z1)] - densBfr[cloudIndex(x0, y0, z0)]) * coef;
}

void Cloud::diffuseStep(float difCoef) {
  float* newDensities = new float[cloudMaxX * cloudMaxY * cloudMaxZ];
	for (int i = 0; i < cloudMaxX - 1; i++) {
    for (int j = 0; j < cloudMaxY - 1; j++) {
      for (int k = 0; k < cloudMaxZ - 1; k++) {
        heatSwap(i, j, k, i + 1, j + 1, k + 1, difCoef * DIAG_3_COEF, newDensities);

        heatSwap(i, j, k, i + 0, j + 1, k + 1, difCoef * DIAG_2_COEF, newDensities);
        heatSwap(i, j, k, i + 1, j + 0, k + 1, difCoef * DIAG_2_COEF, newDensities);
        heatSwap(i, j, k, i + 1, j + 1, k + 0, difCoef * DIAG_2_COEF, newDensities);

        heatSwap(i, j, k, i + 1, j + 0, k + 0, difCoef * DIAG_1_COEF, newDensities);
        heatSwap(i, j, k, i + 0, j + 1, k + 0, difCoef * DIAG_1_COEF, newDensities);
        heatSwap(i, j, k, i + 0, j + 0, k + 1, difCoef * DIAG_1_COEF, newDensities);

        newDensities[cloudIndex(i, j, k)] = 
          min(1, max(0, newDensities[cloudIndex(i, j, k)] + densBfr[cloudIndex(i, j, k)]));
      }
    }
	}

  delete(densBfr);
  densBfr = newDensities;
}

Cloud::~Cloud(void)
{
}