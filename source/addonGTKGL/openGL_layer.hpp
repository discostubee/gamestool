/*
 *
 * !\file
 * !\brief
 */

#ifndef OPENGL_LAYER_HPP
#define OPENGL_LAYER_HPP

#include <gt_graphics/layer.hpp>
#include <gtkmm.h>

namespace gt{

	class cLayer_gtkGL: public cLayer, private tOutline<cLayer_gtkGL>{
	protected:

		//- Shared
		static ::Gtk::Main gGTKMain;	//!<

		//- Personal
		::Gtk::Window mWin;

	public:
		static const char* identify(){ return "layer gtkgl"; }
		static dNameHash replaces(){ return getHash<cLayer>(); }

		cLayer_gtkGL();
		virtual ~cLayer_gtkGL();

		virtual const char* name() const{ return identify(); }		//!< Virtual version of identify.

		virtual void run(cContext* pCon);
	};
}

#endif
