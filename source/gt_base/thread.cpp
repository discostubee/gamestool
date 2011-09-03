#include "thread.hpp"

using namespace gt;

const cPlugTag*	cThread::xPT_fig = tOutline<cThread>::makePlugTag("figment");

const cCommand* cThread::xLinkFig = tOutline<cThread>::makeCommand(
	"link fig",
	cThread::eLinkFig,
	cThread::xPT_fig,
	NULL
);

void
cThread::runThread(cThread *me, cContext* pCon){
#ifdef GT_THREADS
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
	}catch(excep::base_error &e){
		std::cout << e.what() << std::endl;	//!!!
	}catch(std::exception &e){
		std::cout << e.what() << std::endl;	//!!!
	}catch(...){
		std::cout << "unknown error in thread" << std::endl;	//!!!
	}
#endif
}

#ifdef GT_THREADS
cThread::cThread() :
	threadStop(true), firstRun(true)
{}
#else
cThread::cThread()
{}
#endif

cThread::~cThread(){
#ifdef GT_THREADS
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
#endif
}

void
cThread::run(cContext* pCon){
	PROFILE;

	start(pCon);
#ifdef GT_THREADS
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
#else
	link.mD->run(pCon);
#endif
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
				stop(pCon, true);
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
#if defined(GTUT) && defined(GT_THREADS)

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
		const short timeout = 1000;
		const short testLength = 5;
		short testCount = 0;
		short time = 0;
		cContext fakeContext;
		cShareTarget share;
		dStr AChatter = "dog.";
		dStr BChatter = "cat.";
		tPlug<ptrFig> writerA( ptrFig(new cWriter(share, AChatter)) );
		tPlug<ptrFig> writerB( ptrFig(new cWriter(share, BChatter)) );
		ptrLead getHits(new cLead(&cShareTarget::xGetHits, &fakeContext));

		GTUT_ASRT(AChatter.length() == BChatter.length(), "you didn't choose 2 strings of equal length.");
		{
			cThread threadA, threadB;	//- These must be cleaned first to prevent class definitions being cleaned.
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
				//std::cout<<testCount;
				GTUT_ASRT(time < timeout, "timeout when running.");
			}

			std::cout << "chatter=" << share.chatter << std::endl;

			GTUT_ASRT(share.chatter.length() == (AChatter.length() * testCount), "Shared data isn't the right size.");
		}
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

