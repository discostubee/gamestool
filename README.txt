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

Windows XP/7: Eclipse, Visual C++ 9
Ubuntu 9->11L: Eclipse
OS X 10.6: Eclipse

If you want to expand the list of supported stuff, feel free to jump on
the forum or source forge and let us know.

Rational
----------------------------------------------------
There's no clear goal for the gamestool, because I have to be honest with
myself and just say that it's all done for the lolz. The only direction I
can really offer is to say: I wanna make an engine that lets you write once, in a graphical kinda way, and run anywhere. Oh, and support and
engine that generates procedural graphics.

But we already have a write once, run anywhere language you retort! It's
called Java, Ruby, Mr fantastics do anything language. Well, you sir don't
understand the zen of lolz do you!

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
There are several IDE projects located under projects. Currently visual studio and eclipse projects are being maintained. A good place to start (at the moment) is the unitTests project. This lets you see the tests
being run and lets you try out your improvements against the code that
exists.


--- Common problems:
Most problems you'll have compiling this project relate to missing
packages. It would be cool to write a kind of install script, but not
just yet.

- Boost library missing
You'll need to have boost installed in order to compile the games tool.
You can download it from:

Once downloaded, all you need to do is unzip it and chuck it somewhere your
compiler can find it.

It may be a good idea to keep the boost lib, root directory with a name
that includes the version number and not just naming it 'boost'. For instance, I've got mine under: linux:~/includes/boost_1_46

- gtest missing (if building the testing app)
The google testing framework needs to be downloaded and put somewhere
for you to be able to run unit tests. You'll also need to place the built gtest library into the gamestool include directory. Make sure the library is named libgtest_main.a

- Link to Boost library missing in Eclipse build
For eclipse you'll need to point your IDE to the spot you unzipped the lib. You can do this in eclipse by adding an Environment variable under the property manager (Properties->C/C++->Build->Environment). This variable must be named CPATH and its value needs to be the location of your boost directory. You can have as many directories as you like, so long as you use a colon to separate each path.

--- X11 symbols missing in OSX+Eclipse build
You'll need to install X11 for OSX (google for it) first, then add the path to the includes directory to the Eclipse environment, just like you do for the boost library. 

