What?
This is the start of a write-from-scratch version of the server. Most MaNGOS code is gone except for ByteBuffer, which is too handy to lose. All of the libraries that used to be used have been replaced with the Boost C++ libraries, which are far easier to update and are better maintained. For now, physics will be done with the Box2D library. 

Why?
We are to the point where most of the basic features of the server are figured out (opcodes, packets, etc.). What we now need is a stable framework to build off of. I felt (and know) that MaNGOS adds too much shit that we don't need and was built for an entirely different environment. However, in order to ease code transition, most of the classes maintain similar names and functions.


!!! CODING STYLE !!!
In order to keep with Boost/STL style, and in order to make old code more noticable, the coding style is now C-style, not humpback. Hence,

BAD:    functionName
GOOD:	function_name

BAD:	_classVariableName
GOOD:	m_class_variable_name


!!! BUILD SYSTEM !!!
Until this version is somewhat working/stable, I am just going to use the standard Visual Studio project and layout. Later, we should discuss a more suitable build system, that can work in both Linux and Windows.


!!! PORTABILITY !!!
All code should be cross-platform. This shouldn't be hard if you utilize the Boost C++ and STL libraries. All I/O and Network code should use std::iostream and boost::asio, as appropriate.