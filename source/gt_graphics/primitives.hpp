/*
 * !\file	primitives.hpp
 * !\brief	This is 'sort of' the starting point for the graphics library.
 */

#ifndef PRIMITIVES_HPP
#define PRIMITIVES_HPP

namespace gt{
	typedef float			dUnitVDis;		//!< Distance in virtual units, in other words there's no real unit for this distance.
	typedef float			dUnitRad;		//!< Angle in radiants.
	typedef unsigned int	dUnitPix;		//!< Number of pixels.
	typedef unsigned int	dPix32;			//!< Gamestool 32 bit pixel.

	//!\brief A 2D vector
	template<typename UNIT>
	struct t2DVec{
		UNIT		scaler;
		dUnitRad	rot;

		t2DVec<UNIT>& operator+= (const t2DVec &aCopyMe){
			ASRT_NOTSELF(&aCopyMe);
			scaler += aCopyMe.scaler;
			rot += aCopyMe.rot;
			return *this;
		}
	};

	struct sEulerRot{
		dUnitRad	x, y, z, A;

		sEulerRot& operator+= (const sEulerRot &aCopyMe){
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
		UNIT		scaler;
		sEulerRot	rotate;

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
		class rectangle{
		public:
			UNIT top, left, bottom, right;

			rectangle() : top(0), left(0), bottom(0), right(0) {}
			~rectangle(){}

			rectangle<UNIT>& operator+= (const rectangle &aCopyMe){
				ASRT_NOTSELF(&aCopyMe);
				top += aCopyMe.top;
				left += aCopyMe.left;
				bottom += aCopyMe.bottom;
				right += aCopyMe.right;
				return *this;
			}
		};
	}


}

#endif
