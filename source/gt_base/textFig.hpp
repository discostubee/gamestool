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
	private:
		cPlug<dStr> mText;

	public:
		static const cPlugTag* xPT_text;
		static const cCommand* xSetText;
		static const cCommand* xGetText;

		enum{
			eSetText = cFigment::eSwitchEnd,
			eGetText,
			eSwitchEnd
		};

		static const dNatChar* identify(){ return "text fig"; }
		static void draft(){ tOutline<cTextFig>::draft(); }
		static void requirements();

		cTextFig();
		virtual ~cTextFig();

		virtual const dNatChar* name() const{ return cTextFig::identify(); }
		virtual dNameHash hash() const{ return tOutline<cTextFig>::hash(); }

		virtual void run(cContext* pCon);				//!< Temp code to display text in console.
		virtual void jack(ptrLead pLead);
		virtual cByteBuffer& save();
		virtual void loadEat(cByteBuffer* pBuff, dReloadMap* pReloads);
	};
}

#endif
