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
#include "opOnAny.hpp"

///////////////////////////////////////////////////////////////////////////////////
// Macros
#ifdef GT_THREADS
#	define PLUG_PARENT tShadowPlug
#else
#	define PLUG_PARENT tPlugFlakes
#endif

///////////////////////////////////////////////////////////////////////////////////
// Object types
namespace gt{


	//----------------------------------------------------------------------------------------------------------------
	//!\brief	Provides serialization as a healthy breakfast. Get it, cereal, haha haha, uuuuhhh.
	template<typename A>
	class tPlugFlakes: public tDataPlug<A>{
	public:
		//---
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

		//virtual void assignTo(void *pTo, dPlugType pType) const;
		//virtual void appendTo(void *pTo, dPlugType pType) const;

		virtual void linkLead(cLead* pLead);
		virtual void unlinkLead(cLead* pLead);
		virtual void updateStart();
		virtual void updateFinish();

		virtual	cBase_plug& operator= (const cBase_plug &pD) =0;
		virtual A& get() = 0;	//!< Shouldn't need to be locked because only the containing figment should have access to this, and jacking/working are already locked.
		virtual const A& getConst() const =0;

	protected:
		virtual void readShadow(cBase_plug *pWriteTo, dConSig pSig);
		virtual void writeShadow(const cBase_plug *pReadFrom, dConSig pSig);
		virtual void shadowAppends(cBase_plug *pWriteTo, dConSig pSig);
		virtual void appendShadow(cBase_plug *pReadFrom, dConSig pSig);

	friend class cLead;

	private:
		typedef boost::lock_guard<boost::recursive_mutex> dLock;

		struct tShadow{
			boost::recursive_mutex mGuard;
			eShadowMode mMode;
			A mData;
			dConSig mSig;
		};

		//!\brief	Used to manage access to an individual shadow.
		class tCheckout{
		public:
			tCheckout(tShadow *manage);
			tCheckout(const tCheckout &copyMe);
			~tCheckout();

			tShadow* operator-> ();
			tShadow& operator* ();

		protected:
			tShadow *mShadow;
		};

		typedef std::vector< tShadow* > dVecShadows;
		typedef typename dVecShadows::iterator itrShadow;

		boost::recursive_mutex mGuardShadows, mGuardData, mGuardLinks;
		dVecShadows mShadows;

		tCheckout getShadow(dConSig pSig);	//!< Expands the shadow list, if needed.
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

		//--- Polymorph
		virtual cBase_plug& operator= (const cBase_plug &pD);
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
				dLock a(mGuardShadows);
				dLock b(mGuardData);
				dLock c(mGuardLinks);

				//- We need to unplug first before destroying shadows.
				cBase_plug::dMapLeads test;//!!!
				for(
					cBase_plug::dMapLeads::iterator itr = p::mLeadsConnected.begin();
					itr != p::mLeadsConnected.end();
					++itr
				)
					itr->first->unplug(this);

				p::mLeadsConnected.clear();

