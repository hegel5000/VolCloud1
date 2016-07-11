VolCloud
========

About This Repo
---------------
In a computer graphics class I took in 2013, I was assigned to write a ray
tracer for scenes of voxels.  I extended it at little.  All of my original 
(and as of yet undocumented, sry) code is in `./VolCloud1/Cloud.cpp`, so check 
that out.

**Check out `./Release` for some example renderings and config files.**

This was made in Visual Studio 2012.  Rather than figuring out how to actually
compile VolCloud, you might want to try the pre-compiled Windows executable
also found in `./Release`.  Or not, because you shouldn't trust strangers with
executables from the internet.

Usage
-----
Run `./Release/VolCloud1.exe` on a configuration file (since this is Windows,
that means dragging and dropping the text file onto the executable).  There
should be some example config files in that directory as well.  When rendering
is done, a bitmap file will pop out in the same directory as the program, with
a filename specified under the `FILE` argument in the config file.

Settings Concepts
-----------------
VolCloud has two CPU intensive stages: illumination followed by rendering.
Both of them involve ray tracing (but not recursive path tracing).

Note that all of the shapes you compose get flattened into a single cube of
voxels.  This means that the number of shapes in your scene has absolutely no
impact on rendering times (composition/flattening is a trivially fast step).
In fact, extremely dense scenes render slightly faster since rays stop being
simulated once they're fully absorbed.

Oh, and I might use 'config file' and 'scene' interchangeably >.>

###Rays
It's the same ray you remember from geometry class, except instead of an 
infinite number of points, there are a small finite number of sample points at
fixed intervals.

