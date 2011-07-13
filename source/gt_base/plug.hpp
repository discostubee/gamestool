/*
 * !\file	plug.hpp
 * !\brief
 */

#ifndef PLUG_HPP
#define PLUG_HPP

#include "command.hpp"
#include "byteBuffer.hpp"


#define USE_TYPEINFO	//!< Enable or disable typeID as the method used to identify plug types.

#ifdef USE_TYPEINFO
	#include <typeinfo>
#endif

///////////////////////////////////////////////////////////////////////////////////
// Macros

#ifdef USE_TYPEINFO
	typedef const std::type_info & PLUG_TYPE_ID;
	#define PLUG_TYPE_TO_ID(t) typeid(t)
	#define PLUG_CANT_COPY(t) throw excep::cantCopy(mType.name(), typeid(t).name(), __FILE__, __LINE__)
	#define PLUG_CANT_COPY_ID(t) throw excep::cantCopy(mType.name(), t.name(), __FILE__, __LINE__)
#else
	typedef const gt::dNameHash PLUG_TYPE_ID;
	#define PLUG_TYPE_TO_ID(t) nameHash(typeid(t).name())
	#define PLUG_CANT_COPY(t) throw excep::cantCopy("", "", __FILE__, __LINE__)
	#define PLUG_CANT_COPY_ID(t) throw excep::cantCopy("", "", __FILE__, __LINE__)
#endif

///////////////////////////////////////////////////////////////////////////////////
// Classes
namespace gt{

	//--------------------------------------------------------
	//!\brief	this helps to identify what each plug is on a lead.
	class cPlugTag{
	public:
		typedef unsigned int dUID;	//!< Unique ID.

		const dUID	mID;
		const dStr	mName;

		cPlugTag(
			const dNatChar* pPlugName
		):
			mID( makeHash(pPlugName) ),
			mName( pPlugName )
		{}

		cPlugTag& operator=(const cPlugTag& pPlug){ 
			::memcpy((void*)(&mID), (void*)(&pPlug.mID), sizeof(mID));
			::memcpy((void*)(&mName), (void*)(&pPlug.mName), sizeof(mName));
			return *this; 
		}
		
	};

	//--------------------------------------------------------
	//!\brief	The base plug gives a consistent interface for the
	//			template class.
	class cBase_plug{
	public:
		PLUG_TYPE_ID mType;

		cBase_plug(PLUG_TYPE_ID pTI);
		cBase_plug(const cBase_plug& pCopy);
		virtual ~cBase_plug();

		void linkLead(cLead* pLead); //!< Leads must let the plug know that they are linked in.
		void unlinkLead(cLead* pLead); //!< When a plug is destroyed, it must let the lead know.

		//!\brief	Converts data from the native system format, into a cross platform buffer that can be loaded again later.
		//!\param	pAddHere	Appends save data to the end of this buffer.
		virtual void save(cByteBuffer *pAddHere)
			{ DUMB_REF_ARG(pAddHere); DONT_USE_THIS; }
		
		//!\brief	Allows you to pass this plug a buffer for it try and load from.
		//!\param	pChewToy	Eats the buffer you pass it in order to load. This way, memory is conserved. It is up to the byte
		//!						buffer to decide if it should use tricks to expend memory (by delaying the trim), rather than
		//!						take a performance hit. Keeping in minds that saving and loading is not meant to be a fast
		//!						process.
		//!\param	pReloads	This needs to be renamed to something like 'party' or something, because the reload map reflects only the figments
		//!\					that are visible to the reload process.
		virtual void loadEat(cByteBuffer* pChewToy, dReloadMap* pReloads)
			{ DUMB_REF_ARG(pChewToy); DUMB_REF_ARG(pReloads); DONT_USE_THIS; }

		virtual					cBase_plug&	operator= (const cBase_plug &pD);
		virtual					cBase_plug& operator= (const cBase_plug* pD);

		template<typename T>	cBase_plug& operator= (const T &pT);
		template< template<typename> class plug, typename T>	cBase_plug& operator= (plug<T> &pT);

		template<typename T>	T getMDCopy(void);
		template<typename T>	T* getMDPtr(void);

	private:
		std::set<cLead*> mLeadsConnected;

	};

	//--------------------------------------------------------
	//!\brief	A plug is a data container that a lead can connect too.
	//!			The lead can then connect that data to another object
	//!			via the call function, as well as automatically disconnect
	//!			itself from its linked leads when it dies.
	//!			It is also designed for serialization using a byte buffer.
	template<typename A>
	class cPlug: public cBase_plug{
	public:
		A mD;

		cPlug();
		cPlug(const A& pA);
		virtual ~cPlug();

		virtual void save(cByteBuffer* pAddHere);
		virtual void loadEat(cByteBuffer* pChewToy, dReloadMap* pReloads);
		virtual void reset();	//!< reset back to a default. Whatever that may be.

		virtual cBase_plug& operator= (const cBase_plug &pD);
		virtual cBase_plug& operator= (const cBase_plug* pD);
		virtual cBase_plug& operator= (const A& pA);
	private:
		void genericCopy(const cBase_plug* pD);
	};
}

///////////////////////////////////////////////////////////////////////////////////
// Templates
namespace gt{

	template<typename T>
	cBase_plug&
	cBase_plug::operator= (const T &pT){

		if(mType != PLUG_TYPE_TO_ID(T))
			PLUG_CANT_COPY(T);

		dynamic_cast< cPlug<T>* >(this)->mD = pT;

		return *this;
	}

