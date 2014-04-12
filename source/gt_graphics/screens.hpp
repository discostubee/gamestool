#ifndef SCREENS_HPP
#define SCREENS_HPP

#include "stage.hpp"

namespace gt{

	//!\brief	Provides information about the screens.
	class cScreen: public cFigment{
	public:
		typedef unsigned short dID;

		static const cPlugTag* xPT_rect;	//!< The screen's rectum, in relation to screen 1.
		static const cPlugTag* xPT_ID;		//!< The ID of the screen.
		static const cCommand::dUID xSetID;
		static const cCommand::dUID xGetInfo;	//!< Get the dimensions of the screen and its ID that the dimensions represent.

		cScreen();
		virtual ~cScreen();

		GT_IDENTIFY("screen");
		GT_EXTENDS(cFigment);
		GT_VERSION(1);
		virtual dNameHash hash() const{ return getHash<cScreen>(); }

	protected:
		tPlug<dID> mID;

		void patGetInfo(ptrLead pLead);
		void patSetID(ptrLead pLead);

		virtual shape::tRectangle<dUnitPix> getDims(){ return shape::tRectangle<dUnitPix>(); };	//!< Get dimensions of the screen for the given ID.
	};

}

#endif /* SCREENS_HPP_ */
