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

void
cThread::runThread(cThread *me){
	ASRT_NOTNULL(me);
#ifdef GT_THREADS
	try{
		dLock syncLock(me->muSync);	//- wait for lock.

		cContext newContext;
		while(!me->threadStop){
			{	//- Update thread context
				dLock lockConx(me->muConx);
				newContext = me->mSharedConx;
			}
			me->mLink.get()->run(&newContext);
			me->sync.wait(syncLock);
		}
	}catch(excep::base_error &e){
		WARN(e);
	}catch(std::exception &e){
		WARN_S(e.what());
	}catch(...){
		UNKNOWN_ERROR;
	}
	me->threading = false;
#else
	DUMB_REF_PAR(me);
#endif
}

#ifdef GT_THREADS
cThread::cThread() :
	threadStop(true), threading(false)
#else
cThread::cThread()
#endif
{}

cThread::~cThread(){
	stopThread();
}

void
cThread::work(cContext* pCon){
	PROFILE;

	if(mLink.get().valid()){
#		ifdef GT_THREADS
			if(!threading){
				dLock lockMake(muSync); // If we don't wait for the thread to be made, it can be possible to deadlock.
				threadStop = false;
				threading = true;

				mSharedConx = *pCon;
				isMultithreading::nowThreading();
				myThread = boost::thread(cThread::runThread, this);

			}else{
				{
					dLock lockConx(muConx);
					mSharedConx = *pCon;
				}
				sync.notify_all();	// Run the thread once if it's waiting.
			}
#		else
			mLink.get()->run(pCon);
#		endif
	}
}

void
cThread::preLink(){
	stopThread();
}


void
cThread::stopThread(){
#	ifdef GT_THREADS
		if(threading){
			PROFILE;
			try{
				threadStop = true;

				if(myThread.joinable()){
					muSync.lock();	//- Wait until we have sync
					muSync.unlock();
					sync.notify_all();
					myThread.timed_join( boost::posix_time::milliseconds(5000) );
				}else{
					WARN_S("Unable to join thread even though we are threading");
				}
			}catch(...){
				WARN_S("Error stopping thread");
			}
		}
#	endif
}

////////////////////////////////////////////////////////////
// Tests
#if defined(GTUT) && defined(GT_THREADS)
# include "unitTestFigments.hpp"

GTUT_START(test_cThread, test_suit){
	figmentTestSuit<cThread>();
}GTUT_END;

namespace gt{

	//- Testing if a thread can cleanup after itself.
	GTUT_START(test_Thread, threadDestruction){
		const short testLength = 3;

		for(short i=0; i < testLength; ++i){
			cContext fakeContext;
			ptrFig A = gWorld.get()->makeFig(getHash<cThread>());
			ptrFig B = gWorld.get()->makeFig(getHash<cThread>());
			ptrFig C = gWorld.get()->makeFig(getHash<cThread>());
			tPlug<ptrFig> testMe = gWorld.get()->makeFig(getHash<cFigment>());
			ptrLead linkIt = gWorld.get()->makeLead(cThread::xSetLink);

			linkIt->linkPlug(&testMe, cThread::xPT_links);
			A->jack(linkIt, &fakeContext);
			B->jack(linkIt, &fakeContext);
			C->jack(linkIt, &fakeContext);

			for(short j=0; j < testLength; ++j){
				A->run(&fakeContext);
				B->run(&fakeContext);
				C->run(&fakeContext);
			}
		}
	}GTUT_END;

	class cShareTarget : public cFigment{
	protected:
		tPlug<dStr> chatter;
		tPlug<int> hits;

