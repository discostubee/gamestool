Gamestool readme. April 2011
----------------------------------------------------
Hi all, the software in this repository is presented as is and makes no promises or guarantees. All stuff in here that is claimed to be my stuff
is protected under the GNU (version 3) license. There is however, plenty of
other peoples stuff made use of in this project (for instance, boost and
openGL), so please be kind to their software licenses as well.

If there are any problems or questions regarding the software use, please visit www.frontiergraphics.com.au/gamestool and don't be afraid to sign up
and flame away for us failing in some way.


Supported environments
----------------------------------------------------
These are some of the environments currently being supported. Put 
another way, this stuff should work on these platforms. As well as
the gamestool running on these OSs, there is also a list of supported
IDEs.

Windows 7: Visual Studio Express 2010.
Ubuntu 9->11L: Eclipse
OS X 10.6: Eclipse

If you want to expand the list of supported stuff, feel free to jump on
the forum or source forge and let us know.


Rational
----------------------------------------------------
There's no clear goal for the gamestool, because I have to be honest with
myself and just say that it's all done for the lolz. The only direction I
can really offer is to say: I wanna make an engine that lets you write once, 
in a graphical kinda way, and run anywhere. Oh, and support and
engine that generates procedural graphics.

But we already have a write once, run anywhere language you retort! It's
called Java, Ruby, Mr fantastics do anything language. Well, you sir don't
understand the zen of lolz do you!

Jumping in
----------------------------------------------------
Check out the example app project. This should be 
easier to compile and run than the unit tests or
any of the multi-threaded apps because you wont
need to compile the google test library or the boost
library.


Directories
----------------------------------------------------
Directory assistance...

- bin
All the runtime libraries and executables are located here. This 
directory may be empty when you check it out. But once you build the
project you should start seeing it fill up with goodies.

- docs
Mostly used by dOxygen to create a fancy pants, html based, user doc to
help coders new to the project. Other fluffy documents may appears in here
at some point the future.

- includes
Any 3rd party libraries which are platform independent, and small enough,
go in here. We'll update the libraries in here when the providers give
us a reason to do so. We also chuck in some libraries we compiled 
ourselves (like the google gTest lib) just to make things easy.

- projects
This is where we store all the IDE files ect. We'll also try and get the
gnu auto make tools in here at some point.

- source
This is where all the code goes. It's sorted according to the different
components, from static libs, to runtime hotness. Usually anything with
the gt_ prefix is a static lib, and addon is a prefix for our runtime
libraries for the gamestool (AKA plugins).


Compilin it
----------------------------------------------------
There are several IDE projects located under projects. Currently visual 
studio and eclipse projects are being maintained. A good place to start 
(at the moment) is the unitTests project. This lets you see the tests
being run and lets you try out your improvements against the code that
exists.

Make sure you ...

- Download boost library
You'll need to have boost installed in order to compile the games tool.
You can download it from:

Once downloaded, all you need to do is unzip it and chuck it somewhere your
compiler can find it. If can't install it somewhere that any compiler can
find it, you'll need to direct your compiler to it (read below)

It may be a good idea to keep the boost lib, root directory with a name
that includes the version number and not just naming it 'boost'. For 
instance, I've got mine under: linux:~/includes/boost_1_46

- compile boost lib in OSX
Reading the online 'getting started' guide for boost you'll find a spot 
about compiling, but for OSX the part about running the shell script
requires you to explicitly call 'sh' to run a shell script.

Chances are you'll also need to give the shell script permission
to do anything it wants with the boost directory. Running sh
as admin wasn't enough, but chmod with a+rwx did the trick. However
chmod a+wr wasn't enough. 

Next, instead of the b2 output that the offical guide says to use, in
mac, you'll need bjam. Now there are a few options with bjam which
are not listed when you type "$./bjam -help" so go here instead:
http://www.boost.org/boost-build2/doc/html/bbv2/overview/invocation.html

I tell you this because there are a few options you'll need, depending
on your system:

address-module=64
Lets face it, you'll have a 64bit processor and any 64bit mac will be using
64 bit OSx, so put this on.

link=static
runtime-link=static
using this just to make sure compilation is easy.

variant=debug
As every project at the moment is debug, you'll need this to be turned
onto debug.

The library is located under YOUR_BOOST_DIR/stage/lib which you will need
to add to your IDE.

Props to Matt from Technobroia for the bjam tip: http://www.technoboria.com/2009/07/simple-guide-to-installing-boost-on-mac-os-x/

- download and compile google test (if building the testing app)
The google testing framework needs to be downloaded and put somewhere
for you to be able to run unit tests. You'll also need to place the built 
gtest library into the gamestool include directory. Make sure the library 
is named libgtest_main.a because that's what the project is expecting.

To build the static lib just read the guide that comes with it.

- Link external includes in Eclipse
For eclipse you'll need to point your IDE to the spot you unzipped the 
lib. You can do this in eclipse by adding an Environment variable under 
the property manager (Properties->C/C++->Build->Environment). This 
variable must be named CPATH and its value needs to be the location of 
your boost directory. You can have as many directories as you like, so 
long as you use a colon to separate each path.

There are a few includes you need:
Boost
x11 for OSX (linux is already visible)
gtest (if building for testing apps)

- for OSX, get X11 
You'll need to install X11 for OSX (google for it) first, then add the 
path to the includes directory to the Eclipse environment, just like 
you do for the boost library. 

