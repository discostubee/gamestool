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

#ifndef PLUG_HPP
#define PLUG_HPP

#include "lead.hpp"
#include "binPacker.hpp"
#include <vector>

///////////////////////////////////////////////////////////////////////////////////
//



///////////////////////////////////////////////////////////////////////////////////
//
namespace gt{
	namespace voidCopiers{
		template<typename A>
		void baseCopy(const A *pFrom, void *pTo){
			*reinterpret_cast<A*>(pTo) = *pFrom;
		}

		void textToNStr(const dText *pFrom, void *pTo);
		void textToPStr(const dText *pFrom, void *pTo);
	}

	template<typename A>
	inline typename tPlug<A>::dMapCopiers*
	getPlugCopiers(){
		static bool setup = false;
		static typename tPlug<A>::dMapCopiers copiers;

		if(!setup){
			copiers[ cBase_plug::getPlugType<A>() ] = voidCopiers::baseCopy<A>;
			setup=true;
		}

		return &copiers;
	}
}


///////////////////////////////////////////////////////////////////////////////////
// Object types
namespace gt{

	template<typename A> class tPlug;

	//----------------------------------------------------------------------------------------------------------------
	//!\brief	Provides serialization (as a healthy breakfast). Get it, cereal, haha haha, uuuuhhh.
	template<typename A>
	class tPlugFlakes: public cBase_plug{
	public:
		tPlugFlakes(dPlugType pTI) :
			cBase_plug(pTI)
		{}

		virtual ~tPlugFlakes(){}

		virtual void save(cByteBuffer* pSaveHere){
			pSaveHere->add(&get());
		}

		virtual void loadEat(cByteBuffer* pChewToy, dReloadMap *aReloads = NULL){
			pChewToy->trimHead( pChewToy->fill(&get()) );
		}

		virtual A& get() = 0;
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
		tPlugShadows(cBase_plug::dPlugType pTI);
		virtual ~tPlugShadows();

		virtual void linkLead(cLead* pLead);	//!<\note Threadsafe
		virtual void unlinkLead(cLead* pLead);	//!<\note Threadsafe

		virtual void updateStart();
		virtual void updateFinish();

		#ifdef GT_THREADS
			virtual void save(cByteBuffer* pSaveHere){
				updateStart();
				tPlugFlakes<A>::save(pSaveHere);
				updateFinish();
			}

			virtual void loadEat(cByteBuffer* pChewToy, dReloadMap *aReloads = NULL){
				updateStart();
				tPlugFlakes<A>::loadEat(pChewToy, aReloads);
				updateFinish();
			}
		#endif

		virtual A& get() = 0;

		virtual	cBase_plug& operator= (const cBase_plug &pD);
		virtual bool operator== (const cBase_plug &pD);

	protected:

		#ifdef GT_THREADS
			virtual cBase_plug* getShadow(dConSig aCon, eShadowMode whatFor);
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
		typedef void (*fuCopyInto)(const A *copyFrom, void *copyTo);
		typedef std::map<cBase_plug::dPlugType, fuCopyInto> dMapCopiers;

		tPlug();
		tPlug(const A& pA);
		tPlug(const tPlug<A> &other);
		tPlug(const cBase_plug *other);
		virtual ~tPlug();

		virtual cBase_plug& operator= (const cBase_plug &pD);
		virtual bool operator== (const cBase_plug &pD);

		cBase_plug& operator= (const tPlug<A> &other);
		cBase_plug& operator= (const A& pA);

		virtual A& get();

	protected:
		virtual void actualCopyInto(void* pContainer, cBase_plug::dPlugType pType) const;

	private:
		A mD;	//!< Data

	};
}

///////////////////////////////////////////////////////////////////////////////////
// Templates
namespace gt{

	//--------------------------------------
	template<typename A>
	tPlugShadows<A>::tPlugShadows(cBase_plug::dPlugType pTI) :
		tPlugFlakes<A>(pTI)
	{}

