#ifndef UNITTESTFIGMENTS_HPP
#define UNITTESTFIGMENTS_HPP

#if defined(DEBUG) && defined(GTUT)
#	include "figment.hpp"

namespace gt{
		//- Some helpful 'fake' figments.

		//!\brief	A really simple figment used in tests, just used to store a single number.
		class cTestNum: public cFigment, private tOutline<cTestNum>{
		public:
			static const cPlugTag *xPT_num;
			static const cCommand::dUID	xGetData;
			static const cCommand::dUID	xSetData;

			cTestNum();
			virtual ~cTestNum();

			GT_IDENTIFY("test num");
			virtual dNameHash hash() const{ return getHash<cTestNum>(); }

			virtual void save(cByteBuffer* pAddHere);
			virtual void loadEat(cByteBuffer* pBuff, dReloadMap* pReloads = NULL);

		private:
			tPlug<int> myNum;

			void patGetData(ptrLead aLead);
			void patSetData(ptrLead aLead);
		};


		class cSaveTester: public cFigment, private tOutline<cSaveTester>{
		public:
			static const cPlugTag *xPT_str, *xPT_num;
			static const cCommand::dUID	xGetData;

			cSaveTester(){}
			cSaveTester(const dPlaChar* inStr, int pNum) : myStr(inStr), myNum(pNum) {}
			virtual ~cSaveTester(){}

			static const dPlaChar* identify(){ return "save tester"; }
			virtual const dPlaChar* name() const{ return cSaveTester::identify(); }
			virtual dNameHash hash() const{ return getHash<cSaveTester>(); }

			virtual void save(cByteBuffer* pAddHere) {
				myStr.save(pAddHere); myNum.save(pAddHere);
			}

			virtual void loadEat(cByteBuffer* pBuff, dReloadMap* pReloads = NULL) {
				myStr.loadEat(pBuff, pReloads); myNum.loadEat(pBuff, pReloads);
			}
		private:
			tPlug<dStr> myStr;
			tPlug<int> myNum;

			void patGetData(ptrLead aLead);
		};
	}

#endif

#endif