		void patWrite(ptrLead aLead){
			tPlug<dStr> tmp;
			aLead->copyPlug(&tmp, xPT_word);
			chatter.get().append( tmp.get() );

			chatter.get().append( "." );
			++hits.get();
#			ifndef GT_SPEEDTEST
				boost::this_thread::sleep(boost::posix_time::milliseconds(10));
#			endif
		}
		void patHits(ptrLead aLead){
			aLead->linkPlug(&hits, xPT_hits);
		}
		void patGetChatter(ptrLead aLead){
			aLead->linkPlug(&chatter, xPT_chatter);
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
		virtual dNameHash hash() const { return getHash<cShareTarget>(); }

		cShareTarget(): hits(0) {}
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
		tPlug<dStr> phrase;

		void patSetup(ptrLead aLead){
			aLead->copyPlug(&target, xPT_target);
			aLead->copyPlug(&phrase, xPT_word);
		}
	public:
		static const cCommand::dUID xSetup;
		static const cPlugTag *xPT_word, *xPT_target;

		cWriter(){}
		cWriter(cShareTarget *inT, dStr inS) : target(inT), phrase(inS) {}
		virtual ~cWriter() {}

		static const dPlaChar* identify(){ return "test writer"; }
		virtual const dPlaChar* name() const { return identify(); }
		virtual dNameHash hash() const { return getHash<cWriter>(); }

		virtual void work(cContext* pCon) {
			//- Really inefficient, but who cares.
			ptrLead writeLead = gWorld.get()->makeLead(cShareTarget::xWrite);
			writeLead->linkPlug(&phrase, cShareTarget::xPT_word);
			target.get()->jack(writeLead, pCon);
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

		gWorld.get()->flushLines();

		const int testLength = 20;
		int testCount = 0;
		cContext fakeContext;
		tPlug<dStr> AChatter;
		tPlug<dStr> BChatter;
		tPlug<ptrFig> share = gWorld.get()->makeFig(getHash<cShareTarget>());
		tPlug<ptrFig> writerA = gWorld.get()->makeFig(getHash<cWriter>());
		tPlug<ptrFig> writerB = gWorld.get()->makeFig(getHash<cWriter>());
		tPlug<ptrFig> threadA = gWorld.get()->makeFig(getHash<cThread>());
		tPlug<ptrFig> threadB = gWorld.get()->makeFig(getHash<cThread>());

		AChatter = "The fat lazy cats";
		BChatter = "The big silly dog";

		GTUT_ASRT(AChatter.get().length() == BChatter.get().length(), "you didn't choose 2 strings of equal length.");
		{
			ptrLead setupA = gWorld.get()->makeLead(cWriter::xSetup);
			ptrLead setupB = gWorld.get()->makeLead(cWriter::xSetup);

			setupA->linkPlug(&share, cWriter::xPT_target);
			setupA->linkPlug(&AChatter, cWriter::xPT_word);

			setupB->linkPlug(&share, cWriter::xPT_target);
			setupB->linkPlug(&BChatter, cWriter::xPT_word);

			writerA.get()->jack(setupA, &fakeContext);
			writerB.get()->jack(setupB, &fakeContext);
		}
		{
			ptrLead linkTest = gWorld.get()->makeLead(cThread::xSetLink);

			linkTest->linkPlug(&writerA, cThread::xPT_links);
			threadA.get()->jack(linkTest, &fakeContext);
		}
		{
			ptrLead linkTest = gWorld.get()->makeLead(cThread::xSetLink);

			linkTest->linkPlug(&writerB, cThread::xPT_links);
			threadB.get()->jack(linkTest, &fakeContext);
		}

		ptrLead getHits = gWorld.get()->makeLead(cShareTarget::xGetHits);
		std::stringstream ss;
		ss << "Hits:";
		while(testCount < testLength){
			threadA.get()->run(&fakeContext);
			threadB.get()->run(&fakeContext);
			share.get()->jack(getHits, &fakeContext);

			tLitePlug<int> tmp(&testCount);
			getHits->copyPlug(&tmp, cShareTarget::xPT_hits);

			ss << tmp.get() << ",";
		}
		DBUG_LO(ss.str());

		{
			tPlug<dStr> chatter;
			ptrLead getChatter = gWorld.get()->makeLead(cShareTarget::xGetChatter);

			share.get()->jack(getChatter, &fakeContext);
			getChatter->copyPlug(&chatter, cShareTarget::xPT_chatter);

			DBUG_LO("chatter='" << chatter.get() << "'");

			//- Thanks Dave Sinkula: http://www.daniweb.com/software-development/cpp/threads/27905
			std::stringstream ss(chatter.get().c_str());
			std::string token;
			while( getline(ss, token, '.') ){
				if(token.compare(AChatter.get()) != 0 && token.compare(BChatter.get()) != 0){
					GTUT_ASRT(false, "found a corrupt token " << token);
				}
			}
		}

	}GTUT_END;

}
#endif

