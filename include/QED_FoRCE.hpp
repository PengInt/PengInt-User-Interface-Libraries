#ifndef QED_FORCE_REACTION_CALCULATION_ENGINE
#define QED_FORCE_REACTION_CALCULATION_ENGINE

#include <vector>
#include <algorithm>
#include <chrono>
#include <thread>


using GlobalPrecision = float;
using gp = GlobalPrecision;

/*
using N = GlobalPrecision;
using S = GlobalPrecision;
using kg_m3 = GlobalPrecision;
using S_m = GlobalPrecision;
using W_mK = GlobalPrecision;

using K = GlobalPrecision;
constexpr K operator"" _C(long double C) { return C+273.15; }
using C = GlobalPrecision;
constexpr C operator"" _K(long double K) { return K-273.15; }*/

template<typename T>
struct Unit {
	gp Val;
public:
	Unit(gp val) : Val(val) { }
};

struct Meter : Unit<struct MeterTag> {
	gp Val;
public:
	Meter(gp val) : Unit(val) { }
	operator gp() const { return Val; }
}; using m = Meter;
constexpr m operator"" _mm(long double n) { return m(n/1000); }
constexpr m operator"" _cm(long double n) { return m(n/100); }
constexpr m operator"" _dm(long double n) { return m(n/10); }
constexpr m operator"" _m(long double n) { return m(n); }
constexpr m operator"" _dam(long double n) { return m(n*10); }
constexpr m operator"" _hm(long double n) { return m(n*100); }
constexpr m operator"" _km(long double n) { return m(n*1000); }

struct Kilogramme : Unit<struct KilogrammeTag> {
	gp Val;
public:
	Kilogramme(gp val) : Unit(val) { }
	operator gp() const { return Val; }
}; using kg = Kilogramme;
constexpr kg operator"" _mg(long double n) { return kg(n/1000000); }
constexpr kg operator"" _cg(long double n) { return kg(n/100000); }
constexpr kg operator"" _dg(long double n) { return kg(n/10000); }
constexpr kg operator"" _g(long double n) { return kg(n/1000); }
constexpr kg operator"" _dag(long double n) { return kg(n/100); }
constexpr kg operator"" _hg(long double n) { return kg(n/10); }
constexpr kg operator"" _kg(long double n) { return kg(n/1); }


class Kelvin;

struct Celsius : Unit<struct CelsiusTag> {
	gp Val;
public:
	Celsius(gp val) : Unit(val) { }
	operator Kelvin();
	operator gp() const { return Val; }
}; using C = Celsius;
constexpr C operator"" _C(long double n) { return C(n); }
constexpr C operator"" _Kc(long double n) { return C(n-273.15); }

struct Kelvin : Unit<struct KelvinTag> {
	gp Val;
public:
	Kelvin(gp val) : Unit(val) { }
	operator Celsius();
	operator gp() const { return Val; }
}; using K = Kelvin;
constexpr K operator"" _Ck(long double n) { return K(n+273.15); }
constexpr K operator"" _K(long double n) { return K(n); }

Celsius ConvertTemperature(Kelvin k) { return k-273.15; }
Kelvin ConvertTemperature(Celsius k) { return k+273.15; }

Celsius::operator Kelvin() { return ConvertTemperature(*this); }
Kelvin::operator Celsius() { return ConvertTemperature(*this); }

template<typename T, typename U> requires (!std::same_as<T, U>)
bool operator== (const Unit<T>&, const Unit<U>&) = delete;
template<typename T, typename U> requires (!std::same_as<T, U>)
bool operator<=> (const Unit<T>&, const Unit<U>&) = delete;
template<typename T, typename U> requires (!std::same_as<T, U>)
bool operator+ (const Unit<T>&, const Unit<U>&) = delete;
template<typename T, typename U> requires (!std::same_as<T, U>)
bool operator- (const Unit<T>&, const Unit<U>&) = delete;

namespace FoRCE {
	struct Vec3 {
		float x, y, z;
	};
	struct Triangle {
		Vec3* a, b, c;
	};
	struct BVH_Node {
		Vec3 v0, v1;
		bool isLeaf;
		std::vector<BVH_Node*> BVH;
		std::vector<Triangle*> Tri;
	};
	struct PhysicsMaterial {
		kg_m3 Density;
		float ElectricalConductivity, ThermalConductivity;
		float Hardness, Toughness, Malleability, Ductility;
		float YieldStrength;
	};
	class Object {
		bool Server;
		kg Mass;
	};
	class PhysicsEngine;
	std::vector<PhysicsEngine*> PhysicsEngines;
	class PhysicsEngine {
	public:
		std::vector<Object*> Objects;
		PhysicsEngine() {
			PhysicsEngines.push_back(this);
		}
		Object* push_back(Object* obj) { Objects.push_back(obj); return obj; }
		void Tick(float dt, float t) {
			
		}
	};
	void DestroyObject(Object*& obj) {
		delete obj;
		for (PhysicsEngine* PE : PhysicsEngines) std::erase(PE->Objects, obj);
		obj = nullptr;
	}
}

namespace FoRCE_ServerSide {
	class PhysicsEngine {
		FoRCE::PhysicsEngine* PE;
	public:
		PhysicsEngine() { PE = new FoRCE::PhysicsEngine(); }
		virtual void OnUpdate(float dt, float t) { }
		void Run() {
			auto lastframe = std::chrono::high_resolution_clock::now();
			while (true) {
				auto thisframe = std::chrono::high_resolution_clock::now();
				float dt = std::chrono::duration<float, std::micro>(thisframe-lastframe).count()/1000000;
				using namespace std::chrono_literals;
				if (dt < 0.01f) { std::this_thread::sleep_for(0.01s - dt*1s); dt = 0.01; }
				PE->Tick(dt, 0);
				lastframe = thisframe;
			}
		}
	};
}

#endif // QED_FORCE_REACTION_CALCULATION_ENGINE