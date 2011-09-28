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
	template<typename A>
	class tPlugShadows: public cBase_plug{
	public:

		tPlugShadows(PLUG_TYPE_ID pTI);
		virtual ~tPlugShadows();

		virtual cBase_plug* getShadow(dConSig aCon, eShadowMode whatFor);

		virtual cUpdateLemming update();

		void linkLead(cLead* pLead);

		void unlinkLead(cLead* pLead);

	protected:
		virtual A& getMD() =0;	//!< This is so the update can grab out tPlug data.

		virtual void finishUpdate();

	private:
		#ifdef GT_THREADS
			typedef boost::unique_lock<boost::mutex> dMuLock;
			boost::mutex muMap;
		#endif

		typedef std::vector<tShadow<A> > dVecShadow;

		std::set<cLead*> mLeadsConnected;	//!< Lead connections are not copied.
		dVecShadow mShadows;
		std::set<cLead*>::iterator itrLead;	//!< handy.
		typename dVecShadow::iterator itrShadow;	//!< handy.
	};

	//----------------------------------------------------------------------------------------------------------------
	//!\brief	A plug is a data container that a lead can connect too. The lead can then connect that data to another
	//!			object via the jack function, as well as automatically disconnect itself from its linked leads when it
	//!			dies. It is also designed for serialization using a byte buffer.
	//!\note	You don't have to use plugs for all your figments stuff.  Just for the things you want to save and
	//!			reload or pass through a lead to another object.
	template<typename A>
	class tPlug: public tPlugShadows<A>{
	public:
		A mD;	//!< Data: This is public so that the owner can have easy access to it.

		tPlug();
		tPlug(const A& pA);
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
		void loadEat(cByteBuffer* pChewToy, dReloadMap* pReloads);

		//!\brief	reset back to a default. Whatever that may be.
		void reset();

		virtual cBase_plug& operator= (const cBase_plug &pD);

		virtual cBase_plug& operator= (const cBase_plug *pD);

		cBase_plug& operator= (const A& pA);

	protected:
		virtual A& getMD() { return mD; }

	private:
		void genericCopy(const cBase_plug* pD);
	};

}

///////////////////////////////////////////////////////////////////////////////////
// Templates
namespace gt{

	template<typename T>
	T
	cBase_plug::getCopy(){
		if(mType != PLUG_TYPE_TO_ID(T))
			PLUG_CANT_COPY(T);

		return dynamic_cast< tPlug<T>* >(this)->mD;
	}

	template<typename T>
	T*
	cBase_plug::getPtr(){
		if(mType != PLUG_TYPE_TO_ID(T))
			PLUG_CANT_COPY(T);

		return &dynamic_cast< tPlug<T>* >(this)->mD;
	}
/*
	template<typename T>
	cBase_plug&
	cBase_plug::operator= (const T &pT){

		if(mType != PLUG_TYPE_TO_ID(T))
			PLUG_CANT_COPY(T);

		dynamic_cast< tPlug<T>* >(this)->mD = pT;

		return *this;
	}
*/
	template< template<typename> class plug, typename T>
	cBase_plug& 
	cBase_plug::operator= (const plug<T> &pT){
		if(this != &pT){
			if(mType != pT.mType)
				PLUG_CANT_COPY_ID(pT.mType);

			dynamic_cast< tPlug<T>* >(this)->mD = pT.mD;
		}
		return *this;
	}

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
			DBUG_VERBOSE_LO("destroying plug");

			while( !mLeadsConnected.empty() ){
					itrLead = mLeadsConnected.begin();
					try{
						(*itrLead)->unplug(this);
					}catch(...){
						//- carry on removing.
					}
					mLeadsConnected.erase(itrLead);

					DBUG_VERBOSE_LO( "disconnected lead " << reinterpret_cast<unsigned int>(*mLeadsConnected.begin()) );
			}

