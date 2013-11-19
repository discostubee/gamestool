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
 * !\file	utils.hpp
 * !\brief	Contains all kinds of stand alone tools. This is mostly a grab bag where something will get it's own file once it matures.
 */

#ifndef DRAFTALL_HPP
#define DRAFTALL_HPP

#include "figment.hpp"
#include "chainLink.hpp"
#include "anchor.hpp"
#include "runList.hpp"
#include "alias.hpp"
#include "figFactory.hpp"
#include "textFig.hpp"
#include "valve.hpp"
#include "fileIO.hpp"
#ifdef GT_THREADS
#	include "thread.hpp"
#endif

namespace gt{

	void draftAll(){
		tOutline<cFigment>::draft();
		tOutline<cEmptyFig>::draft();
		tOutline<cWorldShutoff>::draft();
		tOutline<cChainLink>::draft();
		tOutline<cAnchor>::draft();
		tOutline<cRunList>::draft();
		tOutline<cAlias>::draft();
		tOutline<cFigFactory>::draft();
		tOutline<cTextFig>::draft();
		tOutline<cValve>::draft();
		tOutline<cBase_fileIO>::draft();
#		ifdef GT_THREADS
			tOutline<cThread>::draft();
#		endif
	}

	void undraftAll(){
		tOutline<cFigment>::remove();
		tOutline<cEmptyFig>::remove();
		tOutline<cWorldShutoff>::remove();
		tOutline<cChainLink>::remove();
		tOutline<cAnchor>::remove();
		tOutline<cRunList>::remove();
		tOutline<cAlias>::remove();
		tOutline<cFigFactory>::remove();
		tOutline<cTextFig>::remove();
		tOutline<cValve>::remove();
		tOutline<cBase_fileIO>::remove();
#		ifdef GT_THREADS
			tOutline<cThread>::remove();
#		endif
	}
}

#endif
