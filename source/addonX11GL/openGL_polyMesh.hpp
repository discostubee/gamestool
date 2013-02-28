
#ifndef OPENGL_POLYMESH_HPP
#define OPENGL_POLYMESH_HPP

#include "openGL_stage.hpp"
#include "gt_graphics/polygonMesh.hpp"

namespace gt{

	//!\brief	Open GL 3.1 version of the polygon mesh.
	//!\note	Code taken from http://www.opengl.org/wiki/http://www.opengl.org/wiki/Tutorial:_OpenGL_3.1_The_First_Triangle_%28C%2B%2B/Win%29
	//!\todo	Make GL meshes coordinate with the rest of the openGL system, adding to a single vertex+index buffer.
	class cPolyMesh_X11GL : public cPolyMesh, public tAddonDependent<X11GLAddon>{
	public:
		cPolyMesh_X11GL();
		virtual ~cPolyMesh_X11GL();

		GT_IDENTIFY("polymesh gl");
		GT_REPLACES(cPolyMesh);
		virtual dNameHash hash() const { return getHash<cPolyMesh_X11GL>(); }

		virtual void run(cContext* pCon);	//!< Draw this mesh

	protected:
		typedef float dGLFloat;	//!< Ensure control over size of float.
		typedef GLuint handVBO;	//!< ID handle to vertex buffer object.
		typedef GLuint handIBO;	//!< ID handle to the index buffer object.
		typedef ushort dIdxV;	//!< Index into vertex buffer, where arrays form index buffers.

	private:
		static const short DIMENSIONS = 3;
		static const short IOA_X = 0, IOA_Y = 1, IOA_Z = 2; //!< Index Offset for Axis.

		dGLFloat *vbuff;
		dIdxV *ibuff;
		handVBO mVBO;	//!< Handle to the vertex buffer object.
		handIBO mIBO;
		size_t polyCount;
		size_t vertCount;

		void formatGLMesh();	//!< Takes the lazy mesh and turns it into the best format for openGL.
	};
}

#endif
