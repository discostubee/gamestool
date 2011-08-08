#include "thread.hpp"

using namespace gt;

const tPlugTag*	cThread::xPT_fig = tOutline<cThread>::makePlugTag("figment");

const cCommand* cThread::xLinkFig = tOutline<cThread>::makeCommand(
	"link fig",
	cThread::eLinkFig,
	cThread::xPT_fig,
	NULL
);

void
cThread::runThread(cThread *me, cContext* pCon){
	try{

		dLock syncLock(me->syncMu);
		dLock lockFinish(me->finishMu);
		cContext newContext(*pCon);
		newContext.add(me);
		me->sync.notify_one();	// let the figment know we're ready.

		while(!me->threadStop){
			me->link.mD->run(&newContext);
			me->sync.wait(syncLock);
		}

	}catch(...){
		std::cout << "unknown error in thread" << std::endl;	//!!!
	}
}

cThread::cThread() :
	firstRun(true), threadStop(true)
{}

cThread::~cThread(){
	if(!firstRun){

		{
			dLock lockReady(syncMu);
			threadStop = true;
			sync.notify_all();
		}
		{
			dLock lockFinish(finishMu); // wait for the thread to finish.

			myThread.join();
		}
	}
}

void
cThread::run(cContext* pCon){
	PROFILE;

	start(pCon);
	if(firstRun){
		dLock lockMake(syncMu); // If we don't wait for the thread to be made, it can be possible to deadlock.
		firstRun = false;
		threadStop = false;

		isMultithreading::nowThreading();
		myThread = boost::thread(cThread::runThread, this, pCon);
		sync.wait(lockMake);
	}else{

		sync.notify_all();
	}

	stop(pCon);

}

void
cThread::jack(ptrLead pLead, cContext* pCon){
	PROFILE;

	start(pCon);
	try{
		switch( pLead->mCom->getSwitch<cThread>() ){
			case eLinkFig:
				link = pLead->getPlug(xPT_fig, pCon);
				break;

			default:
				stop(pCon);
				cFigment::jack(pLead, pCon);
				break;
		}
	}catch(excep::base_error &e){
		WARN(e);
	}catch(...){
		UNKNOWN_ERROR;
	}
	stop(pCon);
}

////////////////////////////////////////////////////////////
// Tests
#ifdef GTUT

namespace gt{

	class cShareTarget : public cFigment{
	public:
		static const cCommand xWrite;
		static const cCommand xGetHits;

		enum{
			eWrite,
			eGetHits
		};

		dStr chatter;
		tPlug<size_t> hits;

		static const char* identify() { return "don't care"; }

		cShareTarget(): hits(0) {}
		virtual ~cShareTarget(){}

		virtual void jack(ptrLead pLead, cContext* pCon) {
			start(pCon);
			switch(pLead->mCom->getSwitch<cShareTarget>()){
				case eGetHits:
					pLead->addToPile(&hits, pCon);
					break;

				case eWrite:
					chatter += pLead->getPiledDItr(pCon).getPlug()->getCopy<dStr>();
					++hits.mD;
					break;
			}
			stop(pCon);
		}

	};
	const cCommand cShareTarget::xWrite(0, "don't care", getHash<cShareTarget>(), cShareTarget::eWrite);
	const cCommand cShareTarget::xGetHits(0, "don't care", getHash<cShareTarget>(), cShareTarget::eGetHits);

	//- The following are shallow test classes not meant for rugged use.
	class cWriter : public cFigment{
	public:
		cShareTarget &target;
		tPlug<dStr> phrase;

		cWriter(cShareTarget &inT, dStr inS) : target(inT), phrase(inS) {}

		virtual ~cWriter() {}

		virtual void run(cContext* pCon) {
			start(pCon);
			{
				ptrLead targetLead(new cLead(&cShareTarget::xWrite, pCon));
				targetLead->addToPile(&phrase, pCon);
				target.jack(targetLead, pCon);
			}
			stop(pCon);
		}
	};

	GTUT_START(test_Thread, sharedData){
		const short timeout = 100;
		const short testLength = 5;
		short testCount = 0;
		short time = 0;
		cContext fakeContext;
		cThread threadA, threadB;
		cShareTarget share;
		dStr AChatter = "me.";
		dStr BChatter = "you.";
		tPlug<ptrFig> writerA( ptrFig(new cWriter(share, AChatter)) );
		tPlug<ptrFig> writerB( ptrFig(new cWriter(share, BChatter)) );
		ptrLead getHits(new cLead(&cShareTarget::xGetHits, &fakeContext));

		{
			ptrLead linkTest(new cLead(cThread::xLinkFig, &fakeContext));
			linkTest->add(&writerA, cThread::xPT_fig, &fakeContext);
			threadA.jack(linkTest, &fakeContext);
		}
		{
			ptrLead linkTest(new cLead(cThread::xLinkFig, &fakeContext));
			linkTest->add(&writerB, cThread::xPT_fig, &fakeContext);
			threadB.jack(linkTest, &fakeContext);
		}

		while(testCount < testLength){
			threadA.run(&fakeContext);
			threadB.run(&fakeContext);
			share.jack(getHits, &fakeContext);
			testCount = getHits->getPiledDItr(&fakeContext).getPlug()->getCopy<size_t>();
			++time;
			std::cout<<testCount;
			GTUT_ASRT(time < timeout, "timeout when running.");
		}

		std::cout << "chatter=" << share.chatter << std::endl;

		GTUT_ASRT(share.chatter.length() == (AChatter.length() + BChatter.length()) * testLength, "Shared data isn't the right size.");
	}GTUT_END;

	GTUT_START(test_Thread, threadDestruction){
		const short testLength = 5;
		for(short i=0; i < testLength; ++i){
			cContext fakeContext;
			cThread threadA;
			for(short j=0; j < testLength; ++j){
				threadA.run(&fakeContext);
			}
		}
	}GTUT_END;

}
#endif
