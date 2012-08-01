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
		virtual const A& getConst() const =0;

	protected:

	};

#	ifdef GT_THREADS

	//!\brief	Used to indicate how shadows are effects, and how the effect the source.
	enum eShadowMode{
		eSM_init,	//!< Initial value.
		eSM_read,	//!< The data has only be read from, or nothing has happened. So just update the shadow and nothing else.
		eSM_link,	//!< The links have changed and the source needs to change accordingly.
		eSM_write,	//!< Data is written to the source.
		eSM_all		//!< Update both the data and the link
	};

	//----------------------------------------------------------------------------------------------------------------
	//!\brief	The shadow plug allows you to manipulate a plug in a multi-threaded environment, while avoiding deadlocks.
	//!			It does this by using copies/shadows of its data per thread that tries to access it via the lead. When it runs,
	//!			it then updates these copies/shadows and the origin depending on what was done to the copy.
	template<typename A>
	class tShadowPlug: public tPlugFlakes<A>{
	public:
		tShadowPlug(cBase_plug::dPlugType pTI);
		virtual ~tShadowPlug();

		virtual void linkLead(cLead* pLead);	//!< Adds the link as a shadow update.
		virtual void unlinkLead(cLead* pLead);	//!< Adds the unlink as a shadow update.
		virtual void updateStart();
		virtual void updateFinish();

		virtual void save(cByteBuffer* pSaveHere){
			updateStart();
			tPlugFlakes<A>::save(pSaveHere);
			updateFinish();
		}

		virtual	cBase_plug& operator= (const cBase_plug &pD) =0;
		virtual bool operator== (const cBase_plug &pD) const =0;

		virtual A& get() = 0;
		virtual const A& getConst() const =0;

	protected:
		virtual void readShadow(cBase_plug *pWriteTo, dConSig aCon);
		virtual void writeShadow(const cBase_plug *pReadFrom, dConSig aCon);

	friend class cLead;

	private:
		struct tShadow{
			eShadowMode mMode;
			std::list<cLead*> links, unlinks;
			A mData;	//!< If the shadow isn't used, this is null.
		};

		typedef boost::lock_guard<boost::recursive_mutex> dLock;
		typedef std::vector< tShadow* > dVecShadow;

		boost::recursive_mutex guardShadows;
		dVecShadow mShadows;
		typename dVecShadow::iterator itrShadow;	//!< handy.
		tShadow *tmpSRef;	//!< handy.
		std::list<cLead*>::iterator itrSLeads;

		tShadowPlug& operator=(const tShadowPlug<A> &other){ return *this; }	//!< Banned.
		void setTmpShadowRef(dConSig pSig);
	};
#	endif

	//----------------------------------------------------------------------------------------------------------------
	//!\brief	Handy way to select which parent to use. Can't typedef and macro is evil, so the next best thing to do
	//!			is this.
#	ifdef GT_THREADS
		template<typename A>
		class tPlugParent : public tShadowPlug<A> {
		public:
			tPlugParent(cBase_plug::dPlugType pTI) : tShadowPlug<A>(pTI) {}
			virtual ~tPlugParent() {}
		};
#	else
		template<typename A>
		class tPlugParent : public tPlugFlakes<A> {
		public:
			tPlugParent(cBase_plug::dPlugType pTI) : tPlugFlakes<A>(pTI) {}
			virtual ~tPlugParent() {}
		};
#	endif

	//----------------------------------------------------------------------------------------------------------------
	//!\brief	Implements cBase_plug.
	//!\note	Refer to the cBase_plug class for more info.
	template<typename A>
	class tPlug: public tPlugParent<A>
	{
	public:
		typedef void (*fuCopyInto)(const A *copyFrom, void *copyTo);
		typedef std::map<cBase_plug::dPlugType, fuCopyInto> dMapCopiers;

		tPlug();
		tPlug(const A& pA);
		tPlug(const tPlug<A> &other);
		tPlug(const cBase_plug *other);
		virtual ~tPlug();

		virtual cBase_plug& operator= (const cBase_plug &pD);
		virtual bool operator== (const cBase_plug &pD) const;

		virtual cBase_plug& operator= (const tPlug<A> &other);
		virtual cBase_plug& operator= (const A& pA);

		virtual A& get();

		virtual const A& getConst() const;

	protected:
		virtual void actualCopyInto(void* pContainer, cBase_plug::dPlugType pType) const;
		virtual void actualCopyFrom(const void* pContainer, cBase_plug::dPlugType pType);

	private:
		A mD;	//!< Data

	};
}

///////////////////////////////////////////////////////////////////////////////////
// Templates
namespace gt{

#	ifdef GT_THREADS

		//--------------------------------------
		template<typename A>
		tShadowPlug<A>::tShadowPlug(cBase_plug::dPlugType pTI) :
			tPlugFlakes<A>(pTI)
		{}

		template<typename A>
		tShadowPlug<A>::~tShadowPlug(){
			typedef tPlugFlakes<A> p;
			try{
				dLock lock(guardShadows);

				//- We need to unplug first before destroying shadows.
				for(
					p::itrLead = p::mLeadsConnected.begin();
					p::itrLead != p::mLeadsConnected.end();
					++p::itrLead
				)
					p::itrLead->first->unplug(this);

				p::mLeadsConnected.clear();

				for(itrShadow = mShadows.begin(); itrShadow != mShadows.end(); ++itrShadow)
					delete (*itrShadow);

			}catch(...){
				WARN_S("Unknown error when destroying a plug");
			}
		}


