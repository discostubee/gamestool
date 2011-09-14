#ifndef CONTEXT_EXAMPLE_CPP
#define CONTEXT_EXAMPLE_CPP

#include <gt_base/context.hpp>
#include <gt_base/figment.hpp>

namespace showoff{

inline void
context(){
	gt::tOutline<gt::cFigment>::draft();
	gt::tOutline<gt::cWorldShutoff>::draft();

	gt::cContext dude;
	gt::cFigment stage1;
	gt::cWorldShutoff stage2;

	std::cout << "Context testing." << std::endl;

	//- needs redoing

}


}

#endif
