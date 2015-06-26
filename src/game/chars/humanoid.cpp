/*
 * YARL - Yet another Roguelike
 * Copyright (C) 2015  Marko van Treeck <markovantreeck@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "humanoid.h"
#include "armor.h"
#include "world.h"

Humanoid::Humanoid(const Tile& t, int hp, int x, int y, double speed,
				   int visionRange,
				   const array<int, noOfAttributes>& attributes, World& world,
				   Attack* unarmed, const list<Item*>& inventory, int bab,
				   Size s, int naturalArmor) :
	Character(t, hp, x, y, speed, visionRange, attributes, world, unarmed,
			  inventory, bab, s, naturalArmor)
{

}

void Humanoid::attack(Entity* target)
{
	setLastTarget(target);
	target->setLastAttacker(this);

	Weapon* w1 = dynamic_cast<Weapon*>(_mainHand);
	Weapon* w2 = dynamic_cast<Weapon*>(_offHand);

	int toHitMod = bab() + attributeMod(strength) + size();

	if(w1)	// weapon in main hand
	{
		if(w2 && w2 != w1)	// two weapon fighting
		{
			toHitMod -= 6;
		}

		int hitRoll = rand() % 20 + 1;
		if(hitRoll == 20 ||	// natural 20 is a hit, nat. 1 a miss
			(hitRoll != 1 && hitRoll + toHitMod >= target->armorClass()))
		{
			world().statusBar().
				addMessage(attackMessage(target, true, w1));

			int damage = w1->damage() + attributeMod(strength);

			// wielded in both hands
			if(w1 == w2 && attributeMod(strength) > 0)
				damage += attributeMod(strength) / 2;

			if(damage <= 0)
				damage = 1;

			target->doDamage(damage);
		}
		else	// miss
			world().statusBar().
				addMessage(attackMessage(target, false, w1));
	}

	if(w2 && w1 != w2)
	{
		toHitMod -= 4;
		int hitRoll = rand() % 20 + 1;
		if(hitRoll == 20 ||	// natural 20 is a hit, nat. 1 a miss
			(hitRoll != 1 && hitRoll + toHitMod >= target->armorClass()))
		{
			world().statusBar().
				addMessage(attackMessage(target, true, w1));

			int damage = w2->damage();

			// only half the strength bonus is added for off hand
			if(attributeMod(strength) > 0)
				damage += attributeMod(strength) / 2;
			else	// strength malus
				damage += attributeMod(strength);

			target->doDamage(damage);
		}
		else
			world().statusBar().
				addMessage(attackMessage(target, false, w1));
	}

	else if(!w1 && !w2)
	{
		world().statusBar().addMessage(attackMessage(target, false));
	}
}

int Humanoid::armorClass()
{
	int ac = Character::armorClass();

	Armor* s = dynamic_cast<Armor*>(_mainHand);
	if(s && s->isShield())
	{
		ac +=  s->ac();
	}

	Armor* s2 = dynamic_cast<Armor*>(_offHand);
	if(s2 && s2->isShield() && s2 != s)
	{
		ac += s2->ac();
	}

	return ac;
}

int Humanoid::attributeMod(Character::Attribute attribute)
{
	int bonus = (_attributes[attribute] - 10) / 2;

	if(attribute == dexterity || attribute == strength)
	{
		if(armor())
			bonus += armor()->checkPenalty();

		Armor* s1 = dynamic_cast<Armor*>(_mainHand);
		Armor* s2 = dynamic_cast<Armor*>(_offHand);
		if(s1 && s1->isShield())
			bonus += s1->checkPenalty();
		if(s2 && s1 != s2 && s2->isShield())
			bonus += s2->checkPenalty();

		bonus += loadCheckPenalty();


		if(attribute == dexterity)
		{
			if(armor() && bonus > armor()->maxDexBon())
				bonus = armor()->maxDexBon();

			if(s1 && s1->isShield() && bonus > s1->maxDexBon())
				bonus = s1->maxDexBon();
			if(s2 && s2->isShield() && bonus > s2->maxDexBon())
				bonus = s2->maxDexBon();

			if(bonus > loadMaxDexBon())
				bonus = loadMaxDexBon();
		}
	}

	return bonus;
}

Item* Humanoid::mainHand()
{
	return _mainHand;
}

Item* Humanoid::offHand()
{
	return _offHand;
}

void Humanoid::setMainHand(Item* i)
{
	_mainHand = i;
}

void Humanoid::setOffHand(Item* i)
{
	_offHand = i;
}
