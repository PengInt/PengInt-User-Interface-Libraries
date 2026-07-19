#ifndef QED_FORCE_REACTION_CALCULATION_ENGINE
#define QED_FORCE_REACTION_CALCULATION_ENGINE

#include <concepts>
#include <vector>
#include <cmath>
#include <cstdint>
#include <algorithm>
#include <chrono>
#include <thread>
#include <string_view>
#include <unordered_map>


#ifndef FoRCE_PRECISION
	#define FoRCE_PRECISION double
#endif



namespace FoRCE {
	using GlobalPrecision = FoRCE_PRECISION;
	
	
	template<typename T>
	concept typename_numeric = std::is_arithmetic_v<T> and !std::is_same_v<T, bool>;
	
	
	template<typename_numeric T>
	std::vector<T> operator+ (std::vector<T> a, std::vector<T> b) {
		std::vector<T> Output = {};
		for (int i = 0; i < std::min(a.size(), b.size()); i++) Output.push_back(a[i] + b[i]);
		for (int i = 0; i < a.size() - Output.size(); i++) Output.push_back(a[i]);
		for (int i = 0; i < b.size() - Output.size(); i++) Output.push_back(b[i]);
		return Output;
	}
	template<typename_numeric T>
	std::vector<T> operator- (std::vector<T> a) {
		std::vector<T> Output = {};
		for (T t : a) Output.push_back(-t);
		return Output;
	}
	template<typename_numeric T>
	std::vector<T> operator- (std::vector<T> a, std::vector<T> b) {
		return a + (-b);
	}
	
	template<typename T>
	bool operator== (std::vector<T> a, std::vector<T> b) {
		if (a.size() != b.size()) return false;
		for (int i = 0; i < a.size(); i++) if (a[i] != b[i]) return false;
		return true;
	}
	
	
	
	uint8_t __PhysicsUnits = 5;
	struct PhysicsValue {
		GlobalPrecision Scalar;
		std::vector<GlobalPrecision> Units;
		constexpr PhysicsValue(GlobalPrecision scalar, std::vector<GlobalPrecision> units) : Scalar(scalar) {
			Units = {};
			for (int i = 0; i < std::min(__PhysicsUnits, (uint8_t) units.size()); i++) Units.push_back(units[i]);
			for (int i = 0; i < 5 - Units.size(); i++) Units.push_back(0);
		}
		constexpr PhysicsValue(GlobalPrecision scalar, std::unordered_map<int, GlobalPrecision> units) : Scalar(scalar) {
			Units = {};
			for (int i = 0; i < 5; i++) Units.push_back(0);
			for (std::pair<int, GlobalPrecision> entry : units) if (0 <= entry.first && entry.first < __PhysicsUnits) Units[entry.first] = entry.second;
		}
		constexpr PhysicsValue(GlobalPrecision scalar) : Scalar(scalar) {
			Units = {};
			for (int i = 0; i < 5; i++) Units.push_back(0);
		}
	};


	bool operator== (const PhysicsValue a, const PhysicsValue b) {
		return a.Units == b.Units;
	}
	PhysicsValue operator+ (const PhysicsValue a, const PhysicsValue b) {
		if (a == b) return PhysicsValue(a.Scalar + b.Scalar, a.Units);
		else throw std::logic_error("PhysicsValue Error: Unit type in addition doesn't match up.");
	}
	PhysicsValue operator- (const PhysicsValue a) {
		return PhysicsValue(-a.Scalar, a.Units);
	}
	PhysicsValue operator- (const PhysicsValue a, const PhysicsValue b) {
		return a + (-b);
	}
	PhysicsValue operator* (const PhysicsValue a, const PhysicsValue b) {
		return PhysicsValue(a.Scalar*b.Scalar, a.Units + b.Units);
	}
	PhysicsValue operator/ (const PhysicsValue a, const PhysicsValue b) {
		return PhysicsValue(a.Scalar/b.Scalar, a.Units - b.Units);
	}
	
	namespace __ {
		const int m = 0, kg = 1, s = 2, K = 3, A = 4;
	}
	
