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


#ifdef GT_THREADS
#	define PLUG_PARENT tShadowPlug
#else
#	define PLUG_PARENT tPlugFlakes
#endif

///////////////////////////////////////////////////////////////////////////////////
// fu
namespace gt{

	namespace voidAssign{
		void textToNStr(const dText *pFrom, void *pTo);
		void textToPStr(const dText *pFrom, void *pTo);
		void plaCStrToPStr(const dPlaChar * const *pFrom, void *pTo);
		void plaCStrToNStr(const dPlaChar * const *pFrom, void *pTo);
		void plaCStrToText(const dPlaChar * const *pFrom, void *pTo);
	}

	namespace voidAppend{
		void textToText(const dText *pFrom, void *pTo);
		void plaCStrToPStr(const dPlaChar * const *pFrom, void *pTo);
		void plaCStrToNStr(const dPlaChar * const *pFrom, void *pTo);
		void plaCStrToText(const dPlaChar * const *pFrom, void *pTo);
	}
}

///////////////////////////////////////////////////////////////////////////////////
// Object types
namespace gt{

	//----------------------------------------------------------------------------------------------------------------
	//!\brief	Provides serialization as a healthy breakfast. Get it, cereal, haha haha, uuuuhhh.
	template<typename A>
	class tPlugFlakes: public tDataPlug<A>{
	public:

		//--- interface
		virtual A& get() = 0;
		virtual const A& getConst() const =0;

		//---
		virtual ~tPlugFlakes(){
		}

		virtual void save(cByteBuffer* pSaveHere){
			pSaveHere->add(&get());
		}

		virtual void loadEat(cByteBuffer* pChewToy, dReloadMap *aReloads = NULL){
			pChewToy->trimHead( pChewToy->fill(&get()) );
		}

	};


#	ifdef GT_THREADS

	//----------------------------------------------------------------------------------------------------------------
	//!\brief	Used to indicate how shadows are effects, and how the effect the source.
	enum eShadowMode{
		eSM_init,	//!< Initial value.
		eSM_read,	//!< The data has only be read from, or nothing has happened. So just update the shadow and nothing else.
		eSM_write,	//!< Data is written to the source.
	};

	//----------------------------------------------------------------------------------------------------------------
	//!\brief	The shadow plug allows you to manipulate a plug in a multi-threaded environment, while avoiding deadlocks.
	//!			It does this by using copies/shadows of its data per thread that tries to access it via the lead. When it runs,
	//!			it then updates these copies/shadows and the origin depending on what was done to the copy.
	template<typename A>
	class tShadowPlug: public tPlugFlakes<A>{
	public:
		tShadowPlug();
		virtual ~tShadowPlug();

		virtual void linkLead(cLead* pLead);
		virtual void unlinkLead(cLead* pLead);
		virtual void updateStart();
		virtual void updateFinish();

		virtual	cBase_plug& operator= (const cBase_plug &pD) =0;
		virtual bool operator== (const cBase_plug &pD) const =0;

		virtual A& get() = 0;	//!< Shouldn't need to be locked because only the containing figment should have access to this, and jacking/working are already locked.
		virtual const A& getConst() const =0;

	protected:
		virtual void readShadow(cBase_plug *pWriteTo, dConSig pSig);
		virtual void writeShadow(const cBase_plug *pReadFrom, dConSig pSig);
		virtual void appendShadow(const cBase_plug *pReadFrom, dConSig aSig);

	friend class cLead;

	private:
		struct tShadow{
			eShadowMode mMode;
			A mData;
			dConSig mSig;
		};

		typedef boost::lock_guard<boost::recursive_mutex> dLock;
		typedef std::vector< tShadow* > dVecShadow;

		boost::recursive_mutex guardShadows, guardLinks;
		dVecShadow mShadows;
		typename dVecShadow::iterator itrShadow;	//!< handy.
		tShadow *tmpSRef;	//!< handy.

		void setTmpShadowRef(dConSig pSig);	//!< Expands the shadow list, if needed, and sets tmpSRef to the shadow for this thread.
	};
#	endif

	//----------------------------------------------------------------------------------------------------------------
	//!\brief	Implements most of what's left unimplemented from cBase_plug. It also provides easy access to its data
	//!			for any containing figments, which also makes it not thread safe if somehow the plug were to be used
	//!			through this interface by 2 threads. Which can't really happen because async leads use shadows.
	//!\note	Refer to the cBase_plug class for more info.
	template<typename A>
	class tPlug: public PLUG_PARENT<A>{
	public:
		tPlug();
		tPlug(const cBase_plug &other);
		tPlug(const A& pA);
		tPlug(const tPlug<A> &other);
		tPlug(const cBase_plug *other);
		virtual ~tPlug();

