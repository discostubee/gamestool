#include "terminal_world.hpp"

////////////////////////////////////////////////////////////
using namespace gt;


cTerminalWorld::cTerminalWorld(){
}

cTerminalWorld::~cTerminalWorld(){
}

void
cTerminalWorld::flushLines(){
	dLines lines;
	primordial::gPrim.getLines(&lines, true);

	if(lines.empty())
		return;

	for(dLines::iterator l=lines.begin(); l != lines.end(); ++l)
		std::cout << *l << std::endl;
}

tAutoPtr<cWorld>
cTerminalWorld::makeWorld(){
	return tAutoPtr<cWorld>(new cTerminalWorld());
}
