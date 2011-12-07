/*
 * !\file	plug.hpp
 * !\brief
 */

#ifndef PLUG_HPP
#define PLUG_HPP

#include "lead.hpp"
#include <vector>

///////////////////////////////////////////////////////////////////////////////////
// Macros
#ifdef GT_THREADS
	#define PLUGUP(plug) cUpdateLemming lem__LINE__ = plug.update()
#else
	#define PLUGUP(plug) (void)plug
#endif

///////////////////////////////////////////////////////////////////////////////////
// Object types
namespace gt{

	template<typename A> class tPlug;


	//----------------------------------------------------------------------------------------------------------------
	template<typename A>
	struct tShadow{

		eShadowMode mMode;
		tPlug<A>* mData;	//!< If the shadow isn't used, this is null.
	};

	//----------------------------------------------------------------------------------------------------------------
	//!\brief	Plug shadows is a bad name because it also handles lead connections.
	//!\todo	make it smart enough to not bother with shadows unless it has more than one context connected.
	template<typename A>
	class tPlugShadows: public cBase_plug{
	public:

		tPlugShadows(PLUG_TYPE_ID pTI);
		virtual ~tPlugShadows();

		#ifdef GT_THREADS
			virtual cBase_plug* getShadow(dConSig aCon, eShadowMode whatFor);
			virtual cUpdateLemming update();
		#endif

		void linkLead(cLead* pLead);
		void unlinkLead(cLead* pLead);


	protected:
		typedef std::map<cLead*, unsigned int> dMapLeads;

		dMapLeads mLeadsConnected;		//!< Lead connections are not copied when copy plug values.
		dMapLeads::iterator itrLead;	//!< handy.

		#ifdef GT_THREADS
			typedef boost::lock_guard<boost::recursive_mutex> dMuLock;
			typedef std::vector<tShadow<A> > dVecShadow;

			boost::recursive_mutex muMap;

			dVecShadow mShadows;
			typename dVecShadow::iterator itrShadow;	//!< handy.

			virtual A& getMD() =0;	//!< This is so the update can grab out tPlug data.

			virtual void finishUpdate();
		#endif

	private:
		tPlugShadows& operator=(const tPlugShadows<A> &other){ return *this; }
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

		//!\brief
		void save(cByteBuffer* pAddHere);

		//!\brief	Allows you to pass this plug a buffer for it try and load from.
		//!\param	pChewToy	Eats the buffer you pass it in order to load. This way, memory is conserved. It is up to the byte
		//!						buffer to decide if it should use tricks to expend memory (by delaying the trim), rather than
		//!						take a performance hit. Keeping in minds that saving and loading is not meant to be a fast
		//!						process.
		//!\param	pReloads	This needs to be renamed to something like 'party' or something, because the reload map reflects only the figments
		//!\					that are visible to the reload process.
		void loadEat(cByteBuffer* pChewToy, dReloadMap *aReloads);

		//!\brief	reset back to a default. Whatever that may be.
		void reset();

		virtual cBase_plug& operator= (const cBase_plug &pD);

		virtual cBase_plug& operator= (const cBase_plug *pD);

		cBase_plug& operator= (const tPlug<A> &other);

		cBase_plug& operator= (const A& pA);

		virtual bool operator == (const cBase_plug &pD) const;

	protected:

		#ifdef GT_THREADS
			virtual A& getMD() { return mD; }
		#endif

	private:
		void genericCopy(const cBase_plug* pD);
	};
}

///////////////////////////////////////////////////////////////////////////////////
// Templates
namespace gt{

	//--------------------------------------
	template<typename A>
	tPlugShadows<A>::tPlugShadows(PLUG_TYPE_ID pTI) : cBase_plug(pTI){
	}