				for(itrShadow itr = mShadows.begin(); itr != mShadows.end(); ++itr){
					delete (*itr);
				}

			}catch(...){
				WARN_S("Unknown error when destroying a plug");
			}
		}


		template<typename A>
		void
		tShadowPlug<A>::linkLead(cLead* pLead){
			PROFILE;

			dLock lock(mGuardLinks);
			cBase_plug::linkLead(pLead);
		}

		template<typename A>
		void
		tShadowPlug<A>::unlinkLead(cLead* pLead){
			PROFILE;

			dLock lock(mGuardLinks);
			cBase_plug::unlinkLead(pLead);
		}

		template<typename A>
		void
		tShadowPlug<A>::updateStart(){
			PROFILE;

			dLock lock(mGuardShadows);
			dLock lockData(mGuardData);

			for(itrShadow itr = mShadows.begin(); itr != mShadows.end(); ++itr){
				if(*itr != NULL){
					if(!gWorld.get()->activeContext( (*itr)->mSig) ){
						tShadow *delMe = NULL;
						{
							dLock lockShadow((*itr)->mGuard);
							delMe = *itr;
							*itr = NULL;
						}
						SAFEDEL(delMe);

					}else if((*itr)->mMode == eSM_write){
						dLock lockShadow((*itr)->mGuard);
						get() = (*itr)->mData;
					}
				}
			}
		}

		template<typename A>
		void
		tShadowPlug<A>::updateFinish(){
			PROFILE;

			dLock lockShadows(mGuardShadows);
			dLock lockData(mGuardData);

			for(itrShadow itr = mShadows.begin(); itr != mShadows.end(); ++itr){
				if(*itr != NULL){
					dLock lockShadow((*itr)->mGuard);
					(*itr)->mData = get();
					(*itr)->mMode = eSM_read;
				}
			}
		}

		template<typename A>
		void
		tShadowPlug<A>::readShadow(cBase_plug *pWriteTo, dConSig pSig){
			PROFILE;

			tCheckout shadow( getShadow(pSig) );

			if(shadow->mMode == eSM_init){
				dLock lock(mGuardData);
				shadow->mData = get();
			}

			tLitePlug<A> tmp(&shadow->mData);
			*pWriteTo = tmp;
		}

		template<typename A>
		void
		tShadowPlug<A>::writeShadow(const cBase_plug *pReadFrom, dConSig pSig){
			PROFILE;

			tCheckout shadow( getShadow(pSig) );

			pReadFrom->assignTo(
				&shadow->mData,
				cBase_plug::genPlugType<A>()
			);

			shadow->mMode = eSM_write;
		}

		template<typename A>
		void
		tShadowPlug<A>::appendShadow(cBase_plug *pReadFrom, dConSig pSig){
			PROFILE;

			tCheckout shadow( getShadow(pSig) );

			pReadFrom->appendTo(
				&shadow->mData,
				cBase_plug::genPlugType<A>()
			);

			shadow->mMode = eSM_write;
		}

		template<typename A>
		void
		tShadowPlug<A>::shadowAppends(cBase_plug *pWriteTo, dConSig pSig){
			PROFILE;

			tCheckout shadow( getShadow(pSig) );

			if(shadow->mMode == eSM_init){	//- This looks bad because it is possible that the unlocked interface is being used to change data at the point we are reading form it.
				dLock lock(mGuardData);
				shadow->mData = get();
			}

			tLitePlug<A> tmp(&shadow->mData);
			*pWriteTo += tmp;
		}


		template<typename A>
		typename tShadowPlug<A>::tCheckout
		tShadowPlug<A>::getShadow(dConSig pSig){
			dLock lockShadows(mGuardShadows);

			size_t s = static_cast<dConSig>(pSig);
			if(mShadows.size() <= s){
				while(mShadows.size() <= s)
					mShadows.push_back(NULL);
			}

			if(mShadows[pSig] == NULL){
				mShadows[pSig] = new tShadow();
				mShadows[pSig]->mMode = eSM_init;
			}

			return tCheckout( mShadows[pSig] );
		}


		//--------------------------------------
		template<typename A>
		tShadowPlug<A>::tCheckout::tCheckout(tShadow *manage){
			manage->mGuard.lock();
			mShadow = manage;
		}

		template<typename A>
		tShadowPlug<A>::tCheckout::tCheckout(const tCheckout &transfer)
		: mShadow(NULL)
		{
			if(transfer.mShadow != NULL){
				tCheckout &ref = const_cast<tCheckout&>(transfer);

				mShadow = ref.mShadow;
				ref.mShadow = NULL;
			}
		}

		template<typename A>
		tShadowPlug<A>::tCheckout::~tCheckout(){
			try{
				if(mShadow != NULL)
					mShadow->mGuard.unlock();

			}catch(std::exception &e){
				WARN_S(e.what());
			}catch(...){

			}
		}

		template<typename A>
		typename tShadowPlug<A>::tShadow*
		tShadowPlug<A>::tCheckout::operator-> (){
			return mShadow;
		}

		template<typename A>
		typename tShadowPlug<A>::tShadow&
		tShadowPlug<A>::tCheckout::operator* (){
			return *mShadow;
		}

#	endif

	//--------------------------------------
	template<typename A>
	tPlug<A>::tPlug(){
	}

	template<typename A>
	tPlug<A>::tPlug(const cBase_plug &other){
		other.assignTo(
			&mD,
			cBase_plug::genPlugType<A>()
		);
	}

	template<typename A>
	tPlug<A>::tPlug(const A& pA)
	: mD(pA)
	{}

	template<typename A>
	tPlug<A>::tPlug(const tPlug<A> &other)
	: mD(other.mD)
	{}

	template<typename A>
	tPlug<A>::tPlug(const cBase_plug *other){
		other->assignTo(
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
		pD.assignTo(&mD, cBase_plug::genPlugType<A>());
		return *this;
	}

	template<typename A>
	cBase_plug&
	tPlug<A>::operator+= (const cBase_plug &pD){
		NOTSELF(&pD);
		pD.appendTo(&mD, cBase_plug::genPlugType<A>());
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

	//----------------------------------------------------------------------------------------------------------------
	//!\brief	plug leads are illegal. Don't make much sense anyhow.
	template<>
	class tPlug<cLead>: public PLUG_PARENT<cLead>{
	};

}



#endif
