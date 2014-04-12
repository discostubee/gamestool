/*
 * !\file	primitives.hpp
 * !\brief	This is 'sort of' the starting point for the graphics library.
 * !\note	All graphics primitives are structures so things are kept simple, and it's ok to access public data.
 * !\note	As another note, (seeing as how this file is kinda the beginning of the graphics library) it's best to use
 * !		generic plugs that are good for saving and passing values in the jack function, here. Vs the specific
 * !		data needed for the fully implemented versions. This may seem to make a class somewhat bloated, but it helps
 * !		to speed the generic interfaces by keeping a copy in its generic form, as well as multithreading.
 */

#ifndef PRIMITIVES_HPP
#define PRIMITIVES_HPP

namespace gt{
	typedef float			dUnitVDis;		//!< Distance in virtual units, in other words there's no real unit for this distance.
	typedef float			dUnitRad;		//!< Angle in radians.
	typedef unsigned int	dUnitPix;		//!< Number of pixels.
	typedef unsigned int	dPix32;			//!< Gamestool 32 bit pixel.

	//!\brief A 2D vector
	template<typename UNIT>
	struct t2DVec{
		UNIT scaler;
		UNIT rot;

		t2DVec<UNIT>& operator+= (const t2DVec &aCopyMe){
			ASRT_NOTSELF(&aCopyMe);
			scaler += aCopyMe.scaler;
			rot += aCopyMe.rot;
			return *this;
		}
	};

	template<typename UNIT>
	struct tEulerRot{
		UNIT x, y, z, A;

		tEulerRot& operator+= (const tEulerRot &aCopyMe){
			ASRT_NOTSELF(&aCopyMe);
			x += aCopyMe.x;
			y += aCopyMe.y;
			z += aCopyMe.z;
			A += aCopyMe.A;
			return *this;
		}
	};

	//!\brief	Euclidean vector
	template<typename UNIT>
	struct t3DVec{
		UNIT scaler;
		UNIT rotate;

		t3DVec<UNIT>& operator+= (const t3DVec &aCopyMe){
			ASRT_NOTSELF(&aCopyMe);
			scaler += aCopyMe.scaler;
			rotate += aCopyMe.rotate;
			return *this;
		}
	};

	//!\brief	Dimensions of a 2D shape.
	template<typename UNIT>
	struct tDim2D{
		UNIT width, height;

		tDim2D<UNIT>& operator+= (const tDim2D &aCopyMe){
			ASRT_NOTSELF(&aCopyMe);
			width += aCopyMe.width;
			height += aCopyMe.height;
			return *this;
		}
	};

	template<typename UNIT>
	struct tPoint2D{
		UNIT x, y;

		tPoint2D<UNIT>& operator+= (const tPoint2D &aCopyMe){
			ASRT_NOTSELF(&aCopyMe);
			x += aCopyMe.x;
			y += aCopyMe.y;
			return *this;
		}
	};

	template<typename UNIT>
	struct tPoint3D{
		UNIT x, y, z;

		tPoint3D<UNIT>& operator+= (const tPoint3D &aCopyMe){
			ASRT_NOTSELF(&aCopyMe);
			x += aCopyMe.x;
			y += aCopyMe.y;
			z += aCopyMe.z;
			return *this;
		}
	};

	//!\brief	Collection of 2D shapes.
	namespace shape{

		template<typename UNIT>
		struct tRectangle{
		public:
			UNIT top, left, bottom, right;

			tRectangle() : top(0), left(0), bottom(0), right(0) {}
			tRectangle(UNIT t, UNIT l, UNIT b, UNIT r) : top(t), left(l), bottom(b), right(r) {}

			tRectangle<UNIT>& operator+= (const tRectangle &aCopyMe){
				ASRT_NOTSELF(&aCopyMe);
				top += aCopyMe.top;
				left += aCopyMe.left;
				bottom += aCopyMe.bottom;
				right += aCopyMe.right;
				return *this;
			}
		};
	}

	//!\brief	Collection of basic 3D shapes.
	namespace geometry{

		//!\brief	When positioning a cube, assume its 0 point is always in the centre. However, the width or height is the total distance, not the
		//!			offset from the centre in either direction.
		template<typename UNIT>
		struct tCube{
			UNIT width, height;	//!< Total distances.

			tCube() : width(0), height(0) {}

			tCube<UNIT>& operator+= (const tCube &pCopyMe){
				if(&pCopyMe != this){
					width += pCopyMe.width;
					height += pCopyMe.height;
				}
				return *this;
			}
		};

		//!\brief	Similar to the cube, in that the 0 point is assumed to be the centre.
		template<typename UNIT>
		struct tSphere{
			UNIT radius;

			tSphere() : radius(0) {}

			tSphere<UNIT>& operator+= (const tSphere &pCopyMe){
				if(&pCopyMe != this){
					radius += pCopyMe.radius;
				}
				return *this;
			}
		};
	}
}

#endif