		//- Polymorph
		virtual cBase_plug& operator= (const cBase_plug &pD);
		virtual bool operator== (const cBase_plug &pD) const;
		virtual cBase_plug& operator+= (const cBase_plug &pD);
		virtual A& get();
		virtual const A& getConst() const;

		//--- Unique
		cBase_plug& operator= (const tPlug<A> &other);
		cBase_plug& operator= (const A& pA);

	private:
		A mD;	//!< Data
	};

}


///////////////////////////////////////////////////////////////////////////////////
// Template implementation
namespace gt{

#	ifdef GT_THREADS

		//--------------------------------------
		template<typename A>
		tShadowPlug<A>::tShadowPlug(){
		}

		template<typename A>
		tShadowPlug<A>::~tShadowPlug(){
			typedef tPlugFlakes<A> p;
			try{

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

			dLock lock(guardLinks);
			cBase_plug::linkLead(pLead);
		}

		template<typename A>
		void
		tShadowPlug<A>::unlinkLead(cLead* pLead){
			PROFILE;

			dLock lock(guardLinks);
			cBase_plug::unlinkLead(pLead);
		}

		template<typename A>
		void
		tShadowPlug<A>::updateStart(){
			PROFILE;

			dLock lock(guardShadows);

			dRefWorld tmpW = gWorld.get();
			for(itrShadow = mShadows.begin(); itrShadow != mShadows.end(); ++itrShadow){
				tmpSRef = (*itrShadow);
				if(tmpSRef != NULL){
					if(!tmpW->activeContext(tmpSRef->mSig)){
						SAFEDEL(*itrShadow);
					}else if(tmpSRef->mMode == eSM_write){
						get() = tmpSRef->mData;
					}
				}
			}
		}

		template<typename A>
		void
		tShadowPlug<A>::updateFinish(){
			typedef PLUG_PARENT<A> p;

			PROFILE;

			dLock lock(guardShadows);

			for(itrShadow = mShadows.begin(); itrShadow != mShadows.end(); ++itrShadow){
				tmpSRef = (*itrShadow);
				if(tmpSRef != NULL){
					tmpSRef->mData = get();
					tmpSRef->mMode = eSM_read;
				}
			}
		}

		template<typename A>
		void
		tShadowPlug<A>::readShadow(cBase_plug *pWriteTo, dConSig pSig){
			setTmpShadowRef(pSig);
			if(tmpSRef->mMode == eSM_init){	//- This looks bad because it is possible that the unlocked interface is being used to change data at the point we are reading form it.
				dLock lock(guardShadows);
				tmpSRef->mData = get();
			}
			tLitePlug<A> tmp(&tmpSRef->mData);
			*pWriteTo = tmp;
		}

		template<typename A>
		void
		tShadowPlug<A>::writeShadow(const cBase_plug *pReadFrom, dConSig pSig){
			setTmpShadowRef(pSig);
			pReadFrom->assign(
				static_cast<void*>(&tmpSRef->mData),
				cBase_plug::genPlugType<A>()
			);
			tmpSRef->mMode = eSM_write;
		}

		template<typename A>
		void
		tShadowPlug<A>::appendShadow(const cBase_plug *pReadFrom, dConSig aSig){
			setTmpShadowRef(aSig);
			pReadFrom->append(
				static_cast<void*>(&tmpSRef->mData),
				cBase_plug::genPlugType<A>()
			);
			tmpSRef->mMode = eSM_write;
		}

		template<typename A>
		void
		tShadowPlug<A>::setTmpShadowRef(dConSig pSig){

			size_t s = static_cast<dConSig>(pSig);
			if(mShadows.size() <= s){
				dLock lock(guardShadows);
				while(mShadows.size() <= s)
					mShadows.push_back(NULL);
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
	tPlug<A>::tPlug(){
	}

	template<typename A>
	tPlug<A>::tPlug(const cBase_plug &other){
		other.assign(
			&mD,
			cBase_plug::genPlugType<A>()
		);
	}

	template<typename A>
	tPlug<A>::tPlug(const A& pA){
	}

	template<typename A>
	tPlug<A>::tPlug(const tPlug<A> &other){
	}

	template<typename A>
	tPlug<A>::tPlug(const cBase_plug *other){
		other->assign(
			&mD,
			cBase_plug::genPlugType<A>()
		);
	}

	template<typename A>
	tPlug<A>::~tPlug(){
	}

	template<typename A>
	cBase_plug&
	tPlug<A>::operator= (const cBase_plug &pD){
		NOTSELF(&pD);
		pD.assign(&mD, cBase_plug::genPlugType<A>());
		return *this;
	}

	template<typename A>
	cBase_plug&
	tPlug<A>::operator+= (const cBase_plug &pD){
		NOTSELF(&pD);
		pD.append(&mD, cBase_plug::genPlugType<A>());
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
	cBase_plug&
	tPlug<A>::operator= (const tPlug<A> &other){
		NOTSELF(&other);
		mD = other.mD;
		return *this;
	}

	template<typename A>
	cBase_plug&
	tPlug<A>::operator= (const A& pA){
		mD = pA;
		return *this;
	}
}

///////////////////////////////////////////////////////////////////////////////////
// Template specializations.
namespace gt{

	//!\brief	plug leads are illegal. Don't make much sense anyhow.
	template<>
	class tPlug<cLead>: public PLUG_PARENT<cLead>{
	};

	//-------------------------------------------------------------------------------------
	template<>
	tDataPlug<dText>::dMapAssigns *
	getVoidAssignments<dText>(){
		static bool setup = false;
		static tDataPlug<dText>::dMapAssigns copiers;

		if(!setup){
			copiers[ cBase_plug::genPlugType<dText>() ] = voidAssign::basic<dText>;
			copiers[ cBase_plug::genPlugType<dNatStr>() ] = voidAssign::textToNStr;
			copiers[ cBase_plug::genPlugType<dStr>() ] = voidAssign::textToPStr;
			setup=true;
		}

		return &copiers;
	}

	template<>
	tDataPlug<dText>::dMapAssigns *
	getVoidAppends<dText>(){
		static bool setup = false;
		static tDataPlug<dText>::dMapAppends app;

		if(!setup){
			app[ cBase_plug::genPlugType<dText>() ] = voidAppend::textToText;
			setup=true;
		}

		return &app;
	}

	//-------------------------------------------------------------------------------------
	template<>
	tDataPlug< const dPlaChar* >::dMapAssigns *
	getVoidAssignments< const dPlaChar* >(){
		static bool setup = false;
		static tDataPlug<const dPlaChar*>::dMapAssigns copiers;

		if(!setup){
			copiers[ cBase_plug::genPlugType<dText>() ] = voidAssign::plaCStrToText;
			copiers[ cBase_plug::genPlugType<dNatStr>() ] = voidAssign::plaCStrToNStr;
			copiers[ cBase_plug::genPlugType<dStr>() ] = voidAssign::plaCStrToPStr;
			setup=true;
		}

		return &copiers;
	}

	template<>
	tDataPlug< const dPlaChar* >::dMapAssigns *
	getVoidAppends< const dPlaChar* >(){
		static bool setup = false;
		static tDataPlug< const dPlaChar* >::dMapAppends app;

		if(!setup){
			app[ cBase_plug::genPlugType<dText>() ] = voidAppend::plaCStrToText;
			app[ cBase_plug::genPlugType<dNatStr>() ] = voidAppend::plaCStrToNStr;
			app[ cBase_plug::genPlugType<dStr>() ] = voidAppend::plaCStrToPStr;
			setup=true;
		}

		return &app;
	}

	//-------------------------------------------------------------------------------------

//	template<>
//	tDataPlug< boost::shared_ptr<cByteBuffer> >::dMapAssigns *
//	getVoidAssignments< boost::shared_ptr<cByteBuffer> >(){
//		static bool setup = false;
//		static tDataPlug< boost::shared_ptr<cByteBuffer> >::dMapAssigns ass;
//
//		if(!setup){
//			setup=true;
//		}
//
//		return &ass;
//	}

	template<>
	tDataPlug< boost::shared_ptr<cByteBuffer> >::dMapAppends *
	getVoidAppends< boost::shared_ptr<cByteBuffer> >(){
		static bool setup = false;
		static tDataPlug< boost::shared_ptr<cByteBuffer> >::dMapAppends app;

		if(!setup){
			setup=true;
		}

		return &app;
	}
}



#endif
