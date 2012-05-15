/*
 * !\file	plug.hpp
 * !\brief
 */

#ifndef PLUG_HPP
#define PLUG_HPP

#include "lead.hpp"
#include "binPacker.hpp"
#include <vector>


///////////////////////////////////////////////////////////////////////////////////
// Object types
namespace gt{

	template<typename A> class tPlug;

	//----------------------------------------------------------------------------------------------------------------
	//!\brief	Provides serialization (as a healthy breakfast). Get it, cereal, haha haha, uuuuhhh.
	template<typename A>
	class tPlugFlakes: public cBase_plug{
	public:
		tPlugFlakes(PLUG_TYPE_ID pTI) : cBase_plug(pTI) {}
		virtual ~tPlugFlakes(){}

		virtual void save(cByteBuffer* pSaveHere){
			pSaveHere->add(&getMD());
		}

		virtual void loadEat(cByteBuffer* pChewToy, dReloadMap *aReloads = NULL){
			pChewToy->fill(&getMD());
			pChewToy->trimHead(sizeof(A));
		}

		virtual A& getMD() = 0;
	};

	//----------------------------------------------------------------------------------------------------------------
	//!\brief	Needs to be nested in plug shadows.
	template<typename A>
	struct tShadow{
		eShadowMode mMode;
		tPlug<A>* mData;	//!< If the shadow isn't used, this is null.
	};

	//----------------------------------------------------------------------------------------------------------------
	//!\brief	The plug shadow allows you to have 1 copy of a plug per thread that connects to it (as leads can only
	//!			be used by the context that makes them. This does not make all the functions in other plug classes
	//!			threadsafe (although any implemented here are threadsafe), unless they are accessed via the get
	//!			shadow function, or if update is called first.
	//!\todo	make it smart enough to not bother with shadows unless it has more than one context connected.
	template<typename A>
	class tPlugShadows: public tPlugFlakes<A>{
	public:
		tPlugShadows(PLUG_TYPE_ID pTI);
		virtual ~tPlugShadows();

		virtual void linkLead(cLead* pLead);	//!<\note Threadsafe
		virtual void unlinkLead(cLead* pLead);	//!<\note Threadsafe

		#ifdef GT_THREADS
			virtual cUpdateLemming update(); //!< locks all the connected leads and updates the shadows. The update finished when the lemming dies.
		#endif

		virtual	cBase_plug& operator= (const cBase_plug &pD);
		virtual bool operator== (const cBase_plug &pD);

		virtual A& getMD() =0;

	protected:

		#ifdef GT_THREADS

			virtual cBase_plug* getShadow(dConSig aCon, eShadowMode whatFor);
			virtual void finishUpdate();

			friend class cUpdateLemming;
		#endif

	friend class cLead;

	private:
		tPlugShadows& operator=(const tPlugShadows<A> &other){ return *this; }

		#ifdef GT_THREADS
			typedef boost::lock_guard<boost::recursive_mutex> dMuLock;
			typedef std::vector< tShadow<A> > dVecShadow;

			boost::recursive_mutex muMap;

			dVecShadow mShadows;
			typename dVecShadow::iterator itrShadow;	//!< handy.
		#endif
	};

	//----------------------------------------------------------------------------------------------------------------
	template<typename A>
	class tPlug: public tPlugShadows<A>{
	public:
		A mD;	//!< Data: This is public so that the owner can have easy access to it.

		tPlug();
		tPlug(const A& pA);
		tPlug(const tPlug<A> &other);
		tPlug(const cBase_plug *other);
		virtual ~tPlug();

		virtual cBase_plug& operator= (const cBase_plug &pD);
		virtual bool operator== (const cBase_plug &pD);

		cBase_plug& operator= (const tPlug<A> &other);
		cBase_plug& operator= (const A& pA);

		virtual A& getMD();

	private:
		void genericCopy(const cBase_plug* pD);
	};
}

///////////////////////////////////////////////////////////////////////////////////
// Templates
namespace gt{

	//--------------------------------------
	template<typename A>
	tPlugShadows<A>::tPlugShadows(PLUG_TYPE_ID pTI) : tPlugFlakes<A>(pTI){
	}

	template<typename A>
	tPlugShadows<A>::~tPlugShadows(){
		try{
			#ifdef GT_THREADS
				dMuLock lock(muMap);
			#endif
			PROFILE;

			for(
				tPlugFlakes<A>::itrLead = tPlugFlakes<A>::mLeadsConnected.begin();
				tPlugFlakes<A>::itrLead != tPlugFlakes<A>::mLeadsConnected.end();
				++tPlugFlakes<A>::itrLead
			){
				try{
					tPlugFlakes<A>::itrLead->first->unplug(this);	//- the count is irrelevant.
				}catch(excep::base_error &e){
					WARN(e);
				}catch(...){
					//- carry on removing.
				}
			}

			#ifdef GT_THREADS
				for(itrShadow = mShadows.begin(); itrShadow != mShadows.end(); ++itrShadow){
					if(itrShadow->mData != NULL)
						delete itrShadow->mData;
				}
			#endif

		}catch(...){
			excep::base_error e("Unknown error when destroying a plug", __FILE__, __LINE__);
			WARN(e);
		}
	}

