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
// Typedefs



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

		~cPlugTag()
		{}

	private:
		cPlugTag& operator = (const cPlugTag&){ return *this; }
	};

	//----------------------------------------------------------------------------------------------------------------
	//!\brief	The interface for the plug template below.
	class cBase_plug{
	public:
		PLUG_TYPE_ID mType;	//!< Must be public so the tPlug templates can use it.

		cBase_plug(PLUG_TYPE_ID pTI);
		cBase_plug(const cBase_plug& pCopy);
		virtual ~cBase_plug();

		template<typename T> T getCopy();

		template<typename T> T* getPtr();

		virtual	cBase_plug&	operator= (const cBase_plug &pD)
			{	DUMB_REF_ARG(pD); DONT_USE_THIS; return *this; }

		virtual	cBase_plug& operator= (const cBase_plug *pD)
			{	DUMB_REF_ARG(pD); DONT_USE_THIS; return *this; }

		template<typename T>	cBase_plug& operator= (const T &pT);

		template< template<typename> class plug, typename T>	cBase_plug& operator= (plug<T> &pT);

		//!\brief


	protected:

		void linkLead(cLead* pLead); //!< Leads must let the plug know that they are linked in.
		void unlinkLead(cLead* pLead); //!< When a plug is destroyed, it must let the lead know.

	friend class cLead;
	private:
		std::set<cLead*> mLeadsConnected;

	};

	//----------------------------------------------------------------------------------------------------------------
	//!\brief	A plug is a data container that a lead can connect too. The lead can then connect that data to another
	//!			object via the jack function, as well as automatically disconnect itself from its linked leads when it
	//!			dies. It is also designed for serialization using a byte buffer.
	//!\note	You don't have to use plugs for all your figments stuff.
	//!			Just for the things you want to save and reload or pass
	//!			through a lead to another object.
	template<typename A>
	class tPlug: public cBase_plug{
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

	template<typename T>
	cBase_plug&
	cBase_plug::operator= (const T &pT){

		if(mType != PLUG_TYPE_TO_ID(T))
			PLUG_CANT_COPY(T);

		dynamic_cast< tPlug<T>* >(this)->mD = pT;

		return *this;
	}

	template< template<typename> class plug, typename T>
	cBase_plug& 
	cBase_plug::operator= (plug<T> &pT){
		if(this != &pT){
			if(mType != pT.mType)
				PLUG_CANT_COPY_ID(pT.mType);

			dynamic_cast< tPlug<T>* >(this)->mD = pT.mD;
		}
		return *this;
	}


	//--------------------------------------
	template<typename A>
	tPlug<A>::tPlug():
		cBase_plug(typeid(A))
	{
	}

	template<typename A>
	tPlug<A>::tPlug(const A& pA):
		cBase_plug(typeid(A)), mD(pA)
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

		if( mType == pD->mType ){	// we can just cast
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
	template<>
	class tPlug<dStr>: public cBase_plug{
	public:
		dStr mD;

		tPlug(): cBase_plug(typeid(dStr)){
		}

		tPlug(dStr pA): cBase_plug(typeid(dStr)), mD(pA){
		}

		virtual ~tPlug(){}

		virtual cBase_plug& operator= (const cBase_plug &pD){
			NOTSELF(&pD);
			if( mType != pD.mType )
				PLUG_CANT_COPY_ID(pD.mType);

				mD = dynamic_cast< const tPlug<dStr>* >( &pD )->mD;

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
				mD.push_back( *pBuff->get(sizeof(size_t)+(idx * sizeof(dNatChar)) ) );
			}

			pBuff->trimHead(sizeof(size_t)+length);
		}

		void reset(){
			mD.clear();
		}
	};
}


#endif
