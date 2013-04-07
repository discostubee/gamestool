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

#include "ptrTools.hpp"

////////////////////////////////////////////////////////////
// Tests
using namespace gt;

#ifdef GTUT

//	class spitInts: public tSpitLemming<int>{
//	public:
//		spitInts(){}
//		virtual ~spitInts(){}
//
//		void setData(int in) { myData=in; }
//
//		virtual int* getData(const tLemming<int>* requester){
//			return &myData;
//		}
//
//	private:
//		int myData;
//
//	};
//
//	GTUT_START(lemmingSpitter, dataHandling){
//		spitInts testSpit;
//		const int magicNum = 3;
//
//
//	}GTUT_END;
//
//	GTUT_START(lemmingSpitter, inWild){
//		tSpitLemming<int> spitTest;
//		{
//			tLemming<int> lemA = spitTest.get();
//			GTUT_ASRT(spitTest.inWild()==1, "incorrect number of lemmings reported to be in the wild.");
//		}
//		GTUT_ASRT(spitTest.inWild()==0, "incorrect number of lemmings reported to be in the wild.");
//
//		{
//			tLemming<int> lemA = spitTest.get();
//			tLemming<int> lemB = lemA;
//			GTUT_ASRT(spitTest.inWild()==1, "incorrect number of lemmings reported to be in the wild.");
//		}
//		GTUT_ASRT(spitTest.inWild()==0, "incorrect number of lemmings reported to be in the wild.");
//	}GTUT_END;



	class pmorphParent {
	public:
		static int instantCount;
		int num;

		pmorphParent(){ ++instantCount; }
		virtual ~pmorphParent(){ --instantCount; }
		virtual int magic(){ return 3; }
	};
	int pmorphParent::instantCount = 0;

	class pmorphChild : public pmorphParent {
	public:
		pmorphChild(){}
		virtual ~pmorphChild(){}
		virtual int magic(){ return 42; }
	};

	GTUT_START(test_pmorph, polymorphing){
		{
			tPMorphJar<pmorphParent> jarA;

			{
				pmorphParent parent;
				parent.num = 2;
				jarA = parent;
			}
			GTUT_ASRT(jarA.get().num == 2, "jar didn't get int right");

			{
				pmorphChild child;
				child.num = 5;
				jarA = child;
			}
			GTUT_ASRT(jarA.get().num == 5, "jar didn't get int right");
			GTUT_ASRT(jarA.get().magic() == 42, "jar didn't get polymorph right");
		}
		GTUT_ASRT(pmorphParent::instantCount==0, "Didn't delete all jar contents.");

	}GTUT_END;

	GTUT_START(test_pmorph, weakReference){
		pmorphChild *child = new pmorphChild;
		{
			tPMorphJar<pmorphParent> jarA;
			tPMorphJar<pmorphParent> jarB;

			jarA = child;
			GTUT_ASRT(pmorphParent::instantCount==1, "Wrong number of instances.");
			jarB = jarA;
			GTUT_ASRT(pmorphParent::instantCount==1, "Wrong number of instances.");
		}
		delete child;
		GTUT_ASRT(pmorphParent::instantCount==0, "Didn't delete all jar contents.");

	}GTUT_END;

#endif
