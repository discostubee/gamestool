#ifndef OPSORDINARYDATA_HPP
#define OPSORDINARYDATA_HPP

#include "opOnAny.hpp"
#include "gt_string.hpp"

///////////////////////////////////////////////////////////////////////////////////
//
namespace gt{

	template<>
	class cAnyOp::tOps<dText>{
	private:
		static void assignText(const dText & pFrom, void *pTo){
			reinterpret_cast<dText*>(pTo)->t.assign(pFrom);
		}

		static void appendText(const dText & pFrom, void *pTo){
			reinterpret_cast<dText*>(pTo)->t.append(pFrom);
		}

		static void assignNatStr(const dText & pFrom, void *pTo){
			reinterpret_cast<dNatStr*>(pTo)->t.assign( toNStr(pFrom) );
		}

		static void appendNatStr(const dText & pFrom, void *pTo){
			reinterpret_cast<dNatStr*>(pTo)->t.append( toNStr(pFrom) );
		}

		static void assignPlaStr(const dText & pFrom, void *pTo){
			reinterpret_cast<dStr*>(pTo)->assign( toPStr(pFrom) );
		}

		static void appendPlaStr(const dText & pFrom, void *pTo){
			reinterpret_cast<dStr*>(pTo)->append( toPStr(pFrom) );
		}


	public:
		static void setup(tKat<dText> * pK, cAnyOp * pUsing){
			pK->addAss(&getRef(), genPlugType<dText>(), assignText);
			pK->addApp(&getRef(), genPlugType<dText>(), appendText);
			pK->addAss(&getRef(), genPlugType<dNatStr>(), assignNatStr);
			pK->addApp(&getRef(), genPlugType<dNatStr>(), appendNatStr);
			pK->addAss(&getRef(), genPlugType<dStr>(), assignPlaStr);
			pK->addApp(&getRef(), genPlugType<dStr>(), appendPlaStr);
		}
	};

	template<>
	class cAnyOp::tOps<dNatStr>{
	private:
		static void assignText(const dNatStr & pFrom, void *pTo){
			reinterpret_cast<dText*>(pTo)->t.assign( toText(pFrom) );
		}

		static void appendText(const dNatStr & pFrom, void *pTo){
			reinterpret_cast<dText*>(pTo)->t.append( toText(pFrom) );
		}

		static void assignNatStr(const dNatStr & pFrom, void *pTo){
			reinterpret_cast<dNatStr*>(pTo)->t.assign( pFrom );
		}

		static void appendNatStr(const dNatStr & pFrom, void *pTo){
			reinterpret_cast<dNatStr*>(pTo)->t.append( pFrom );
		}

		static void assignPlaStr(const dNatStr & pFrom, void *pTo){
			reinterpret_cast<dStr*>(pTo)->assign( toPStr(pFrom) );
		}

		static void appendPlaStr(const dNatStr & pFrom, void *pTo){
			reinterpret_cast<dStr*>(pTo)->append( toPStr(pFrom) );
		}

	public:
		static void setup(tKat<dNatStr> * pK, cAnyOp * pUsing){
			pK->addAss(&getRef(), genPlugType<dText>(), assignText);
			pK->addApp(&getRef(), genPlugType<dText>(), appendText);
			pK->addAss(&getRef(), genPlugType<dNatStr>(), assignNatStr);
			pK->addApp(&getRef(), genPlugType<dNatStr>(), appendNatStr);
			pK->addAss(&getRef(), genPlugType<dStr>(), assignPlaStr);
			pK->addApp(&getRef(), genPlugType<dStr>(), appendPlaStr);
		}
	};

	template<>
	class cAnyOp::tOps<dStr>{
	private:
		static void assignText(const dStr & pFrom, void *pTo){
			reinterpret_cast<dText*>(pTo)->t.assign( toText(pFrom) );
		}

		static void appendText(const dStr & pFrom, void *pTo){
			reinterpret_cast<dText*>(pTo)->t.append( toText(pFrom) );
		}

		static void assignNatStr(const dStr & pFrom, void *pTo){
			reinterpret_cast<dNatStr*>(pTo)->t.assign( toNStr(pFrom) );
		}

		static void appendNatStr(const dStr & pFrom, void *pTo){
			reinterpret_cast<dNatStr*>(pTo)->t.append( toNStr(pFrom) );
		}

		static void assignPlaStr(const dStr & pFrom, void *pTo){
			reinterpret_cast<dStr*>(pTo)->assign( pFrom );
		}

		static void appendPlaStr(const dStr & pFrom, void *pTo){
			reinterpret_cast<dStr*>(pTo)->append( pFrom );
		}

	public:
		static void setup(tKat<dStr> * pK, cAnyOp * pUsing){
			pK->addAss(&getRef(), genPlugType<dText>(), assignText);
			pK->addApp(&getRef(), genPlugType<dText>(), appendText);
			pK->addAss(&getRef(), genPlugType<dNatStr>(), assignNatStr);
			pK->addApp(&getRef(), genPlugType<dNatStr>(), appendNatStr);
			pK->addAss(&getRef(), genPlugType<dStr>(), assignPlaStr);
			pK->addApp(&getRef(), genPlugType<dStr>(), appendPlaStr);
		}
	};

	template<>
	class cAnyOp::tOps<const dPlaChar *>{
	private:
		static void assignStr(const dPlaChar * const& pFrom, void * pTo){
			reinterpret_cast<dStr*>(pTo)->assign( pFrom );
		}

		static void appendStr(const dPlaChar * const& pFrom, void * pTo){
			reinterpret_cast<dStr*>(pTo)->append( pFrom );
		}

		static void assignNatStr(const dPlaChar * const& pFrom, void * pTo){
			reinterpret_cast<dNatStr*>(pTo)->t = toNStr(pFrom);
		}

		static void appendNatStr(const dPlaChar * const& pFrom, void * pTo){
			reinterpret_cast<dNatStr*>(pTo)->t.append( toNStr(pFrom) );
		}

		static void assignText(const dPlaChar * const& pFrom, void * pTo){
			reinterpret_cast<dText*>(pTo)->t = toText(pFrom);
		}

		static void appendText(const dPlaChar * const& pFrom, void * pTo){
			reinterpret_cast<dText*>(pTo)->t.append( toText(pFrom) );
		}

	public:
		static void setup(tKat<const dPlaChar *> * pK, cAnyOp * pUsing){
			pK->addAss(&getRef(), genPlugType<dStr>(), assignStr);
			pK->addApp(&getRef(), genPlugType<dStr>(), appendStr);
			pK->addAss(&getRef(), genPlugType<dNatStr>(), assignNatStr);
			pK->addApp(&getRef(), genPlugType<dNatStr>(), appendNatStr);
			pK->addAss(&getRef(), genPlugType<dText>(), assignText);
			pK->addApp(&getRef(), genPlugType<dText>(), appendText);
		}
	};
}

#endif