	#ifdef GT_THREADS
		template<typename A>
		cBase_plug*
		tPlugShadows<A>::getShadow(dConSig aCon, eShadowMode whatFor){
			//- It should be fine to not lock here because this data should be independent of the other threads, and the shadow list should only be updated when all leads are locked.
			if(mShadows[aCon].mMode == eSM_read)	//- these operations should be quick as its a vector.
				mShadows[aCon].mMode = whatFor;
			return mShadows[aCon].mData;
		}

		template<typename A>
		cUpdateLemming
		tPlugShadows<A>::update(){
			PROFILE;
			muMap.lock();	//- lemming unlocks this when it dies and call finish.

			for(	//- Name is qualified but itrLead is in cBase_plug
				tPlugFlakes<A>::itrLead = tPlugFlakes<A>::mLeadsConnected.begin();
				tPlugFlakes<A>::itrLead != tPlugFlakes<A>::mLeadsConnected.end();
				++tPlugFlakes<A>::itrLead
			)
				tPlugFlakes<A>::itrLead->first->muLead.lock();	//- let any mutex throws go all the way and cause the program to exit.

			for(itrShadow = mShadows.begin(); itrShadow != mShadows.end(); ++itrShadow){
				if(itrShadow->mData != NULL){
					if(itrShadow->mMode == eSM_write)
						getMD() = itrShadow->mData->mD;
				}
			}

			return cUpdateLemming(this);
		}

		template<typename A>
		void
		tPlugShadows<A>::finishUpdate(){
			for(itrShadow = mShadows.begin(); itrShadow != mShadows.end(); ++itrShadow){
				if(itrShadow->mData != NULL){
					itrShadow->mMode = eSM_read;
					itrShadow->mData->mD = getMD();
				}
			}

			for(
				tPlugFlakes<A>::itrLead = tPlugFlakes<A>::mLeadsConnected.begin();
				tPlugFlakes<A>::itrLead != tPlugFlakes<A>::mLeadsConnected.end();
				++tPlugFlakes<A>::itrLead
			)
				tPlugFlakes<A>::itrLead->first->muLead.unlock();

			muMap.unlock();
		}
	#endif

	template<typename A>
	void
	tPlugShadows<A>::linkLead(cLead* pLead){
		#ifdef GT_THREADS
			dMuLock lock(muMap);

			PROFILE;
			ASRT_NOTNULL(pLead);

			if(mShadows.size() < pLead->mConx +1){
				tShadow<A> stamp = { eSM_read, NULL };
				mShadows.resize(pLead->mConx +1, stamp);
			}

			itrShadow = mShadows.begin() + pLead->mConx;
			itrShadow->mData = new tPlug<A>(getMD());
		#endif

		cBase_plug::linkLead(pLead);
	}

	template<typename A>
	void
	tPlugShadows<A>::unlinkLead(cLead* pLead){
		#ifdef GT_THREADS
			dMuLock lock(muMap);
		#endif

		cBase_plug::unlinkLead(pLead);
	}

	template<typename A>
	cBase_plug&
	tPlugShadows<A>::operator= (const cBase_plug &pD){
		//todo
		return *this;
	}

	template<typename A>
	bool
	tPlugShadows<A>::operator== (const cBase_plug &pD){
		//todo
		return false;
	}


	//--------------------------------------
	template<typename A>
	tPlug<A>::tPlug():
		tPlugShadows<A>(typeid(A))
	{}

	template<typename A>
	tPlug<A>::tPlug(const A& pA):
		tPlugShadows<A>(typeid(A)), mD(pA)
	{}

	template<typename A>
	tPlug<A>::tPlug(const tPlug<A> &other) :
		tPlugShadows<A>(typeid(A)), mD(other.mD)
	{}

	template<typename A>
	tPlug<A>::tPlug(const cBase_plug *other) :
		tPlugShadows<A>(other->mType)
	{
		other->copyInto(&mD);
	}

	template<typename A>
	tPlug<A>::~tPlug(){
	}

	template<typename A>
	cBase_plug&
	tPlug<A>::operator= (const cBase_plug &pD){
		NOTSELF(&pD);
		pD.copyInto(&mD);
		return *this;
	}

	template<typename A>
	cBase_plug&
	tPlug<A>::operator= (const tPlug<A> &other){
		NOTSELF(&other);
		mD = other.mD;
		return *this;
	}

	template<typename A>
	bool
	tPlug<A>::operator== (const cBase_plug &pD){
		return (pD.mType == tPlugShadows<A>::mType);
	}

	template<typename A>
	cBase_plug&
	tPlug<A>::operator= (const A& pA){
		mD = pA;
		return *this;
	}

	template<typename A>
	A&
	tPlug<A>::getMD(){
		return mD;
	}
}

///////////////////////////////////////////////////////////////////////////////////
// Template specializations.
namespace gt{

	//!\brief	plug leads are illegal. Don't make much sense anyhow.
	template<>
	class tPlug<cLead>: public tPlugShadows<cLead>{
		//--- I AM BANNED COZ I BREAK THINGS ---///
	};
}


#endif
