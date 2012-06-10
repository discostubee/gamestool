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

#include "thread.hpp"

using namespace gt;

const cPlugTag*	cThread::xPT_fig = tOutline<cThread>::makePlugTag("figment");

const cCommand::dUID cThread::xLinkFig = tOutline<cThread>::makeCommand(
	"link fig",
	&cThread::patLink,
	cThread::xPT_fig,
	NULL
);

void
cThread::runThread(cThread *me, cContext* pCon){
	ASRT_NOTNULL(me);	ASRT_NOTNULL(pCon);
#ifdef GT_THREADS
	try{
		dLock syncLock(me->syncMu);
		dLock lockFinish(me->finishMu);
		cContext newContext(*pCon);
		me->sync.notify_one();	// let the figment know we're ready.

		while(!me->threadStop){
			newContext.runJackJobs();
			me->link.get()->run(&newContext);
			me->sync.wait(syncLock);
		}
	}catch(excep::base_error &e){
		WARN(e);//std::cout << e.what() << std::endl;	//!!!
	}catch(std::exception &e){
		//std::cout << e.what() << std::endl;	//!!!
	}catch(...){
		UNKNOWN_ERROR; //std::cout << "unknown error in thread" << std::endl;	//!!!
	}
#else
	DUMB_REF_ARG(me); DUMB_REF_ARG(pCon);
#endif
}

#ifdef GT_THREADS
cThread::cThread() :
	threadStop(true), firstRun(true)
#else
cThread::cThread()
#endif
{
	addUpdRoster(&link);
}

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
	updatePlugs();
	if(!link.get().valid()){
		stop(pCon);
		return;
	}

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
	link.get()->run(pCon);
#endif
	stop(pCon);
}

void
cThread::patLink(ptrLead aLead){
	link = aLead->getPlug(xPT_fig);
}


////////////////////////////////////////////////////////////
// Tests
#if defined(GTUT) && defined(GT_THREADS)

GTUT_START(test_cThread, test_suit){
	figmentTestSuit<cThread>();
}GTUT_END;

namespace gt{

	class cShareTarget : public cFigment, private tOutline<cShareTarget>{
	protected:
		tPlug<std::string> chatter;
		tPlug<int> hits;

		void patWrite(ptrLead aLead){
			tPlug<std::string> tmp = aLead->getPlug(xPT_word);
			chatter.get().append( tmp.get() );

			chatter.get().append( "." );
			++hits.get();
			#ifndef GT_SPEEDTEST
				boost::this_thread::sleep(boost::posix_time::milliseconds(10));
			#endif
		}
		void patHits(ptrLead aLead){
			aLead->addPlug(&hits, xPT_hits);
		}
		void patGetChatter(ptrLead aLead){
			aLead->addPlug(&chatter, xPT_chatter);
		}

	public:
		static const cPlugTag* xPT_word;
		static const cPlugTag* xPT_hits;
		static const cPlugTag* xPT_chatter;
		static const cCommand::dUID xWrite;
		static const cCommand::dUID xGetHits;
		static const cCommand::dUID xGetChatter;

		static const dPlaChar* identify() { return "don't care target"; }
		virtual const dPlaChar* name() const { return identify(); }		//!< Virtual version of identify.
		virtual dNameHash hash() const { return tOutline<cShareTarget>::hash(); }

		cShareTarget(): hits(0) { addUpdRoster(&chatter); addUpdRoster(&hits); }
		virtual ~cShareTarget(){}

	};
	const cPlugTag* cShareTarget::xPT_word = tOutline<cShareTarget>::makePlugTag("word");
	const cPlugTag* cShareTarget::xPT_hits = tOutline<cShareTarget>::makePlugTag("hits");
	const cPlugTag* cShareTarget::xPT_chatter = tOutline<cShareTarget>::makePlugTag("chatter");
	const cCommand::dUID cShareTarget::xWrite = tOutline<cShareTarget>::makeCommand("write", &cShareTarget::patWrite, xPT_word, NULL);
	const cCommand::dUID cShareTarget::xGetHits = tOutline<cShareTarget>::makeCommand("get hits", &cShareTarget::patHits, xPT_hits, NULL);
	const cCommand::dUID cShareTarget::xGetChatter = tOutline<cShareTarget>::makeCommand("get chatter", &cShareTarget::patGetChatter, xPT_chatter, NULL);

	//- The following are shallow test classes not meant for rugged use.
	class cWriter : public cFigment, private tOutline<cWriter>{
	private:
		tPlug<ptrFig> target;
		tPlug<std::string> phrase;

		void patSetup(ptrLead aLead){
			target = aLead->getPlug(xPT_target);
			phrase = aLead->getPlug(xPT_word);
		}
	public:
		static const cCommand::dUID xSetup;
		static const cPlugTag *xPT_word, *xPT_target;

		cWriter(){ addUpdRoster(&target); addUpdRoster(&phrase); }
		cWriter(cShareTarget *inT, std::string inS) : target(inT), phrase(inS) {}
		virtual ~cWriter() {}