			for(itrShadow = mShadows.begin(); itrShadow != mShadows.end(); ++itrShadow){
				if(itrShadow->mData != NULL)
					delete itrShadow->mData;
			}

		}catch(...){
			excep::base_error e("Unknown error when destroying a plug", __FILE__, __LINE__);
			WARN(e);
		}
	}

	template<typename A>
	cBase_plug*
	tPlugShadows<A>::getShadow(dConSig aCon, eShadowMode whatFor){
		//- It should be fine to not lock here because this data should be independant of the other threads, and the shadow list should only be updated when all leads are locked.
		if(mShadows[aCon].mMode == eSM_read)	//- these operations should be quick as its a vector.
			mShadows[aCon].mMode = whatFor;
		return mShadows[aCon].mData;
	}

	template<typename A>
	cUpdateLemming
	tPlugShadows<A>::update(){
		#ifdef GT_THREADS
			muMap.lock();	//- lemming unlocks this when it dies and call finish.
		#endif

		PROFILE;

		//- let any mutex throws go all the way and cause the program to exit.
		for(itrLead = mLeadsConnected.begin(); itrLead != mLeadsConnected.end(); ++itrLead)
			(*itrLead)->muLead.lock();

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
			(*itrLead)->muLead.unlock();

		#ifdef GT_THREADS
			muMap.unlock();
		#endif
	}

	template<typename A>
	void
	tPlugShadows<A>::linkLead(cLead* pLead){
		#ifdef GT_THREADS
			dMuLock lock(muMap);
		#endif

		PROFILE;
		ASRT_NOTNULL(pLead);

		DBUG_VERBOSE_LO("Connecting lead " << reinterpret_cast<unsigned int>(pLead) );
		mLeadsConnected.insert(pLead);

		if(mShadows.size() < pLead->mConx){
			tShadow<A> stamp = { eSM_read, NULL };
			mShadows.resize(pLead->mConx, stamp);
		}

		itrShadow = mShadows.begin() + pLead->mConx;


	}

	template<typename A>
	void
	tPlugShadows<A>::unlinkLead(cLead* pLead){
		#ifdef GT_THREADS
			dMuLock lock(muMap);
		#endif

		PROFILE;
		ASRT_NOTNULL(pLead);

		std::set<cLead*>::iterator tempI = mLeadsConnected.find(pLead);
		if( tempI != mLeadsConnected.end() ){	// there should be no need for this check.
			mLeadsConnected.erase( tempI );


			DBUG_VERBOSE_LO( "disconnecting lead " << reinterpret_cast<unsigned int>(pLead) );
		}
	}


	//--------------------------------------
	template<typename A>
	tPlug<A>::tPlug():
		tPlugShadows<A>(typeid(A))
	{
	}

	template<typename A>
	tPlug<A>::tPlug(const A& pA):
		tPlugShadows<A>(typeid(A)), mD(pA)
	{
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
			PLUG_CANT_COPY_ID(pD->mType);
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
	tPlug<A>::operator= (const A& pA){
		mD = pA;
		return *this;
	}

	template<typename A>
	void
	tPlug<A>::save(cByteBuffer* pAddHere){
		PROFILE;
		pAddHere->add(&mD);
	}

	template<typename A>
	void
	tPlug<A>::loadEat(cByteBuffer* pChewToy, dReloadMap* pReloads){
		DUMB_REF_ARG(pReloads);
		if(pChewToy->size() < sizeof(mD))
			throw cByteBuffer::excepUnderFlow(__FILE__, __LINE__);

		::memcpy(&mD, pChewToy->get(), sizeof(mD));
		pChewToy->trimHead(sizeof(mD));
	}

	template<typename A>
	void
	tPlug<A>::reset(){}
}

///////////////////////////////////////////////////////////////////////////////////
// Template specialisations.
namespace gt{

	//--------------------------------------
	//!\brief	Your program will store strings in whatever format it was built with. However, it will have and load using UTF16.
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
			if( mType != pD.mType )
				PLUG_CANT_COPY_ID(pD.mType);

				mD = dynamic_cast< const tPlug<std::string>* >( &pD )->mD;

			return *this;
		}

		virtual cBase_plug& operator= (const cBase_plug* pD){
			NOTSELF(pD);
			if( mType != pD->mType )
				PLUG_CANT_COPY_ID(pD->mType);

				mD = *(const_cast<cBase_plug*>(pD)->getPtr<dStr>());

			return *this;
		}

		void operator= (dStr pA){ mD = pA; }

		void save(cByteBuffer* pAddHere){
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

		void loadEat(cByteBuffer* pBuff, dReloadMap* pReloads){
			DUMB_REF_ARG(pReloads);

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
		virtual std::string& getMD() { return mD; }
	};

}


#endif
