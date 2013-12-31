/*
 * !\file	arti3D.hpp
 * !\brief
 */

#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

#include "gt_base/chainLink.hpp"
#include "primitives.hpp"

namespace gt{

	//!\brief	Transform the rest of the chain by the values here.
	class cTransform: public cChainLink{
	public:
		static const cPlugTag* xPT_pos;
		static const cPlugTag* xPT_rot; //!< A Euler vector.
		static const cPlugTag* xPT_sca;

		static const cCommand::dUID	xGetTransforms;	//!< Sets relevant plugs to the current transform values of this artifact.
		static const cCommand::dUID	xSetTransforms;	//!< Uses the provided plugs to set the transform values.

		cTransform();
		virtual ~cTransform();

		GT_IDENTIFY("transform");
		GT_EXTENDS(cChainLink);
		GT_VERSION(1);
		virtual dNameHash hash() const{ return getHash<cTransform>(); }

	protected:
		tPlug< t3DVec<dUnitVDis> > mPos, mSca;
		tPlug< tEulerRot<dUnitVDis> > mRot;

		void patGetTransforms(ptrLead aLead);
		void patSetTransforms(ptrLead aLead);

		virtual void getTransforms(){ DONT_USE_THIS; } //!< Refresh the plug data using the final implementation's data.
		virtual void setTransforms(){ DONT_USE_THIS; } //!< Refresh the implementation data using the plugs.
	};

}

#endif