		static const dPlaChar* identify(){ return "test writer"; }
		virtual const dPlaChar* name() const { return identify(); }
		virtual dNameHash hash() const { return tOutline<cWriter>::hash(); }

		virtual void run(cContext* pCon) {
			start(pCon);
			updatePlugs();

				//- Really inefficient, but who cares.
				ptrLead writeLead = gWorld.get()->makeLead(cShareTarget::xWrite, pCon->getSig());
				writeLead->addPlug(&phrase, cShareTarget::xPT_word);
				target.get()->jack(writeLead, pCon);
			stop(pCon);
		}

	};

	const cPlugTag* cWriter::xPT_word = tOutline<cWriter>::makePlugTag("word");
	const cPlugTag* cWriter::xPT_target = tOutline<cWriter>::makePlugTag("target");
	const cCommand::dUID cWriter::xSetup = tOutline<cWriter>::makeCommand(
		"setup", &cWriter::patSetup, xPT_word, xPT_target, NULL
	);

	GTUT_START(test_Thread, sharedData){
		tOutline<cShareTarget>::draft();
		tOutline<cWriter>::draft();
		tOutline<cThread>::draft();

		const int timeout = 10000;
		const int testLength = 20;
		int testCount = 0;
		int time = 0;
		cContext fakeContext;
		tPlug<std::string> AChatter;
		tPlug<std::string> BChatter;
		tPlug<ptrFig> share = gWorld.get()->makeFig(getHash<cShareTarget>());
		tPlug<ptrFig> writerA = gWorld.get()->makeFig(getHash<cWriter>());
		tPlug<ptrFig> writerB = gWorld.get()->makeFig(getHash<cWriter>());
		tPlug<ptrFig> threadA = gWorld.get()->makeFig(getHash<cThread>());
		tPlug<ptrFig> threadB = gWorld.get()->makeFig(getHash<cThread>());

		AChatter = std::string("cat");
		BChatter = std::string("dog");

		GTUT_ASRT(AChatter.get().length() == BChatter.get().length(), "you didn't choose 2 strings of equal length.");
		{
			{
				ptrLead setupA = gWorld.get()->makeLead(cWriter::xSetup, fakeContext.getSig());
				ptrLead setupB = gWorld.get()->makeLead(cWriter::xSetup, fakeContext.getSig());
				setupA->addPlug(&share, cWriter::xPT_target);
				setupB->addPlug(&share, cWriter::xPT_target);
				setupA->addPlug(&AChatter, cWriter::xPT_word);
				setupB->addPlug(&BChatter, cWriter::xPT_word);
				writerA.get()->jack(setupA, &fakeContext);
				writerB.get()->jack(setupB, &fakeContext);
			}
			{
				ptrLead linkTest = gWorld.get()->makeLead(cThread::xLinkFig, fakeContext.getSig());
				linkTest->addPlug(&writerA, cThread::xPT_fig);
				threadA.get()->jack(linkTest, &fakeContext);
			}
			{
				ptrLead linkTest = gWorld.get()->makeLead(cThread::xLinkFig, fakeContext.getSig());
				linkTest->addPlug(&writerB, cThread::xPT_fig);
				threadB.get()->jack(linkTest, &fakeContext);
			}

			ptrLead getHits = gWorld.get()->makeLead(cShareTarget::xGetHits, fakeContext.getSig());
			while(testCount < testLength){
				threadA.get()->run(&fakeContext);
				threadB.get()->run(&fakeContext);
				share.get()->jack(getHits, &fakeContext);
				getHits->getValue(&testCount, cShareTarget::xPT_hits);
				++time;
				GTUT_ASRT(time < timeout, "timeout when running.");
			}

			{
				tPlug<dPlaStr> chatter;
				ptrLead getChatter = gWorld.get()->makeLead(cShareTarget::xGetChatter, fakeContext.getSig());
				share.get()->jack(getChatter, &fakeContext);

				chatter = getChatter->getPlug(cShareTarget::xPT_chatter);
				DBUG_LO("chatter='" << chatter.get() << "'");

				//- Thanks Dave Sinkula: http://www.daniweb.com/software-development/cpp/threads/27905
				std::stringstream ss(chatter.get());
				std::string token;
				while( getline(ss, token, '.') ){
					if(token.compare(AChatter.get()) != 0 && token.compare(BChatter.get()) != 0){
						GTUT_ASRT(false, "found a corrupt token " << token);
					}
				}
			}

		}

		tOutline<cShareTarget>::removeFromWorld();
		tOutline<cWriter>::removeFromWorld();
	}GTUT_END;

	GTUT_START(test_Thread, threadDestruction){
		const short testLength = 5;
		for(short i=0; i < testLength; ++i){
			cContext fakeContext;
			cThread A, B, C;
			for(short j=0; j < testLength; ++j){
				A.run(&fakeContext);
				B.run(&fakeContext);
				C.run(&fakeContext);
			}
		}
	}GTUT_END;

}
#endif