	template<typename A>
	tPlugShadows<A>::~tPlugShadows(){
		try{
			#ifdef GT_THREADS
				dMuLock lock(muMap);
			#endif

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
			WARN("Unknown error when destroying a plug");
		}
	}

	#ifdef GT_THREADS
		template<typename A>
		cBase_plug*
		tPlugShadows<A>::getShadow(dConSig aCon, eShadowMode whatFor){
			//- It should be fine to not lock here because this data should be independent of the other threads, and the shadow list should only be updated when all leads are locked.
			if(mShadows[aCon].mMode == eSM_read)	//- these operations should be quick as it's a vector.
				mShadows[aCon].mMode = whatFor;
			return mShadows[aCon].mData;
		}
	#endif

	template<typename A>
	void
	tPlugShadows<A>::updateStart(){
		#ifdef GT_THREADS
			PROFILE;
			muMap.lock();	//- lemming unlocks this when it dies and calls finish.

			for(	//- Name is qualified but itrLead is in cBase_plug
				tPlugFlakes<A>::itrLead = tPlugFlakes<A>::mLeadsConnected.begin();
				tPlugFlakes<A>::itrLead != tPlugFlakes<A>::mLeadsConnected.end();
				++tPlugFlakes<A>::itrLead
			)
				tPlugFlakes<A>::itrLead->first->muLead.lock();	//- let any mutex throws go all the way and cause the program to exit.

			for(itrShadow = mShadows.begin(); itrShadow != mShadows.end(); ++itrShadow){
				if(itrShadow->mData != NULL){
					if(itrShadow->mMode == eSM_write)
						get() = itrShadow->mData->get();
				}
			}
		#endif
	}

	template<typename A>
	void
	tPlugShadows<A>::updateFinish(){
		#ifdef GT_THREADS
			for(itrShadow = mShadows.begin(); itrShadow != mShadows.end(); ++itrShadow){
				if(itrShadow->mData != NULL){
					itrShadow->mMode = eSM_read;
					itrShadow->mData->get() = get();
				}
			}

			for(
				tPlugFlakes<A>::itrLead = tPlugFlakes<A>::mLeadsConnected.begin();
				tPlugFlakes<A>::itrLead != tPlugFlakes<A>::mLeadsConnected.end();
				++tPlugFlakes<A>::itrLead
			)
				tPlugFlakes<A>::itrLead->first->muLead.unlock();

			muMap.unlock();
		#endif
	}

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
			itrShadow->mData = new tPlug<A>(get());
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
		tPlugShadows<A>(cBase_plug::getPlugType<A>())
	{}

	template<typename A>
	tPlug<A>::tPlug(const A& pA):
		tPlugShadows<A>(cBase_plug::getPlugType<A>()),
		mD(pA)
	{}

	template<typename A>
	tPlug<A>::tPlug(const tPlug<A> &other) :
		tPlugShadows<A>(cBase_plug::getPlugType<A>()),
		mD(other.mD)
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
	tPlug<A>::get(){
		return mD;
	}

	template<typename A>
	void
	tPlug<A>::actualCopyInto(void* pContainer, cBase_plug::dPlugType pType) const{
		typename dMapCopiers::iterator itrCopiers = getPlugCopiers<A>()->find( cBase_plug::getPlugType<A>() );
		if(itrCopiers != getPlugCopiers<A>()->end()){
			itrCopiers->second( &mD, pContainer );
		}else{
			throw excep::cantCopy(typeid(A).name(), "unknown", __FILE__, __LINE__);
		}
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

	template<>
	inline tPlug<dText>::dMapCopiers*
	getPlugCopiers<dText>(){
		static bool setup = false;
		static tPlug<dText>::dMapCopiers copiers;

		if(!setup){
			copiers[ cBase_plug::getPlugType<dText>() ] = voidCopiers::baseCopy<dText>;
			copiers[ cBase_plug::getPlugType<dNatStr>() ] = voidCopiers::textToNStr;
			copiers[ cBase_plug::getPlugType<dStr>() ] = voidCopiers::textToPStr;
			setup=true;
		}

		return &copiers;
	}
}


#endif
