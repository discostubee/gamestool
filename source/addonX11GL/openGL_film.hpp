/*
 * !\file
 * !\brief
 */

#ifndef OPENGL_FILM_HPP
#define OPENGL_FILM_HPP

#include "openGL_stage.hpp"

#include "gt_graphics/film.hpp"

namespace gt{

	class cFilm_X11GL: public cFilm{
	public:
		cFilm_X11GL();
		virtual ~cFilm_X11GL();

		GT_IDENTIFY("film gl");
		GT_REPLACES(cFilm);
		virtual dNameHash hash() const { return getHash<cFilm_X11GL>(); }

		virtual void work(cContext* pCon);

	protected:
		virtual void updateLayout();
		virtual void getRez(tDim2D<dUnitPix> &aSetMe);
	};
}

#endif
