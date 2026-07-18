#ifndef QED_FORCE_REACTION_CALCULATION_ENGINE
#define QED_FORCE_REACTION_CALCULATION_ENGINE

#include <vector>
#include <algorithm>
#include <chrono>
#include <thread>
#include <string_view>


using GlobalPrecision = double;



struct PhysicsValue {
	GlobalPrecision Scalar;
	int Metre, Second, Kilogramme, Kelvin, Newton;
	constexpr PhysicsValue(GlobalPrecision scalar, int metre, int second, int kilogramme, int kelvin, int newton) : Scalar(scalar), Metre(metre), Second(second), Kilogramme(kilogramme), Kelvin(kelvin), Newton(newton) { }
	constexpr PhysicsValue(GlobalPrecision scalar) : Scalar(scalar), Metre(0), Second(0), Kilogramme(0), Kelvin(0), Newton(0) { }
};
PhysicsValue operator+ (const PhysicsValue a, const PhysicsValue b) {
	if (a.Metre == b.Metre && a.Second == b.Second && a.Kilogramme == b.Kilogramme && a.Kelvin == b.Kelvin && a.Newton == b.Newton) return PhysicsValue(a.Scalar + b.Scalar, a.Metre, a.Second, a.Kilogramme, a.Kelvin, a.Newton);
	else throw std::logic_error("PhysicsValue Error: Unit type in addition doesn't match up.");
}
PhysicsValue operator- (const PhysicsValue a) {
	return PhysicsValue(-a.Scalar, a.Metre, a.Second, a.Kilogramme, a.Kelvin, a.Newton);
}
PhysicsValue operator- (const PhysicsValue a, const PhysicsValue b) {
	return a + (-b);
}
PhysicsValue operator* (const PhysicsValue a, const PhysicsValue b) {
	return PhysicsValue(a.Scalar*b.Scalar, a.Metre + b.Metre, a.Second + b.Second, a.Kilogramme + b.Kilogramme, a.Kelvin + b.Kelvin, a.Newton + b.Newton);
}
PhysicsValue operator/ (const PhysicsValue a, const PhysicsValue b) {
	return PhysicsValue(a.Scalar/b.Scalar, a.Metre - b.Metre, a.Second - b.Second, a.Kilogramme - b.Kilogramme, a.Kelvin - b.Kelvin, a.Newton - b.Newton);
}

constexpr PhysicsValue operator"" _mm(const long double n) { return PhysicsValue(n/1'000, 1, 0, 0, 0, 0); }
constexpr PhysicsValue operator"" _cm(const long double n) { return PhysicsValue(n/100, 1, 0, 0, 0, 0); }
constexpr PhysicsValue operator"" _m(const long double n) { return PhysicsValue(n, 1, 0, 0, 0, 0); }
constexpr PhysicsValue operator"" _km(const long double n) { return PhysicsValue(n*1'000, 0, 0, 1, 0, 0); }

constexpr PhysicsValue operator"" _s(const long double n) { return PhysicsValue(n, 0, 1, 0, 0, 0); }
constexpr PhysicsValue operator"" _sec(const long double n) { return PhysicsValue(n, 0, 1, 0, 0, 0); }
constexpr PhysicsValue operator"" _min(const long double n) { return PhysicsValue(n/60, 0, 1, 0, 0, 0); }
constexpr PhysicsValue operator"" _h(const long double n) { return PhysicsValue(n/3'600, 0, 1, 0, 0, 0); }
constexpr PhysicsValue operator"" _hrs(const long double n) { return PhysicsValue(n/3'600, 0, 1, 0, 0, 0); }
constexpr PhysicsValue operator"" _d(const long double n) { return PhysicsValue(n/86'400, 0, 1, 0, 0, 0); }
constexpr PhysicsValue operator"" _dys(const long double n) { return PhysicsValue(n/86'400, 0, 1, 0, 0, 0); }

constexpr PhysicsValue operator"" _mg(const long double n) { return PhysicsValue(n/1'000'000, 0, 0, 1, 0, 0); }
constexpr PhysicsValue operator"" _g(const long double n) { return PhysicsValue(n/1'000, 0, 0, 1, 0, 0); }
constexpr PhysicsValue operator"" _kg(const long double n) { return PhysicsValue(n, 0, 0, 1, 0, 0); }

constexpr PhysicsValue operator"" _K(const long double n) { return PhysicsValue(n, 0, 0, 0, 1, 0); }
constexpr PhysicsValue operator"" _C(const long double n) { return PhysicsValue(n + 273.15, 0, 0, 0, 1, 0); }

constexpr PhysicsValue operator"" _N(const long double n) { return PhysicsValue(n, 0, 0, 0, 0, 1); }
constexpr PhysicsValue operator"" _kN(const long double n) { return PhysicsValue(n*1'000, 0, 0, 0, 0, 1); }
constexpr PhysicsValue operator"" _MN(const long double n) { return PhysicsValue(n*1'000'000, 0, 0, 0, 0, 1); }




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
		PhysicsValue Density;
		float ElectricalConductivity, ThermalConductivity;
		float Hardness, Toughness, Malleability, Ductility;
		float YieldStrength;
	};
	class Object {
		bool Server;
		PhysicsValue Mass;
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