	template<typename A>
	tPlugShadows<A>::~tPlugShadows(){
		try{
			#ifdef GT_THREADS
				dMuLock lock(muMap);
			#endif
			PROFILE;

			for(dMapLeads::iterator itrLead = mLeadsConnected.begin(); itrLead != mLeadsConnected.end(); ++itrLead){
				try{
					itrLead->first->unplug(this);	//- the count is irrelevant.
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

			for(itrLead = mLeadsConnected.begin(); itrLead != mLeadsConnected.end(); ++itrLead)
				itrLead->first->muLead.lock();	//- let any mutex throws go all the way and cause the program to exit.

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

			for(itrLead = mLeadsConnected.begin(); itrLead != mLeadsConnected.end(); ++itrLead)
				itrLead->first->muLead.unlock();

			muMap.unlock();
		}
	#endif

	template<typename A>
	void
	tPlugShadows<A>::linkLead(cLead* pLead){
		#ifdef GT_THREADS
			dMuLock lock(muMap);
		#endif

		PROFILE;
		ASRT_NOTNULL(pLead);

		itrLead = mLeadsConnected.find(pLead);
		if(itrLead==mLeadsConnected.end()){
			mLeadsConnected[pLead] = 1;
		}else{
			++itrLead->second;
		}

		#ifdef GT_THREADS
			if(mShadows.size() < pLead->mConx +1){
				tShadow<A> stamp = { eSM_read, NULL };
				mShadows.resize(pLead->mConx +1, stamp);
			}

			itrShadow = mShadows.begin() + pLead->mConx;
			itrShadow->mData = new tPlug<A>();
			itrShadow->mData->mD = getMD();
		#endif
	}

	template<typename A>
	void
	tPlugShadows<A>::unlinkLead(cLead* pLead){
		#ifdef GT_THREADS
			dMuLock lock(muMap);
		#endif

		PROFILE;
		ASRT_NOTNULL(pLead);

		itrLead = mLeadsConnected.find(pLead);
		if(itrLead != mLeadsConnected.end()){
			--itrLead->second;
			if(itrLead->second == 0){
				mLeadsConnected.erase(itrLead);
			}

		}else{
			WARN("lead isn't connected to this plug.");
		}
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
		genericCopy(other);
	}

	template<typename A>
	tPlug<A>::~tPlug(){
	}

	template<typename A>
	void 
	tPlug<A>::genericCopy(const cBase_plug* pD){
		PROFILE;

		ASRT_NOTNULL(pD);
		ASRT_NOTSELF(pD);

		if( tPlugShadows<A>::mType == pD->mType ){	// we can just cast
			mD = dynamic_cast< tPlug<A>* >( const_cast<cBase_plug*>(pD) )->mD;
		}else{ // we need to see if there is an acceptable converter.
			//!!! can do this with a map of maps (2D map). Map this thing's template version of the copy check to the targets. Then at the right location, check if there is a templated copy function.
			//!!! for now we just fail.
			PLUG_CANT_COPY(typeid(A), pD->mType);
		}
	}

	template<typename A>
	cBase_plug&
	tPlug<A>::operator= (const cBase_plug &pD){
		//NOTSELF(&pD);	// Performed in generic copy.
		genericCopy(&pD);
		return *this;
	}

	template<typename A>
	cBase_plug&
	tPlug<A>::operator= (const cBase_plug* pD){
		//NOTSELF(pD);	// Performed in generic copy.
		genericCopy(pD);
		return *this;
	}

	template<typename A>
	cBase_plug&
	tPlug<A>::operator= (const tPlug<A> &other){
		if(this != &other){
			mD = other.mD;
		}
		return *this;
	}

	template<typename A>
	cBase_plug&
	tPlug<A>::operator= (const A& pA){
		mD = pA;
		return *this;
	}

	template<typename A>
	bool
	tPlug<A>::operator == (const cBase_plug &pD) const{
		if( tPlugShadows<A>::mType == pD.mType ){		// we can just cast
			return mD == dynamic_cast< tPlug<A>* >( const_cast<cBase_plug*>(&pD) )->mD;
		}

		throw excep::base_error("Can't compare", __FILE__, __LINE__);
	}

	template<typename A>
	void
	tPlug<A>::save(cByteBuffer* pAddHere){
		pAddHere->add(&mD);
	}

	template<typename A>
	void
	tPlug<A>::loadEat(cByteBuffer* pChewToy, dReloadMap *aReloads){
		DUMB_REF_ARG(aReloads);

		pChewToy->fill(&mD);
		pChewToy->trimHead(sizeof(mD));
	}

	template<typename A>
	void
	tPlug<A>::reset(){}

}

///////////////////////////////////////////////////////////////////////////////////
// Template specializations.
namespace gt{

	//------------------------------------------------------------------------------------------------------------------
	//!\brief	Your program will store strings in whatever format it was built with. However, it will have to save and
	//!			load using UTF16.
	template<>
	class tPlug<std::string>: public tPlugShadows<std::string>{
	public:
		std::string mD;

		tPlug(): tPlugShadows<std::string>(typeid(dStr)){
		}

		tPlug(std::string pA): tPlugShadows<std::string>(typeid(dStr)), mD(pA){
		}

		virtual ~tPlug(){}

		virtual cBase_plug& operator= (const cBase_plug &pD){
			NOTSELF(&pD);

			pD.copyInto(&mD);

			return *this;
		}

		virtual cBase_plug& operator= (const cBase_plug* pD){
			NOTSELF(pD);

			pD->copyInto(&mD);

			return *this;
		}

		void operator= (const std::string &pA){
			mD = pA;
		}

		bool operator == (const cBase_plug &pD) const{
			if( tPlugShadows<std::string>::mType == pD.mType ){	// we can just cast
				return mD.compare(
						//dynamic_cast< tPlug<std::string>* >( const_cast<cBase_plug*>(&pD) )->mD
						dynamic_cast< const tPlug<std::string>* >( &pD )->mD
				) == 0;
			}

			throw excep::base_error("Can't compare", __FILE__, __LINE__);
		}

		void save(cByteBuffer* pAddHere){
			//!\todo Save as UTF16.

			//!\todo	Avoid temporary buffer.
			const size_t length = mD.size();
			const size_t totalSize = length+sizeof(size_t);	// each character should only be 1 byte in size.
			dByte* temp = new dByte[totalSize];

			try{
				::memcpy( temp, &length, sizeof(size_t) );
				for(size_t idx=0; idx < length; ++idx){
					//!\todo make a more sophisticated conversion.
					temp[idx+sizeof(size_t)] = mD.at(idx);
				}

				pAddHere->add(temp, totalSize);
			}catch(...){
				delete [] temp; throw;
			}
			delete [] temp;
		}

		void loadEat(cByteBuffer* pBuff, dReloadMap *aReloads){
			//!\todo Load as UTF16.

			DUMB_REF_ARG(aReloads);

			if(pBuff->size() < sizeof(size_t))
				throw cByteBuffer::excepUnderFlow(__FILE__, __LINE__);

			size_t length = *(reinterpret_cast<const size_t*>(pBuff->get()));	// Length in 8 bit ascii.

			if(length==0)
				return;

			if(pBuff->size() < sizeof(size_t) + length )
				throw cByteBuffer::excepUnderFlow(__FILE__, __LINE__);

			mD.clear();
			mD.reserve(length);

			for(size_t idx=0; idx< length; ++idx){
				//!\todo make more sophisticated conversion.
				mD.push_back( *pBuff->get(sizeof(size_t)+(idx * sizeof(char)) ) );
			}

			pBuff->trimHead(sizeof(size_t)+length);
		}

		void reset(){
			mD.clear();
		}

	protected:
		virtual std::string& getMD() {
			return mD;
		}
	};


	//------------------------------------------------------------------------------------------------------------------
	//!\brief	plug leads are illegal. Don't make much sense anyhow.
	template<>
	class tPlug<cLead>: public tPlugShadows<cLead>{
		//--- I AM BANNED COZ I BREAK THINGS ---///
	};
}


#endif