		template<typename A>
		void
		tShadowPlug<A>::linkLead(cLead* pLead){
			PROFILE;

			dLock lock(guardShadows);
			setTmpShadowRef(pLead->getCurrentSig());
			tmpSRef->links.push_back(pLead);
			//- may not be needed.
			/*if(tmpSRef->mMode != eSM_read)
				tmpSRef->mMode = eSM_all;
			else
				tmpSRef->mMode = eSM_link;*/
		}

		template<typename A>
		void
		tShadowPlug<A>::unlinkLead(cLead* pLead){
			PROFILE;

			dLock lock(guardShadows);
			setTmpShadowRef(pLead->getCurrentSig());
			tmpSRef->unlinks.push_back(pLead);
			/*if(tmpSRef->mMode != eSM_read)
				tmpSRef->mMode = eSM_all;
			else
				tmpSRef->mMode = eSM_link;*/
		}

		template<typename A>
		void
		tShadowPlug<A>::updateStart(){
			PROFILE;

			dLock lock(guardShadows);

			for(itrSLeads = tmpSRef->links.begin(); itrSLeads != tmpSRef->links.end(); ++itrSLeads)
				cBase_plug::linkLead(*itrSLeads);

			for(itrSLeads = tmpSRef->unlinks.begin(); itrSLeads != tmpSRef->unlinks.end(); ++itrSLeads)
				cBase_plug::unlinkLead(*itrSLeads);

			for(itrShadow = mShadows.begin(); itrShadow != mShadows.end(); ++itrShadow){
				if(*itrShadow != NULL && ((*itrShadow)->mMode == eSM_write || (*itrShadow)->mMode == eSM_all)){
					get() = (*itrShadow)->mData;
				}
			}
		}

		template<typename A>
		void
		tShadowPlug<A>::updateFinish(){
			typedef tPlugParent<A> p;

			PROFILE;

			dLock lock(guardShadows);

			for(itrShadow = mShadows.begin(); itrShadow != mShadows.end(); ++itrShadow){
				if(*itrShadow != NULL){
					(*itrShadow)->mData = get();
					(*itrShadow)->mMode = eSM_read;
				}
			}
		}

		template<typename A>
		void
		tShadowPlug<A>::readShadow(cBase_plug *pWriteTo, dConSig aCon){
			if(tPlugFlakes<A>::mType != pWriteTo->mType)
				return;

			setTmpShadowRef(aCon);

			if(tmpSRef->mMode == eSM_init){	//- may be a bad idea.
				dLock lock(guardShadows);
				tmpSRef->mData = get();
			}

			pWriteTo->copyFrom(&tmpSRef->mData);
		}

		template<typename A>
		void
		tShadowPlug<A>::writeShadow(const cBase_plug *pReadFrom, dConSig aCon){
			if(tPlugFlakes<A>::mType != pReadFrom->mType)
				return;

			setTmpShadowRef(aCon);
			pReadFrom->copyInto(&tmpSRef->mData);
			tmpSRef->mMode = eSM_write;
		}

		template<typename A>
		void
		tShadowPlug<A>::setTmpShadowRef(dConSig pSig){
			ASRT_TRUE(pSig != SL_NO_ENTRY, "lead has no current context.");

			size_t s = static_cast<dConSig>(pSig);
			if(s > mShadows.size() ){
				dLock lock(guardShadows);
				while(s > mShadows.size())
					mShadows.push_back( NULL );
			}

			tmpSRef = mShadows[pSig];

			if(tmpSRef==NULL){
				dLock lock(guardShadows);
				tmpSRef = new tShadow();
				tmpSRef->mMode = eSM_init;
			}
		}

#	endif

	//--------------------------------------

	template<typename A>
	tPlug<A>::tPlug():
		tPlugParent<A>(cBase_plug::getPlugType<A>())
	{}

	template<typename A>
	tPlug<A>::tPlug(const A& pA):
		tPlugParent<A>(cBase_plug::getPlugType<A>()),
		mD(pA)
	{}

	template<typename A>
	tPlug<A>::tPlug(const tPlug<A> &other) :
		tPlugParent<A>(other.mType),
		mD(other.mD)
	{}

	template<typename A>
	tPlug<A>::tPlug(const cBase_plug *other) :
		tPlugParent<A>(other->mType)
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
	tPlug<A>::operator== (const cBase_plug &pD) const {
		return (pD.mType == tPlugFlakes<A>::mType);
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
	const A&
	tPlug<A>::getConst() const{
		return mD;
	}

	template<typename A>
	void
	tPlug<A>::actualCopyInto(void* pContainer, cBase_plug::dPlugType pType) const{
		PROFILE;

		typename dMapCopiers::iterator itrCopiers = getPlugCopiers<A>()->find( pType );
		if(itrCopiers != getPlugCopiers<A>()->end()){
			itrCopiers->second(&mD, pContainer);
		}else{
			throw excep::cantCopy(typeid(A).name(), "unknown", __FILE__, __LINE__);
		}
	}

	template<typename A>
	void
	tPlug<A>::actualCopyFrom(const void* pContainer, cBase_plug::dPlugType pType){
		PROFILE;

		if(pType != tPlugParent<A>::mType)
			throw excep::cantCopy(typeid(A).name(), "unknown", __FILE__, __LINE__);

		mD = *reinterpret_cast<const A*>(pContainer);
	}
}

///////////////////////////////////////////////////////////////////////////////////
// Template specializations.
namespace gt{

	//!\brief	plug leads are illegal. Don't make much sense anyhow.
	template<>
	class tPlug<cLead>: public tPlugParent<cLead>{
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
