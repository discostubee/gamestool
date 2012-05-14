#include "logic.hpp"

using namespace gt;

////////////////////////////////////////////////////////////
const cPlugTag* cLogicInput::xPT_inputLead = tOutline<cLogicInput>::makePlugTag("input lead");

const cPlugTag* cLogicInput::xPT_inputPlug = tOutline<cLogicInput>::makePlugTag("input plug");

const cCommand* cLogicInput::xAddInputs = tOutline<cLogicInput>::makeCommand(
	"add inputs",
	cLogicInput::eAddInput,
	cLogicInput::xPT_inputLead,
	cLogicInput::xPT_inputPlug,
	NULL
);

cLogicInput::cLogicInput(){
}

cLogicInput::~cLogicInput(){
}

void
cLogicInput::run(cContext* pCon){
	PROFILE;

	// Shouldn't have to do any more than this, because each lead directly references the data we're interested
	// in.

	DBUG_LO("running logic input.");

	if(pCon->isStacked(this))
		return;

	pCon->add(this);
	mNext.mD->run(pCon);
	pCon->finished(this);
}

void
cLogicInput::jack(ptrLead pLead){
	PROFILE;

	try{
		switch( pLead->mCom->getSwitch<cLogicInput>() ){
			case eAddInput:{

				DBUG_LO("Added input");
			}break;

			default:
			case eNotMyBag:{
				cFigment::jack(pLead);
			}break;
		}
	}catch(excep::base_error &e){
		WARN(e);
	}
}

////////////////////////////////////////////////////////////
cLogicOutput::cLogicOutput(){
}

cLogicOutput::~cLogicOutput(){
}

void
cLogicOutput::run(cContext* pCon){
	if(pCon->isStacked(this))
		return;

/*
	while( mStackItr != pCon->endOfStack() ){
		mStackHash = (*mStackItr)->hash();
		if( mStackHash == getHash<cBoolLogic>() ){
			//cBoolLogic* temp = const_cast<cBoolLogic*>( reinterpret_cast<const cBoolLogic*>( (*mStackItr) ) );

			for(scrOutItr = mOutputs.begin(); scrOutItr != mOutputs.end(); ++scrOutItr){
				for(
					scrPlugItr = scrOutItr->mPlugsToUse.begin();
					scrPlugItr != scrOutItr->mPlugsToUse.end();
					++scrPlugItr
				){
					//scrOutItr->mLead->add( &(temp->mState), (*scrPlugItr) );
				}

			}
			break;

		}else if( mStackHash == getHash<cLogicInput>() ){
			for(scrOutItr = mOutputs.begin(); scrOutItr != mOutputs.end(); ++scrOutItr){
			}
			break;

		}else if( mStackHash == getHash<cArithmetic>() ){
			for(scrOutItr = mOutputs.begin(); scrOutItr != mOutputs.end(); ++scrOutItr){
			}
			break;

		}
	}
*/
}

////////////////////////////////////////////////////////////
cArithmetic::cArithmetic(){
	mOp = eSummation;
	mResult = 0.0f;
}

cArithmetic::~cArithmetic(){
}

void
cArithmetic::run(cContext* pCon){
}

////////////////////////////////////////////////////////////
cBoolLogic::cBoolLogic():
	mKeepSearchingStack(false)
{
	mOp = eOR;
	mState = false;
	mNext = gWorld->getEmptyFig();
}

cBoolLogic::~cBoolLogic(){
}

void
cBoolLogic::run(cContext* pCon){
	if(pCon->isStacked(this))
		return;
/*
	//Go to the last logic operator
	mKeepSearchingStack = true;
	mStackItr = pCon->inspectAStack();

	// Find a logic block further up the stack and interpret its data as a series of bool nodes.
	while( mKeepSearchingStack && mStackItr != pCon->endOfStack() ){
		mStackHash = (*mStackItr)->hash();
		if( mStackHash == getHash<cBoolLogic>() ){
			mKeepSearchingStack = false;

		}else if( mStackHash == getHash<cLogicInput>() ){
			cLogicInput* tempInput = const_cast<cLogicInput*>( reinterpret_cast<const cLogicInput*>( (*mStackItr) ) );
			std::vector<sLeadConnect>::iterator inItr;
			sLeadConnect::dVecPlugTag::iterator tagItr;

			for(
				inItr = tempInput->mInputs.begin();
				inItr != tempInput->mInputs.end();
				++inItr
			){
				for(
					tagItr = inItr->mPlugsToUse.begin();
					tagItr != inItr->mPlugsToUse.end();
					++tagItr
				){
					//try{
					//	mNodes.push(
					//		inItr->mLead->getD( (*tagItr) )->getMDCopy<bool>()
					//	);
					//}catch(excep::cantCopy){
						//// do nothing.
					//}
				}
			}

			mKeepSearchingStack = false;
		}

		++mStackItr;
	}

	// Now apply the bool logic
	if( !mNodes.empty() ){
		switch(mOp.mD){
			case eOR:	scrBool=false; break;
			case eAND: 	scrBool=true; break;
			case eXOR: 	scrBool=false; break;
			case eNAND:	scrBool=false; break;
		}

		while(!mNodes.empty()){
			switch(mOp.mD){
				case eOR:	if( scrBool == false && mNodes.top() == true ){ scrBool = true; } break;
				case eAND: 	break;
				case eXOR: 	break;
				case eNAND:	break;
			}
			mNodes.pop();
		}
		mState = scrBool;
	}
*/
	pCon->add(this);
	mNext.mD->run(pCon);
	pCon->finished(this);
}

////////////////////////////////////////////////////////////

cEquation::cEquation(){
}

cEquation::~cEquation(){
}

void
cEquation::run(cContext* pCon){
}
