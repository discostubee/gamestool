
#ifndef UNITTESTFIGMENTS_HPP
#define UNITTESTFIGMENTS_HPP

	#if defined(DEBUG) && defined(GTUT)
		#include "figment.hpp"

		namespace gt{
			//- Some helpful 'fake' figments.

			//!\brief	A really simple figment used in tests, just used to store a single number.
			class cTestNum: public cFigment, private tOutline<cTestNum>{
			public:
				static const cPlugTag *xPT_num;
				static const cCommand::dUID	xGetData;

				cTestNum();
				virtual ~cTestNum();

				static const char* identify(){ return "test number"; }
				virtual const char* name() const{ return cTestNum::identify(); }
				virtual dNameHash hash() const{ return tOutline<cTestNum>::hash(); }

				virtual void save(cByteBuffer* pAddHere);
				virtual void loadEat(cByteBuffer* pBuff, dReloadMap* pReloads = NULL);

			private:
				tPlug<int> myNum;

				void patGetData(ptrLead aLead);
			};
		}
	#endif

#endif