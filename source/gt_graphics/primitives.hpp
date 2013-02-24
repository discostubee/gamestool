/*
 * !\file	primitives.hpp
 * !\brief	graphic primitives
 */

#ifndef PRIMITIVES_HPP
#define PRIMITIVES_HPP

namespace gt{
	typedef float			dUnitVDis;	//!< Virtual distance measurement.
	typedef float			dUnitVRad;	//!< Virtual angle measurement, in radians.
	typedef unsigned int	dUnitPix32;	//!< 32 bit pixel.

	//!\brief A 2D vector
	struct s2DVec{
		dUnitVDis	scaler;
		dUnitVRad	rot;
	};

	struct sEulerRot{
		dUnitVRad	x, y, z, A;
	};

	struct s3DVec{
		dUnitVDis	scaler;
		sEulerRot	rotate;
	};

	template<typename UNIT>
	struct sWH{
		UNIT width, height;
	};

	template<typename UNIT>
	struct sPoint2D{
		UNIT x, y;
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