Since this is a voxel renderer, for canvas side lengths x, y, and z, and step 
distance s, the ray simulation time (let's call it R) is `O(max(x, y, z)/s)`.

Note that each ray is really three completely separate rays: once for each 
color buffer/channel.

###Illumination
For every light specified in the config file, and for every voxel in the
canvas, a ray is cast from that light to that voxel.  Light is absorbed from
the ray along the way and whatever is left is added to the total illumination
of the voxel at the end.

That is to say, illumination runs in `O(x*y*z*l*R)`, where x, y, and z are the
canvas dimensions and l is the number of lights in the scene.

Lights which placed are close to dense shapes or the edge of the canvas will 
take less time to simulate, since illumination rays wil be cut short.

###Rendering/pixel rays
You can sorta think of this step as photographic exposure.  For every pixel 
in the output image, a ray is cast, the direction of which is determined by
Cloud::pixelVector.  You can think of it as there being a pinprick lense with
the monitor on the other side, and your rays start from pixels in the little
screen and travel through the lense.

In any case, rendering runs in `O(x*y*R)`, where x and y are the dimensions of
your output image.

Scenes will take less time to render if the camera is placed close to dense
shapes or the edge of the screen, since pixel rays will be cut short.

Settings
--------

###DELT
Set this to whatever number you want.  It doesn't matter.  Right now it _does_
have to appear in the config file, though.  Sorry about that.

###STEP
This is the distance in voxels (but it's a float) that a ray of light travels 
before it checks its surroundings, essentially.  A shorter STEP length will 
cause rays of light as well as pixel rays to peter out faster when they go 
through semitransparent shapes i.e. will make the image darker (but in a very 
specific way that I haven't studied but which is different from what lowering 
the MRGB values does).

This is _not_ a proper numerical simulation where lower smaller steps are 
fundamentally better.  Still, something well over 1.0 is probably bad, since at
that point your rays might be completely missing voxels.

However, like in a proper numerical simulation, the program run time is
inversely proportional to the step size.

###XYZC
Dimensions of the voxel canvas in X, Y, Z, where Y is height and each number is
an integer.

In any case, this determines the dimensions of your 3D canvas in voxels.  More
voxels will give you less voxelly images, although really I would just 
recommend leaving this at 100 100 100.

###BRGB
Background color in R, G, B, each a float.  Each of the three color components
is on a scale from 0 (black) to 1 (maximum intensity in that channel).  Every 
pixel ray will eventually hit the background, unless it is completely absorbed
before it gets there.

###MRGB
It stands for material color, but it's more of an exposure setting, in R, G, B,
each a float.  Again, each of the three color components goes 0 to 1, with 0 
making that channel darker and 1 making that channel lighter.

This setting has no effect on how voxels are illuminated.  Instead, it is used
during pixel rendering.  When a pixel ray adds up the brightness of voxels as
it passes through them, the relevant component of MRGB is multiplied by that 
voxel brightness.  If you see colors getting clipped to white, maybe try 
turning down this value.  But otherwise leave it at 1 1 1.

###FILE
The output file name.

###RESO
Resolution in X, Y, each integers.  The width and height of the output image.

###EYEP
Camera (eye) position in X, Y, Z, each floats.  Y is height.  I'm not sure what
happens if you put the camera outside of the canvas and then have it face away
from it, but my guess is an infinite loop.  Sorry about that.

###VDIR
View direction in X, Y, Z, each floats.  

It's not really intuitive how setting the camera direction works, so sorry 
about that.  But as a guideline: if a component of EYEP is close to zero, make
the corresponding component of VDIR positive.  If a component of EYEP is close
to the corresponding component of XYZC, make the corresponding component of
VDIR negative.

###UVEC
Up vector in X, Y, Z, each floats.  If you set this to 0 1 0, then Y will mean
height in all the XYZ related settings.

###FOVY
Field of view Y; a floating point number of degrees.  Here, Y refers to the
the height of the output image, not the voxel canvas.

Note that wide-angle images won't be any slower to render than more zoomed-in
ones, since you're putting out just as many pixel rays in either case.

###LPOS
Position of light #1 in X, Y, Z, each floats.  Other lights can appear in the
shapes list (except that more shapes don't cost anything performance-wise while
the illumination step is proportional to the number of lights).

###LCOL
Color of light #1 in R, G, B.  See above.  

Note that this light number one does not count towards the shapes/lights list
size.

Shape Concepts
--------
###Density
This is a property of individual voxels, and is the fraction of the light going
through the given voxel that gets absorbed by it.  1 means that all light is
absorbed and 0 does nothing.

###Negative density
No voxel can have a net negative density.  However, a _shape_ can.  If you put
a negative density shape in the config file after a positive density shape, 
your negative density shape will be 'cut out' of the positive density one.

Note that the net density of a voxel gets clamped between 0 and 1 after each
shape gets added to the 3D canvas.  This means that a negative density shape 
at the beginning of a config file will do nothing.

More importantly, a -1 density shape will completely cut itself out of earlier
shapes in without leaving ghostly cutouts in later-specified shapes.

###Inner density/outer density
If you set both of these arguments to the same value, then your shape will have
uniform density.  If you don't, then the innermost voxel of your shape will
have exactly the specified inner density, while the outermost voxel shell of 
the shape will have exactly the outer density.  Every shell of voxels in 
between will be a linear interpolation between those two values.

###'general'
Every shape which says 'general' after it also has a version without that word,
wherein the innerDensity and outerDensity arguments are both set to 1.  They
aren't documented, though you might see them in the example config files.

Shapes/lights
------
**The (integer) number of entries in the shapes/lights list goes at the top
  of the list!**

(This is because I wrote a crappy parser T_T)

Lights can be mingled into the list in any order.

###light
```
x y z
r g b
```
See LPOL and LCOL above in the Settings section.  It's just that if you want to
have more lights than that first one, then you just put the additional light 
entries into the shapes/lights list.  Since shapes don't have color, you have 
to manage all of your colors with lights.

###sphereGeneral
```
x y z 
radius innerDensity outerDensity
```
These will look _extremely_ jagged.

###cloudGeneral
```
x y z
radius innerDensity outerDensity
```
It's a bit like the sphere, except that all the voxels in it are multiplied by
a Perlin noise function.  So really it looks nothing like a sphere.

The flame effect in shadow_puppet.bmp is produced by cutting a negative density
cloud out of a flat cuboid slab; a light is shined through that cut.

###cubeGeneral
```
x1 y1 z1
x2 y2 z2
radius innerDensity outerDensity
```
Really it's a cuboid.  It cannot be rotated, unfortunately.  But the axis it's
constrained to keeps its surfaces nice and smooth.  The first three numbers
give the location of one corner of the cube, while the other three give the
opposite corner.

###cubeFuzzy
```
x1 y1 z1
x2 y2 z2
radius innerDensity outerDensity
```
This is to cubeGeneral as cloudGeneral is to sphereGeneral.  I used this shape
with a high outerDensity to produce the early 00s FPS bathroom faucet effect 
seen in negasphere.bmp.

###pyroclastic
```
x y z
radius
```
I have no idea how the duck these work.  Good luck.  But they're pretty cool.
One word of caution is that to the pyroclastic puff shape, your radius argument
is more of a guideline.

A Request
---------
I wanna see how well VolCloud can be used to make vaporwave album cover art.  
You'd need to give it a round of Photoshop to put in the 余計なカタカナ and maybe some
of that wavy blue pattern from paper Solo cups.  But other than that, I think 
we're on to something pretty ａｅｓｔｈｅｔｉｃ.  So yeah, make some art with this.

Watch out for a Haskell remake of VolCloud, coming . . . at some point.