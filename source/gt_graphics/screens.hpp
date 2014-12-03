#ifndef SCREENS_HPP
#define SCREENS_HPP

#include "stage.hpp"

namespace gt{

	//!\brief	Provides information about a screen.
	class cScreen: public cFigment{
	public:
		typedef unsigned short dID;

		static const cPlugTag* xPT_rect;	//!< The screen's rectum... ha... in relation to screen 1.
		static const cPlugTag* xPT_ID;	//!< The ID of the screen, starting from 1.
		static const cCommand::dUID xSetID;	//!< Screens default to ID 1. Set this to get info about other screens.
		static const cCommand::dUID xGetShape;	//!< Get the dimensions and ID of this screen. All 0 if this screen is not attached.
		static const cCommand::dUID xGetAttachedIDs;	//!< Returns a list of IDs for attached screens.

		cScreen();
		virtual ~cScreen();

		GT_IDENTIFY("screen");
		GT_EXTENDS(cFigment);
		GT_VERSION(1);
		virtual dNameHash hash() const{ return getHash<cScreen>(); }

	protected:
		tPlug<dID> mID;

		void patGetShape(ptrLead pLead);
		void patSetID(ptrLead pLead);
		void patGetAttached(ptrLead pLead);

		//!< Get dimensions of the screen for the current ID.
		virtual shape::tRectangle<dUnitPix> getDims();
		virtual std::vector<dID> getIDs();
	};

}

#endif /* SCREENS_HPP_ */