	template< template<typename> class plug, typename T>
	cBase_plug& 
	cBase_plug::operator= (plug<T> &pT){
		
		if(mType != pT.mType)
			PLUG_CANT_COPY_ID(pT.mType);

		dynamic_cast< cPlug<T>* >(this)->mD = pT.mD;

		return *this;
	}

	template<typename T>
	T
	cBase_plug::getMDCopy(void){
		PROFILE;

		if(mType != PLUG_TYPE_TO_ID(T))
			PLUG_CANT_COPY(T);

		return dynamic_cast< cPlug<T>* >(this)->mD;
	}

	template<typename T>
	T*
	cBase_plug::getMDPtr(void){
		PROFILE;

		if(mType != PLUG_TYPE_TO_ID(T))
			PLUG_CANT_COPY(T);

		return &dynamic_cast< cPlug<T>* >(this)->mD;
	}


	//--------------------------------------
	template<typename A>
	cPlug<A>::cPlug():
		cBase_plug(typeid(A))
	{
	}

	template<typename A>
	cPlug<A>::cPlug(const A& pA):
		cBase_plug(typeid(A)), mD(pA)
	{
	}

	template<typename A>
	cPlug<A>::~cPlug(){
	}

	template<typename A>
	void 
	cPlug<A>::genericCopy(const cBase_plug* pD){
		PROFILE;

		ASRT_NOTNULL(pD);
		ASRT_NOTSELF(pD);

		if( mType == pD->mType ){	// we can just cast
			mD = dynamic_cast< cPlug<A>* >( const_cast<cBase_plug*>(pD) )->mD;
		}else{ // we need to see if there is an acceptable converter.
			//!!! can do this with a map of maps (2D map). Map this thing's template version of the copy check to the targets. Then at the right location, check if there is a templated copy function.
			//!!! for now we just fail.
			PLUG_CANT_COPY_ID(pD->mType);
		}
	}

	template<typename A>
	cBase_plug&
	cPlug<A>::operator= (const cBase_plug &pD){
		//NOTSELF(&pD);	// Performed in generic copy.
		genericCopy(&pD);
		return *this;
	}

	template<typename A>
	cBase_plug&
	cPlug<A>::operator= (const cBase_plug* pD){
		//NOTSELF(pD);	// Performed in generic copy.
		genericCopy(pD);
		return *this;
	}

	template<typename A>
	cBase_plug&
	cPlug<A>::operator= (const A& pA){
		mD = pA;
		return *this;
	}

	template<typename A>
	void
	cPlug<A>::save(cByteBuffer* pAddHere){
		PROFILE;
		pAddHere->add(&mD);
	}

	template<typename A>
	void
	cPlug<A>::loadEat(cByteBuffer* pChewToy, dReloadMap* pReloads){
		if(pChewToy->size() < sizeof(mD))
			throw cByteBuffer::excepUnderFlow(__FILE__, __LINE__);

		::memcpy(&mD, pChewToy->get(), sizeof(mD));
		pChewToy->trimHead(sizeof(mD));
	}

	template<typename A>
	void
	cPlug<A>::reset(){}
}

///////////////////////////////////////////////////////////////////////////////////
// Template specialisations.
namespace gt{

	//--------------------------------------
	template<>
	class cPlug<dStr>: public cBase_plug{
	public:
		dStr mD;

		cPlug(): cBase_plug(typeid(dStr)){
		}

		cPlug(dStr pA): cBase_plug(typeid(dStr)), mD(pA){
		}

		virtual ~cPlug(){}

		virtual void operator= (dStr pA){ mD = pA; }

		virtual cBase_plug& operator= (const cBase_plug &pD){
			NOTSELF(&pD);
			if( mType == pD.mType ){	// we can just cast
				mD = *(const_cast<cBase_plug*>(&pD)->getMDPtr<dStr>());
			}else{
				PLUG_CANT_COPY_ID(pD.mType);
			}
			return *this;
		}

		virtual cBase_plug& operator= (const cBase_plug* pD){
			NOTSELF(pD);
			if( mType == pD->mType ){	// we can just cast
				mD = *(const_cast<cBase_plug*>(pD)->getMDPtr<dStr>());
			}else{
				PLUG_CANT_COPY_ID(pD->mType);
			}
			return *this;
		}

		virtual void save(cByteBuffer* pAddHere){
			//!\todo	Avoid temporary buffer.
			const size_t length = mD.size();
			const size_t totalSize = length+sizeof(size_t);	// each character should only be 1 byte in size.
			dByte* temp = new dByte[totalSize];

			::memcpy( temp, &length, sizeof(size_t) );
			for(size_t idx=0; idx < length; ++idx){
				//!\todo make a more sophisticated conversion.
				temp[idx+sizeof(size_t)] = mD.at(idx);
			}

			try{
				pAddHere->add(temp, totalSize);
			}catch(...){
				delete [] temp; throw;
			}
			delete [] temp;
		}

		virtual void loadEat(cByteBuffer* pBuff, dReloadMap* pReloads){

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
				mD.push_back( *pBuff->get(sizeof(size_t)+idx) );
			}

			pBuff->trimHead(sizeof(size_t)+length);
		}

		virtual void reset(){
			mD.clear();
		}
	};
}


#endif