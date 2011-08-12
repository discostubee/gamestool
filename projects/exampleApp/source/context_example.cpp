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

	dude.add(&stage1);
	if(dude.isStacked(&stage1) && !dude.isStacked(&stage2))
		std::cout << "stage 1 stacked, 2 is not." << std::endl;
	else
		std::cout << "derp, line: " << __LINE__ << std::endl;

	dude.add(&stage2);
	if(dude.isStacked(&stage1) && dude.isStacked(&stage2))
		std::cout << "stage 1 stacked, as well as 2." << std::endl;
	else
		std::cout << "derp, line: " << __LINE__ << std::endl;

	if(stage1.hash() == dude.getLastOfType(stage1.hash())->hash())
		std::cout << "stage 2 is the last of its type" << std::endl;
	else
		std::cout << "derp, line: " << __LINE__ << std::endl;

	if(stage2.hash() == dude.getLastOfType(stage2.hash())->hash())
		std::cout << "stage 2 is the last of its type" << std::endl;
	else
		std::cout << "derp, line: " << __LINE__ << std::endl;

	dude.finished(&stage2);
	if(dude.isStacked(&stage1) && !dude.isStacked(&stage2))
		std::cout << "stage 1 stacked, 2 is not." << std::endl;
	else
		std::cout << "derp, line: " << __LINE__ << std::endl;

	dude.finished(&stage1);

}


}

#endif
