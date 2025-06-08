#include "genCode.h"

std::string Entity::getEntityName()
{
	switch (maxHealth) {
	case 50:
		return "Infected";
	case 100:
		return "Spitter";
	case 250:
		return "Hunter/Smoker";
	case 325:
		return "Jockey";
	case 600:
		return "Charger";
	case 1000:
		return "Witch";
	case 4000:
		return "Tank";
	default:
		return "Unknown Entity";
	}
}