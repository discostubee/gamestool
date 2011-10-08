/*
 * textFig.hpp
 *
 *  Created on: 19/05/2010
 *      Author: stuandlou
 */

#ifndef TEXTFIG_HPP
#define TEXTFIG_HPP

#include "figment.hpp"

namespace gt{

	//!\note	Stores string types in a format that is most efficient to this compilation of gamestool.
	//!			When saving, it is converted into a string format that is consistent across all compilations.
	class cTextFig: public cFigment, private tOutline<cTextFig>{
	public:
		static const cPlugTag* xPT_text;
		static const cCommand::dUID xSetText;
		static const cCommand::dUID xGetText;

		cTextFig();
		virtual ~cTextFig();

		static const char* identify(){ return "text fig"; }
		virtual const char* name() const{ return cTextFig::identify(); }
		virtual dNameHash hash() const{ return tOutline<cTextFig>::hash(); }

		virtual void run(cContext* pCon);				//!< Temp code to display text in console.
		virtual void save(cByteBuffer* pAddHere);
		virtual void loadEat(cByteBuffer* pBuff, dReloadMap* pReloads);

	protected:
		void patSetText(ptrLead aLead);
		void patGetText(ptrLead aLead);

	private:
		tPlug<std::string> mText;
	};
}

#endif
