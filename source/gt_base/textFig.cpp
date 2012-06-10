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

#include "textFig.hpp"

////////////////////////////////////////////////////////////
using namespace gt;

const cPlugTag* cTextFig::xPT_text = tOutline<cTextFig>::makePlugTag("text");

const cCommand::dUID cTextFig::xSetText = tOutline<cTextFig>::makeCommand(
	"set text", &cTextFig::patSetText,
	cTextFig::xPT_text,
	NULL
);

const cCommand::dUID cTextFig::xGetText = tOutline<cTextFig>::makeCommand(
	"get text", &cTextFig::patGetText,
	cTextFig::xPT_text,
	NULL
);

cTextFig::cTextFig(){
	addUpdRoster(&mText);
}

cTextFig::~cTextFig(){
}

void
cTextFig::patSetText(ptrLead aLead){
	aLead->setPlug(&mText, xPT_text);
}

void
cTextFig::patGetText(ptrLead aLead){
	mText = aLead->getPlug(xPT_text);
}

void
cTextFig::save(cByteBuffer* pAddHere){
	mText.save(pAddHere);
}

void
cTextFig::loadEat(cByteBuffer* pBuff, dReloadMap* pReloads){
	mText.loadEat(pBuff, pReloads);
}


////////////////////////////////////////////////////////////

#ifdef GTUT

GTUT_START(test_string, platformToText){
	const char *sample = "てすと";
	dText textA = toText(sample);
	dStr strB = toPStr(textA);
	GTUT_ASRT(strcmp(sample, strB.c_str())==0, "failed to convert");
}GTUT_END;


#endif

