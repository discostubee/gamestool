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
	};

	struct sEulerRot{
		dUnitRad	x, y, z, A;
	};

	//!\brief	Euclidean vector
	template<typename UNIT>
	struct t3DVec{
		UNIT		scaler;
		sEulerRot	rotate;
	};

	//!\brief	Dimensions of a 2D shape.
	template<typename UNIT>
	struct tDim2D{
		UNIT width, height;
	};

	template<typename UNIT>
	struct tPoint2D{
		UNIT x, y;
	};

	template<typename UNIT>
	struct tPoint3D{
		UNIT x, y, z;
	};

	//!\brief	Collection of 2D shapes.
	namespace shape{

		template<typename UNIT>
		class rectangle{
		public:
			rectangle() : top(0), left(0), bottom(0), right(0) {}
			~rectangle(){}

			UNIT top, left, bottom, right;
		};
	}


}

#endif
