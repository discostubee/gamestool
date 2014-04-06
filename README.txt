Gamestool readme. April 2014
----------------------------------------------------
Hi all, the software in this repository is presented as is and makes no 
promises or guarantees. All stuff in here that is claimed to be my stuff
is protected under the GNU (version 3) license. There is however, plenty of
other peoples stuff made use of in this project (for instance, boost and
openGL), so please be kind to their software licenses as well.

If there are any problems or questions regarding the software use, 
please visit www.frontiergraphics.com.au/gamestool and don't be afraid to 
sign up and flame away for us failing in some way.


Supported environments
----------------------------------------------------
These are some of the environments currently being supported. Put 
another way, this stuff should work on these platforms. As well as
the gamestool running on these OSs, there is also a list of supported
IDEs.

Windows 7: Visual Studio Express 2010.
Ubuntu 12: Eclipse 4.2
OS X 10.6: Eclipse 3.7

If you want to expand the list of supported stuff, feel free to jump on
the forum or source forge and let us know.


Rational
----------------------------------------------------
There's no clear goal for the gamestool, because I have to be honest with
myself and just say that it's all done for the lolz. The only direction I
can really offer is to say: I wanna make an engine that lets you write once, 
in a graphical kinda way, and run anywhere. Oh, and support an
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

Projects can also have a source directory, which is for code that is intended to have no other purpose but for that project.

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

