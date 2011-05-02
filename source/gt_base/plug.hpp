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

	//-------------------------------------------------------------------------------------
	//!\brief	helpful when loading.
	class cReload{
	public:
		ptrFig		fig;
		cByteBuffer	data;

		cReload(){}
		cReload(ptrFig pFig, const dByte* buff = NULL, size_t buffSize = 0): fig(pFig){
			if(buff != NULL && buffSize > 0)
				data.copy(buff, buffSize);
		}
		~cReload(){}
	};

	typedef std::map<dFigSaveSig, cReload*> dReloadMap;

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

		//!\brief	Converts data from the native system format, into a cross platform buffer to be saved later.
		virtual cByteBuffer& save()
			{ DONT_USE_THIS; cByteBuffer *temp = new cByteBuffer(); return *temp; }	
		
		//!\brief	Allows you to pass this plug a buffer for it try and load from.
		virtual void load(const cByteBuffer& pBuff)					
			{ DUMB_REF_ARG(pBuff); DONT_USE_THIS; }

		virtual					cBase_plug&	operator= (const cBase_plug &pD);
		template<typename T>	cBase_plug& operator= (const T &pT);

		template< template<typename> class plug, typename T>	cBase_plug& operator= (plug<T> &pT);

		template<typename T>	T getMDCopy(void);
		template<typename T>	T* getMDPtr(void);

	private:
		std::set<cLead*> mLeadsConnected;

	};

	//--------------------------------------------------------
	//!\brief	A plug is a data container that a lead can connect too.
	//			The lead can then connect that data to another object
	//			via the call function.
	template<typename A>
	class cPlug: public cBase_plug{
	public:
		A mD;

		cPlug();
		cPlug(const A& pA);
		virtual ~cPlug();

		virtual cBase_plug& operator= (const cBase_plug &pD);
		virtual void operator= (const A& pA);

		virtual cByteBuffer& save();
		virtual void loadEat(cByteBuffer* pChewToy, dReloadMap* pReloads);	//!< Eats the buffer you pass it in order to load. This way, memory is conserved.
	private:
		void genericCopy(const cBase_plug* pD);
	};
}

///////////////////////////////////////////////////////////////////////////////////
// Templates

namespace gt{

	//--------------------------------------
	
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
		genericCopy(&pD);
		return *this;
	}

	template<typename A>
	void
	cPlug<A>::operator= (const A& pA){
		mD = pA;
	}

	template<typename A>
	cByteBuffer&
	cPlug<A>::save(){
		PROFILE;

		//void* temp = &mD;	//stop compiler bitching.
		cByteBuffer* saveBuff = new cByteBuffer();
		saveBuff->copy(
			static_cast<dByte*>(
				reinterpret_cast<dByte*>(&mD)
			),
			sizeof(A)
		);
		return *saveBuff;
	}

	template<typename A>
	void
	cPlug<A>::loadEat(cByteBuffer* pChewToy, dReloadMap* pReloads){
		//!\todo
	}
}

///////////////////////////////////////////////////////////////////////////////////
// Template specialisations.
namespace gt{

	//--------------------------------------
	template<>
	class cPlug<ptrFig>: public cBase_plug{
	public:
		ptrFig mD;

		cPlug() : cBase_plug(typeid(ptrFig)), mD(gWorld->getEmptyFig()){
		}

		cPlug(boost::shared_ptr<cFigment> pA) : cBase_plug(typeid(ptrFig)), mD(pA){
		}

		virtual ~cPlug(){}

		virtual cBase_plug& operator= (const cBase_plug &pD){
			PROFILE;

			//!\todo figure out a way to prevent code duplication.
			if( mType == pD.mType ){	// we can just cast
				//cBase_plug* temp = const_cast<cBase_plug*>(&pD);
				mD = dynamic_cast< cPlug<ptrFig>* >(
					const_cast<cBase_plug*>(&pD)
				)->mD;
			}else{
				PLUG_CANT_COPY_ID(pD.mType);
			}

			return *this;
		}

		virtual void operator= (ptrFig pA){ mD = pA; }

		virtual cByteBuffer& save(){
			PROFILE;

			//- Using the pointer as a unique number to identify the referenced figment.
			cByteBuffer* saveBuff = new cByteBuffer();
			dFigSaveSig saveSig = static_cast<dFigSaveSig>(mD.get());
			saveBuff->add( (dByte*)(&saveSig), sizeof(dFigSaveSig) );
			return *saveBuff;
		}

		virtual void loadEat(cByteBuffer* pChewToy, dReloadMap* pReloads){
			PROFILE;

			if(pReloads != NULL){
				dFigSaveSig saveSig = 0;
				pChewToy->fill(&saveSig);
				dReloadMap::iterator itr = pReloads->find(saveSig);

				mD = itr->second->fig;
				pChewToy->trim(sizeof saveSig);
			}
		}
	};

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

			if( mType == pD.mType ){	// we can just cast
				// bein' bad
				mD = *const_cast<cBase_plug*>(&pD)->getMDPtr<dStr>();

			}else{
				PLUG_CANT_COPY_ID(pD.mType);
			}
			return *this;
		}

		virtual cByteBuffer& save(){
			
			cByteBuffer* saveBuff = new cByteBuffer();

			//- Need to make up the version that saves the portable buffer
			char* temp = new char[mD.size()+1];
			::memcpy( temp, mD.c_str(), mD.size() );
			temp[mD.size()] = '\0';
			saveBuff->take(static_cast<dByte*>(temp), (mD.size() +1) * sizeof(char));
			return *saveBuff;
		}

		virtual void loadEat(cByteBuffer* pBuff, dReloadMap* pReloads){
			//!\todo Avoid temp buffer.
			size_t length = pBuff->size()+1;
			dByte* temp = new dByte[length];

			pBuff->copy(temp, length-1);
			temp[length-1] = '\0';
			mD = (dNatChar*)temp;
			pBuff->trim(length-1);
			delete temp;
		}
	};
}


#endif
