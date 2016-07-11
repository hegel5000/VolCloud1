VolCloud
========

About This Repo
---------------
In a computer graphics class I took in 2013, I was assigned to write a ray
tracer for scenes of voxels.  All of my original code is in 
`./VolCloud1/Cloud.cpp`, so check that out.

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
Both of them involve ray tracing.

Note that all of the shapes you compose get flattened into a single cube of
voxels.  This means that the number of shapes in your scene has absolutely no
impact on rendering times (composition/flattening is a trivially fast step).
In fact, extremely dense scenes render slightly faster since rays stop being
simulated once they're fully absorbed.

Oh, and I might use 'config file' and 'scene' interchangeably >.>

###Rays
It's the same ray you remember from geometry class, except except that rather
than an infinite number of points, there are a small finite number of sample
points at fixed intervals.

Since this is a voxel renderer, for canvas side lengths x, y, and z, and step 
distance s, the ray simulation time (let's call it R) is `O(max(x, y, z)/s)`.
(For a proper path tracer, it's instead about the number of collidable 
primitives in the scene and the max ray recursion depth).

Note that each ray is really three completely separate rays: once for each 
color buffer.

###Illumination
For every light specified in the config file, and for every voxel in the
canvas, a ray is cast from that light to that voxel.  Light is absorbed from
the ray along the way and whatever is left is added to the total illumination
of the voxel at the end.

That is to say, illumination runs in `O(x*y*z*l*R)`, where x, y, and z are the
canvas dimensions and l is the number of lights in the scene.

Lights which are close to dense shapes or the edge of the canvas will take less
time to simulate, since illumination rays wil be cut short.

###Rendering/pixel rays
You can possibly think of this as photographic exposure?  For every pixel 
in the output image, a ray is cast, the direction of which is determined by
Cloud::pixelVector.  You can think of it as there being a pinprick lense with
the monitor on the other side, and your rays get from the screen and through
the lense.

In any case, rendering runs in `O(x*y*R)`, where x and y are the dimensions of
your output image.

Scenes will take less time to render if the camera is placed close to dense
shapes or the edge of the screen, since pixel rays will be cut short.

###XYZ
Note that the camera will always be always be vertically aligned along the
Y-axis.

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

Overall I'm not sure what changing this setting does and I would recommend
leaving it at 0.5.  This is _not_ a proper numerical simulation where lower
smaller steps are fundamentally better.

However, like in a proper numerical simulation, the program run time is
inversely proportional to the step size.

###XYZC
Dimensions of the voxel canvas in X, Y, Z, where Y is height and each number is
an integer.

In any case, this determines the dimensions of your 3D canvas in voxels.  More
voxels will give you less voxelly images, although really I just recommend
leaving this at 100 100 100.

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
it passes through them, this number is multiplied by that voxel brightness.  If
you see colors getting clipped to white, maybe try turning down these numbers?

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
to the corresponding component of XYCZ, make the corresponding component of
VDIR negative.

###UVEC
Up vector in X, Y, Z, each floats.  If you set this to 0 1 0, then a positive Y
value means height.

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

Shape Concepts
--------
###Density
This is the fraction of the light going through it that gets
absorbed.  1 means that all light is absorbed and 0 does nothing.

###Negative density
No voxel can have a net negative density.  However, a _shape_ can.  If you
a negative density shape in the config file after a positive density shape,
your negative density shape will be 'cut out' of the positive density one.

Note that the net density of a voxel gets clamped between 0 and 1 between after
every shape gets added.  This means that a negative density shape at the
beginning of a config file will do nothing.

More importantly, a -1 density shape will cut itself out of earlier shapes in
the config file, without leaving ghostly cutouts in later shapes in the file.

###Inner density/outer density
If you set both of these arguments to the same value, then your shape will have
uniform density.  If you don't, then the innermost voxel of your shell will
have exactly the inner density, the outermost shell of voxels will have exactly
the outer density, and every shell of voxels in between will be a linear
extrapolation between those two values.

###'general'
Every shape which says 'general' after it also has a version without that word,
wherein the innerDensity and outerDensity arguments are both set to 1.  They
aren't documented.

Shapes/lights
------
**The number of entries in the shapes/lights list goes here.**

(I wrote a crappy parser T_T)

Lights can be mingled into the list in any order.

###light
```
x y z
r g b
```
See LPOL and LCOL above in the Settings section.  It's just that if you want to
have more lights than those first two, you just put light entries into the
shapes/lights list.  Since shapes don't have color, you have to manage all of
your colors with lights.

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
a Perlin noise function.  I recommend setting the innerDensity to something
in the middle and the outer density to something very low or zero.

The flame effect in shadow_puppet.bmp is produced by cutting a negative density
cloud into a flat cuboid slab, and light is shined through that cut.

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
This is to cubeGeneral as cloudGeneral is to sphereGeneral.  I used this with a
high outerDensity to produce the early 00s FPS bathroom faucet effect seen in
negasphere.bmp.

###pyroclastic
```
x y z
radius
```
I have no idea how the duck these work.  Good luck.  But they're pretty cool.
One word of caution is that the pyroclastic puff generator does sees the radius
argument as merely a guideline.

A Request
---------
I wanna see how well this can be used to make vaporwave album cover art.  You'd
need to give it a round of photoshop to put in the 余計なカタカナ and maybe some of
that pattern from paper Solo cups.  But other than that, I think we're on to
something totally ａｅｓｔｈｅｔｉｃ.