/*
**********************************************************************************************************
 *  Copyright (C) 2010  Stuart Bridgens
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License (version 3) as published by
 *  the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *********************************************************************************************************
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

		static const dNatChar* identify(){ return "text fig"; }
		virtual const dNatChar* name() const{ return cTextFig::identify(); }
		virtual dNameHash hash() const{ return tOutline<cTextFig>::hash(); }

		virtual void run(cContext* pCon);				//!< Temp code to display text in console.
		virtual void save(cByteBuffer* pAddHere);
		virtual void loadEat(cByteBuffer* pBuff, dReloadMap* pReloads);

	protected:
		void patSetText(ptrLead aLead);
		void patGetText(ptrLead aLead);

	private:
		tPlug<dText> mText;
	};
}


#endif
