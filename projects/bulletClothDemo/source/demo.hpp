/*
 * !\file
 * !\brief
 *
 *  Copyright (C) 2010  Stuart Bridgens
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef BULLETDEMO_HPP
#define BULLETDEMO_HPP

#include <gt_base/figment.hpp>

#include <addonX11GL/openGL_windowFrame.hpp>

namespace gt{

	//-------------------------------------------------------------------------------------
	//!\brief	A self contained demo figment.
	class cBulletDemo: public cFigment, public tOutline<cBulletDemo>{
	public:
		cBulletDemo();
		virtual ~cBulletDemo();

		static const dNatChar* identify(){ return "bullet demo"; }
		virtual const dNatChar* name() const { return identify(); }
		virtual dNameHash hash() const { return tOutline<cBulletDemo>::hash(); }

		virtual void run(cContext* pCon);

	private:
		void cube();
		void ground();
	};
}

#endif