	constexpr PhysicsValue operator"" _mm(const long double n) { return PhysicsValue(n/1'000, std::unordered_map<int, GlobalPrecision>{{__::m, 1}}); }
	constexpr PhysicsValue operator"" _cm(const long double n) { return PhysicsValue(n/100, std::unordered_map<int, GlobalPrecision>{{__::m, 1}}); }
	constexpr PhysicsValue operator"" _m(const long double n) { return PhysicsValue(n, std::unordered_map<int, GlobalPrecision>{{__::m, 1}}); }
	constexpr PhysicsValue operator"" _km(const long double n) { return PhysicsValue(n*1'000, std::unordered_map<int, GlobalPrecision>{{__::m, 1}}); }
	
	constexpr PhysicsValue operator"" _s(const long double n) { return PhysicsValue(n, std::unordered_map<int, GlobalPrecision>{{__::s, 1}}); }
	constexpr PhysicsValue operator"" _sec(const long double n) { return PhysicsValue(n, std::unordered_map<int, GlobalPrecision>{{__::s, 1}}); }
	constexpr PhysicsValue operator"" _min(const long double n) { return PhysicsValue(n*60, std::unordered_map<int, GlobalPrecision>{{__::s, 1}}); }
	constexpr PhysicsValue operator"" _h(const long double n) { return PhysicsValue(n*3'600, std::unordered_map<int, GlobalPrecision>{{__::s, 1}}); }
	constexpr PhysicsValue operator"" _hrs(const long double n) { return PhysicsValue(n*3'600, std::unordered_map<int, GlobalPrecision>{{__::s, 1}}); }
	constexpr PhysicsValue operator"" _d(const long double n) { return PhysicsValue(n*86'400, std::unordered_map<int, GlobalPrecision>{{__::s, 1}}); }
	constexpr PhysicsValue operator"" _dys(const long double n) { return PhysicsValue(n*86'400, std::unordered_map<int, GlobalPrecision>{{__::s, 1}}); }
	
	constexpr PhysicsValue operator"" _mg(const long double n) { return PhysicsValue(n/1'000'000, std::unordered_map<int, GlobalPrecision>{{__::kg, 1}}); }
	constexpr PhysicsValue operator"" _g(const long double n) { return PhysicsValue(n/1'000, std::unordered_map<int, GlobalPrecision>{{__::kg, 1}}); }
	constexpr PhysicsValue operator"" _kg(const long double n) { return PhysicsValue(n, std::unordered_map<int, GlobalPrecision>{{__::kg, 1}}); }
	
	constexpr PhysicsValue operator"" _K(const long double n) { return PhysicsValue(n, std::unordered_map<int, GlobalPrecision>{{__::K, 1}}); }
	constexpr PhysicsValue operator"" _C(const long double n) { return PhysicsValue(n + 273.15, std::unordered_map<int, GlobalPrecision>{{__::K, 1}}); }
	
	constexpr PhysicsValue operator"" _A(const long double n) { return PhysicsValue(n, std::unordered_map<int, GlobalPrecision>{{__::A, 1}}); }
	constexpr PhysicsValue operator"" _kA(const long double n) { return PhysicsValue(n/1'000, std::unordered_map<int, GlobalPrecision>{{__::A, 1}}); }
	
	constexpr PhysicsValue operator"" _N(const long double n) { return PhysicsValue(n, std::unordered_map<int, GlobalPrecision>{{__::kg, 1}, {__::m, 1}, {__::s, -2}}); }
	constexpr PhysicsValue operator"" _kN(const long double n) { return PhysicsValue(n*1'000, std::unordered_map<int, GlobalPrecision>{{__::kg, 1}, {__::m, 1}, {__::s, -2}}); }
	constexpr PhysicsValue operator"" _MN(const long double n) { return PhysicsValue(n*1'000'000, std::unordered_map<int, GlobalPrecision>{{__::kg, 1}, {__::m, 1}, {__::s, -2}}); }
	
	constexpr PhysicsValue operator"" _Pa(const long double n) { return PhysicsValue(n, std::unordered_map<int, GlobalPrecision>{{__::kg, 1}, {__::m, -1}, {__::s, -2}}); }
	constexpr PhysicsValue operator"" _kPa(const long double n) { return PhysicsValue(n/1'000, std::unordered_map<int, GlobalPrecision>{{__::kg, 1}, {__::m, -1}, {__::s, -2}}); }
	constexpr PhysicsValue operator"" _MPa(const long double n) { return PhysicsValue(n/1'000'000, std::unordered_map<int, GlobalPrecision>{{__::kg, 1}, {__::m, -1}, {__::s, -2}}); }
	constexpr PhysicsValue operator"" _GPa(const long double n) { return PhysicsValue(n/1'000'000'000, std::unordered_map<int, GlobalPrecision>{{__::kg, 1}, {__::m, -1}, {__::s, -2}}); }
	
	constexpr PhysicsValue operator"" _J(const long double n) { return PhysicsValue(n, std::unordered_map<int, GlobalPrecision>{{__::kg, 1}, {__::m, 2}, {__::s, -2}}); }
	constexpr PhysicsValue operator"" _kJ(const long double n) { return PhysicsValue(n/1'000, std::unordered_map<int, GlobalPrecision>{{__::kg, 1}, {__::m, 2}, {__::s, -2}}); }
	constexpr PhysicsValue operator"" _MJ(const long double n) { return PhysicsValue(n/1'000'000, std::unordered_map<int, GlobalPrecision>{{__::kg, 1}, {__::m, 2}, {__::s, -2}}); }
	constexpr PhysicsValue operator"" _GJ(const long double n) { return PhysicsValue(n/1'000'000'000, std::unordered_map<int, GlobalPrecision>{{__::kg, 1}, {__::m, 2}, {__::s, -2}}); }
	
	constexpr PhysicsValue operator"" _W(const long double n) { return PhysicsValue(n, std::unordered_map<int, GlobalPrecision>{{__::kg, 1}, {__::m, 2}, {__::s, -3}}); }
	constexpr PhysicsValue operator"" _kW(const long double n) { return PhysicsValue(n/1'000, std::unordered_map<int, GlobalPrecision>{{__::kg, 1}, {__::m, 2}, {__::s, -3}}); }
	constexpr PhysicsValue operator"" _MW(const long double n) { return PhysicsValue(n/1'000'000, std::unordered_map<int, GlobalPrecision>{{__::kg, 1}, {__::m, 2}, {__::s, -3}}); }
	
	constexpr PhysicsValue operator"" _V(const long double n) { return PhysicsValue(n, std::unordered_map<int, GlobalPrecision>{{__::kg, 1}, {__::m, 2}, {__::s, -3}, {__::A, -1}}); }
	constexpr PhysicsValue operator"" _kV(const long double n) { return PhysicsValue(n/1'000, std::unordered_map<int, GlobalPrecision>{{__::kg, 1}, {__::m, 2}, {__::s, -3}, {__::A, -1}}); }
	
	constexpr PhysicsValue operator"" _ohm(const long double n) { return PhysicsValue(n, std::unordered_map<int, GlobalPrecision>{{__::kg, 1}, {__::m, 2}, {__::s, -3}, {__::A, -2}}); }
	constexpr PhysicsValue operator"" _kohm(const long double n) { return PhysicsValue(n/1'000, std::unordered_map<int, GlobalPrecision>{{__::kg, 1}, {__::m, 2}, {__::s, -3}, {__::A, -2}}); }
	constexpr PhysicsValue operator"" _Mohm(const long double n) { return PhysicsValue(n/1'000'000, std::unordered_map<int, GlobalPrecision>{{__::kg, 1}, {__::m, 2}, {__::s, -3}, {__::A, -2}}); }
	constexpr PhysicsValue operator"" _Gohm(const long double n) { return PhysicsValue(n/1'000'000'000, std::unordered_map<int, GlobalPrecision>{{__::kg, 1}, {__::m, 2}, {__::s, -3}, {__::A, -2}}); }


	struct Vector3 {
		GlobalPrecision x, y, z;
	};
	GlobalPrecision operator~ (Vector3 a) {
		return std::sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
	}
	Vector3 operator+ (Vector3 a, Vector3 b) {
		return Vector3(a.x + b.x, a.y + b.y, a.z + b.z);
	}
	Vector3 operator- (Vector3 a) {
		return Vector3(-a.x, -a.y, -a.z);
	}
	Vector3 operator- (Vector3 a, Vector3 b) {
		return a + (-b);
	}
	Vector3 operator* (Vector3 a, GlobalPrecision b) {
		return Vector3(a.x*b, a.y*b, a.z*b);
	}
	Vector3 operator/ (Vector3 a, GlobalPrecision b) {
		return Vector3(a.x/b, a.y/b, a.z/b);
	}
	Vector3 operator! (Vector3 a) {
		return a/~a;
	}
	


	struct PhysicsVector {
		Vector3 Centre, Vector;
		PhysicsValue Value;
	};

	GlobalPrecision operator~ (PhysicsVector a) {
		return a.Value.Scalar;
	}
	bool operator== (const PhysicsVector a, const PhysicsVector b) {
		return a.Value == b.Value;
	}
	PhysicsVector operator+ (const PhysicsVector a, const PhysicsVector b) {
		if (a == b) return PhysicsVector((a.Centre + b.Centre)/2, a.Vector + b.Vector, a.Value);
		throw std::logic_error("PhysicsVector Error: Unit type in addition doesn't match up.");
	}
	PhysicsVector operator+ (const PhysicsVector a) {
		return PhysicsVector(a.Centre, -a.Vector, a.Value);
	}
	PhysicsVector operator! (PhysicsVector a) {
		return PhysicsVector(a.Centre, !a.Vector, a.Value*~a.Vector);
	}


	struct Triangle {
		Vector3* a, b, c;
	};
	struct BVH_Node {
		Vector3 v0, v1;
		std::vector<BVH_Node*> BVH;
		std::vector<Triangle*> Tri;
		~BVH_Node() {
			for (BVH_Node* bvh : BVH) delete bvh;
			for (Triangle* tri : Tri) delete tri;
		}
	};
	struct PhysicsMaterial {
		PhysicsValue Density;
		float ElectricalConductivity, ThermalConductivity;
		float Hardness, Toughness, Malleability, Ductility;
		float YieldStrength;
	};
	class Object {
		bool ServerManaged;
		PhysicsValue Mass;
		Vector3 CentreOfMass;
		std::vector<Vector3> Vertices;
		BVH_Node* FirstBVH;
		std::vector<PhysicsVector> Forces;
		~Object() {
			delete FirstBVH;
		}
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