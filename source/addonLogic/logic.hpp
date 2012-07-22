/*
 * !\file	logic.hpp
 * !\brief	Each block of logic uses the context to see the process before it.
 * 			Use run lists if you want to branch the logic to basically do a 'for each' loop
 *
 *		For each lead assigned, each plug in the lead is operated on like this:
 *		Bools convert to true if > 0, or is false if == 0. Bools can be appended to string as 'true' or 'false'.
 *		Strings can only append, but can append numbers as well as other strings.
 *		Figments do nothing.
 *		Numbers of any kind are used like normal.
 *
 *  Copyright (C) 2010  Stuart Bridgens
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef LOGIC_HPP
#define LOGIC_HPP

#include <gt_base/figment.hpp>

namespace gt{

	//-------------------------------------------------------------------------------------
	struct sLeadConnect{
		typedef std::vector<const cPlugTag*> dVecPlugTag;

		ptrLead	mLead;
		dVecPlugTag mPlugsToUse;	//!< What plugs on the lead do we use?
	};

	//-------------------------------------------------------------------------------------
	// Inputs are leads that reference that object's data.
	class cLogicInput: public cFigment, private tOutline<cLogicInput>{
	protected:

		cPlug<ptrFig> mNext; //!< Points to the next logic block.

	public:
		std::vector<sLeadConnect> mInputs;	//!<

		enum eJackCom{
			eAddInput = cFigment::eSwitchEnd +1,
			eSwitchEnd,
		};

		static const cPlugTag*	xPT_inputLead;	//!< \note idx=0
		static const cPlugTag*	xPT_inputPlug;	//!< \note idx=1
		static const cCommand*	xAddInputs;		

		//-----------------------------
		static const dNatChar* identify(){ return "Logic input"; }

		//-----------------------------
		cLogicInput();
		virtual ~cLogicInput();

		//-----------------------------
		virtual const dNatChar* name() const{ return identify(); }		//!< Virtual version of identify.
		virtual dNameHash hash() const{ return tOutline<cLogicInput>::hash(); }		//!< Virtual version of get hash.

		//-----------------------------
		virtual void run(cContext* pCon);	//!<
		virtual void jack(ptrLead pLead);	//!<
	};

	//-------------------------------------------------------------------------------------
	// Takes the previous artithmetic or bool logic, and sends it to all the outputs.
	class cLogicOutput: public cFigment, private tOutline<cLogicOutput>{
	protected:

		std::vector<sLeadConnect> mOutputs;	//!< Stores where to send the ouput from previous logic.

	private:
		dNameHash mStackHash;
		std::vector<sLeadConnect>::iterator scrOutItr;
		sLeadConnect::dVecPlugTag::iterator scrPlugItr;
		dProgramStack::reverse_iterator mStackItr;

	public:

		//-----------------------------
		static const dNatChar* identify(){ return "Logic output"; }

		//-----------------------------
		cLogicOutput();
		virtual ~cLogicOutput();

		//-----------------------------
		virtual const dNatChar* name() const{ return identify(); }		//!< Virtual version of identify.
		virtual dNameHash hash() const{ return tOutline<cLogicOutput>::hash(); }		//!< Virtual version of get hash.

		//-----------------------------
		virtual void run(cContext* pCon);	//!<

	};

	//-------------------------------------------------------------------------------------
	// Performs a single type of arithmetic on the previous input, bool or other arithmetic
	// logic figments All input is converted to the float type.
	class cArithmetic: public cFigment, private tOutline<cArithmetic>{

	public:
		enum eOpType{
			eSummation,
			eProduct,
		};

		//-----------------------------
		static const dNatChar* identify(){ return "arithmetic"; }

		//-----------------------------
		cArithmetic();
		virtual ~cArithmetic();

		//-----------------------------
		virtual const dNatChar* name() const{ return identify(); }		//!< Virtual version of identify.
		virtual dNameHash hash() const{ return tOutline<cArithmetic>::hash(); }		//!< Virtual version of get hash.

		//-----------------------------
		//!\brief	run to change state and go to the next logic block.
		//!\note	Looks for logic inputs, arithmetic logic other bools
		virtual void run(cContext* pCon);

	protected:
		cPlug<eOpType>	mOp;
		cPlug<float>	mResult;
		cPlug<ptrFig>	mNext; //!< Points to the next figment.
	};

	//-------------------------------------------------------------------------------------
	// Applies the boolean operator to ever input.
	// Strings are true if they contain anything.
	class cBoolLogic: public cFigment, private tOutline<cBoolLogic>{

	public:
		//-----------------------------
		enum eBoolOp{
			eOR,	// True if any input is true.
			eAND,	// True is all inputs are true.
			eXOR,	// True if only 1 input is true.
			eNAND	// True is no inputs are true.
		};

		cPlug<bool>	  	mState;	//!< This state changes depending on the context during a run.

		//-----------------------------
		static const dNatChar* identify(){ return "Bool logic"; }

		//-----------------------------
		cBoolLogic();
		virtual ~cBoolLogic();

		//-----------------------------
		virtual const dNatChar* name() const{ return identify(); }		//!< Virtual version of identify.
		virtual dNameHash hash() const{ return tOutline<cBoolLogic>::hash(); }		//!< Virtual version of get hash.

		//-----------------------------
		//!\brief	run to change state and go to the next logic block.
		//!\note	Looks for logic inputs, arithmetic logic other bools
		virtual void run(cContext* pCon);

	protected:

		bool mKeepSearchingStack;
		dProgramStack::reverse_iterator mStackItr;
		dNameHash mStackHash;
		std::stack<bool> mNodes; //!< the input logic is reduced down to a series of bool nodes.
		bool scrBool;

		cPlug<eBoolOp>	mOp;	//!< What operation to perform.
		cPlug<ptrFig> 	mNext; //!< Points to the next figment.
	};

	//-------------------------------------------------------------------------------------
	// Has a series of nodes which are either inputs from a logic figment further upstream,
	// or an equation operator. Operates on binary logic as if they were normal numbers, 1 being
	// true, and 0 being false.
	class cEquation: public cFigment, private tOutline<cEquation>{
	public:

		//-----------------------------
		static const dNatChar* identify(){ return "equation"; }

		//-----------------------------
		cEquation();
		virtual ~cEquation();

		//-----------------------------
		virtual const dNatChar* name() const{ return identify(); }		//!< Virtual version of identify.
		virtual dNameHash hash() const{ return tOutline<cEquation>::hash(); }		//!< Virtual version of get hash.

		//-----------------------------
		//!\brief	run to change state and go to the next logic block.
		//!\note	Looks for logic inputs, arithmetic logic other bools
		virtual void run(cContext* pCon);
	};
}

#endif
