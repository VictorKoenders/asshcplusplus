#include <iostream>
#include <vector>
#include <optional>
#include <memory>
#include <sstream>

class IItem {
public:
	virtual ~IItem() {}
};

class Resource {
public:
	virtual ~Resource() {}
};

class EnergyResource : public Resource {

};

class SteelResource : public Resource {

};

typedef float(*level_func)(int);

template <class TResource>
class RunningCost {
private:
	level_func func;
public:
	RunningCost(level_func func) : func(func) {
	}
	float amount(int level) const {
		return func(level);
	}
};

template <class TResource>
class Production {
private:
	level_func func;
public:
	Production(level_func func) : func(func) {
	}
	float amount(int level) const {
		return func(level);
	}
};
class Building : public IItem {
public:
	std::string Name;

	Building(std::string name) : Name(name)
	{
	}

	std::string info() const {
		std::stringstream stream;
		stream << this->Name;

		if (const RunningCost<EnergyResource>* cost = dynamic_cast<const RunningCost<EnergyResource>*>(this)) {
			stream << std::endl << "- Running cost (energy): " << cost->amount(1);
		}
		if (const RunningCost<SteelResource>* cost = dynamic_cast<const RunningCost<SteelResource>*>(this)) {
			stream << std::endl << "- Running cost (steel): " << cost->amount(1);
		}
		if (const Production<EnergyResource>* prod = dynamic_cast<const Production<EnergyResource>*>(this)) {
			stream << std::endl << "- Production (energy): " << prod->amount(1);
		}
		if (const Production<SteelResource>* prod = dynamic_cast<const Production<SteelResource>*>(this)) {
			stream << std::endl << "- Production (steel): " << prod->amount(1);
		}
		return stream.str();
	}
};

constexpr float steel_plant_energy_cost(int level) {
	return level * 30.f;
}

constexpr float steel_plant_production(int level) {
	return level * 30.f;
}

class SteelPlant :
	public Building,
	public RunningCost<EnergyResource>,
	public Production<SteelResource> {
public:
	SteelPlant() :
		Building("Steel plant"),
		RunningCost<EnergyResource>(steel_plant_energy_cost),
		Production<SteelResource>(steel_plant_production)
	{
	}
};

class Planet {
public:
	std::vector<std::unique_ptr<IItem>> items;

	template <typename T>
	std::optional<const T*> get() const {
		for (const std::unique_ptr<IItem>& item : items) {
			if (const T* t = dynamic_cast<const T*>(item.get())) {
				return t;
			}
		}
		return {};
	}
};

int main() {
	Planet planet;
	if (auto building = planet.get<Building>()) {
		std::cout << (*building)->info();
	}
	else {
		std::cout << "Could not find building" << std::endl;
	}
	planet.items.push_back(std::make_unique<SteelPlant>());
	if (auto building = planet.get<Building>()) {
		std::cout << (*building)->info();
	}
	else {
		std::cout << "Could not find building" << std::endl;
	}

	std::getchar();
	return 0;
}
