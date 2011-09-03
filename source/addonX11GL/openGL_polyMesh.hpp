
#ifndef OPENGL_POLYMESH
#define OPENGL_POLYMESH

#include "openGL_layer.hpp"
#include <gt_graphics/polygonMesh.hpp>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

namespace gt{

	//!\brief	Open GL 3.1 version of the polygon mesh.
	//!\note	Code taken from http://www.opengl.org/wiki/http://www.opengl.org/wiki/Tutorial:_OpenGL_3.1_The_First_Triangle_%28C%2B%2B/Win%29
	//!\todo	Make GL meshes coordinate with the rest of the openGL system, adding to a single vertex+index buffer.
	class cPolyMesh_GL : public cPolyMesh, private tOutline<cPolyMesh_GL>{
	public:
		typedef float dGLFloat;	//!< Ensure control over size of float.
		typedef GLuint handVBO;	//!< ID handle to vertex buffer object.
		typedef GLuint handIBO;	//!< ID handle to the index buffer object.
		typedef ushort dIdxV;	//!< Index into vertex buffer, where arrays form index buffers.

		//!\brief
		struct sGLFloatVert{
			dGLFloat x, y, z;       //!< Vertex
			dGLFloat nx, ny, nz;    //!< Normal
			dGLFloat u, v;			//!< Texture mapping
		};

		cPolyMesh_GL();
		virtual ~cPolyMesh_GL();

		static const dNatChar* identify(){ return "gl mesh"; }
		virtual const dNatChar* name() const { return identify(); }
		virtual dNameHash hash() const { return tOutline<cPolyMesh_GL>::hash(); }

		static dNameHash replaces(){ return getHash<cPolyMesh>(); }
		virtual dNameHash getReplacement() const{ return cPolyMesh_GL::replaces(); }

		virtual void run(cContext* pCon);	//!< Draw this mesh

	private:
		sGLFloatVert *vbuff;
		dIdxV *ibuff;
		handVBO mVBO;	//!< Handle to the vertex buffer object.
		handIBO mIBO;
		size_t polyCount;
		size_t vertCount;

		void formatGLMesh();	//!< Takes the lazy mesh and turns it into the best format for openGL.
	};
}

#endif
