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


///////////////////////////////////////////////////////////////////////////////////
//
namespace gt{

	//----------------------------------------------------------------------------------------------------------------
	//!\brief	Another step towards a full plug, designed just to manage assignments and appends.
	//!\note	Putting the assignments and append maps in here so that it's easier to specialise the tOpOnAny.
	template<typename A>
	class tDataPlug: public cBase_plug{
	public:
		~tDataPlug();

		//--- implemented
		virtual dPlugType getType() const;
		virtual void assignTo(void *pTo, dPlugType pType) const;
		virtual void appendTo(void *pTo, dPlugType pType) const;

		virtual bool operator== (const cBase_plug &pD) const;
		virtual cBase_plug& operator= (const cBase_plug &pD);
		virtual cBase_plug& operator+= (const cBase_plug &pD);

		//--- new interface
		virtual A& get() =0;
		virtual const A& getConst() const =0;

	private:
		tDataPlug<A>& operator= (const tDataPlug<A> &dontcare){ return *this; }
		tDataPlug<A>& operator+= (const tDataPlug<A> &dontcare){ return *this; }
	};


	//----------------------------------------------------------------------------------------------------------------
	//!\brief
	template<typename A>
	class tLitePlug: public cBase_plug{
	public:

		//--- implemented
		dPlugType getType() const;
		void assignTo(void *pTo, dPlugType pType) const;
		void appendTo(void *pTo, dPlugType pType) const;

		bool operator== (const cBase_plug &pD) const;
		cBase_plug& operator= (const cBase_plug &pD);
		cBase_plug& operator+= (const cBase_plug &pD);

		//--- new interface
		tLitePlug(A * pRef);
		~tLitePlug();

		//---
		void save(cByteBuffer* pSaveHere) { DONT_USE_THIS; }
		void loadEat(cByteBuffer* pChewToy, dReloadMap *aReloads = NULL) { DONT_USE_THIS; }

		A& get();
		const A& getConst() const;

	private:
		A * mRef;	//!< DON'T cleanup.
	};


	//----------------------------------------------------------------------------------------------------------------
	//!\brief	Provides serialization as a healthy breakfast. Get it, cereal, haha haha, uuuuhhh.
	template<typename A>
	class tPlugFlakes: public tDataPlug<A>{
	public:
		//---
		virtual A& get() = 0;
		virtual const A& getConst() const =0;

		//---
		virtual ~tPlugFlakes();
		virtual void save(cByteBuffer* pSaveHere);
		virtual void loadEat(cByteBuffer* pChewToy, dReloadMap *aReloads = NULL);
	};


	//----------------------------------------------------------------------------------------------------------------
	//!\brief	Implements most of what's left unimplemented from cBase_plug. It also provides easy access to its data
	//!			for any containing figments, which also makes it not thread safe if somehow the plug were to be used
	//!			through this interface by 2 threads. Which can't really happen because async leads use shadows.
	//!\note	Refer to the cBase_plug class for more info.
	template<typename A>
	class tPlug: public tPlugFlakes<A>{
	public:
		tPlug();
		tPlug(const cBase_plug &other);
		tPlug(const tPlug<A> &other);
		tPlug(const A &pA);
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

	//-----------------------------------------------------------------------------
	template<typename A>
	tDataPlug<A>::~tDataPlug(){
		PLUG_DATALOCK;
	}

	template<typename A>
	dPlugType
	tDataPlug<A>::getType() const {
		return genPlugType<A>();
	}

	template<typename A>
	void
	tDataPlug<A>::assignTo(void *pTo, dPlugType pType) const{
		PLUG_DATALOCK;
		cAnyOp::assign(getConst(), pTo, pType);
	}

	template<typename A>
	void
	tDataPlug<A>::appendTo(void *pTo, dPlugType pType) const{
		PLUG_DATALOCK;
		cAnyOp::append(getConst(), pTo, pType);
	}

	template<typename A>
	bool
	tDataPlug<A>::operator== (const cBase_plug &pD) const {
		return (genPlugType<A>() == pD.getType());
	}

	template<typename A>
	cBase_plug&
	tDataPlug<A>::operator= (const cBase_plug &pD){
		NOTSELF(&pD);
		PLUG_DATALOCK;
		pD.assignTo(&get(), genPlugType<A>());
		return *this;
	}

	template<typename A>
	cBase_plug&
	tDataPlug<A>::operator+= (const cBase_plug &pD){
		NOTSELF(&pD);
		PLUG_DATALOCK;
		pD.appendTo(&get(), genPlugType<A>());
		return *this;
	}

	//-----------------------------------------------------------------------------

	template<typename A>
	dPlugType
	tLitePlug<A>::getType() const{
		return genPlugType<A>();
	}

	template<typename A>
	void
	tLitePlug<A>::assignTo(void *pTo, dPlugType pType) const{
		cAnyOp::assign(*mRef, pTo, pType);
	}

	template<typename A>
	void
	tLitePlug<A>::appendTo(void *pTo, dPlugType pType) const{
		cAnyOp::assign(*mRef, pTo, pType);
	}

	template<typename A>
	bool
	tLitePlug<A>::operator== (const cBase_plug &pD) const{
		return (genPlugType<A>() == pD.getType());
	}

	template<typename A>
	cBase_plug&
	tLitePlug<A>::operator= (const cBase_plug &pD){
		NOTSELF(&pD);
		pD.assignTo(mRef, genPlugType<A>());
		return *this;
	}

	template<typename A>
	cBase_plug&
	tLitePlug<A>::operator+= (const cBase_plug &pD){
		NOTSELF(&pD);
		pD.appendTo(mRef, genPlugType<A>());
		return *this;
	}

	template<typename A>
	A&
	tLitePlug<A>::get(){
		return *mRef;
	}

	template<typename A>
	const A&
	tLitePlug<A>::getConst() const{
		return *mRef;
	}

	template<typename A>
	tLitePlug<A>::tLitePlug(A * pRef)
	: mRef(pRef)
	{}

	template<typename A>
	tLitePlug<A>::~tLitePlug()
	{}

	//-----------------------------------------------------------------------------
	template<typename A>
	tPlugFlakes<A>::~tPlugFlakes(){
	}

	template<typename A>
	void
	tPlugFlakes<A>::save(cByteBuffer* pSaveHere){
		PLUG_DATALOCK;
		pSaveHere->add(&get());
	}

	template<typename A>
	void
	tPlugFlakes<A>::loadEat(cByteBuffer* pChewToy, dReloadMap *aReloads){
		PLUG_DATALOCK;
		DUMB_REF_PAR(aReloads);
		pChewToy->trimHead( pChewToy->fill(&get()) );
	}

	//-----------------------------------------------------------------------------

	template<typename A>
	tPlug<A>::tPlug(){
	}

	template<typename A>
	tPlug<A>::tPlug(const cBase_plug &other){
		other.assignTo(
			&mD,
			genPlugType<A>()
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
			genPlugType<A>()
		);
	}

	template<typename A>
	tPlug<A>::~tPlug(){
	}

	template<typename A>
	cBase_plug&
	tPlug<A>::operator= (const cBase_plug &pD){
		NOTSELF(&pD);
		pD.assignTo(&mD, genPlugType<A>());
		return *this;
	}

	template<typename A>
	cBase_plug&
	tPlug<A>::operator+= (const cBase_plug &pD){
		NOTSELF(&pD);
		pD.appendTo(&mD, genPlugType<A>());
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
	class tPlug<cLead>: public tPlugFlakes<cLead>{
	};

}



#endif
