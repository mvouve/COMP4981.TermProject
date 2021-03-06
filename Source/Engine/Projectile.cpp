#include "Projectile.h"
#include "Event.h"
#include <iostream>
#include <cmath>
#include <map>
#include <SFML/System/Vector2.hpp>

using namespace Marx;

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: Projectile
--
-- DATE: March 15, 2015
--
-- REVISIONS: April 6, 2015
--
-- DESIGNER: Marc Vouve
--			Thomas Tallentire
--
-- PROGRAMMER: Marc Vouve
--				Thomas Tallentire
--
-- INTERFACE: Projectile::Projectile(SGO &_sprite, Map *map, Entity * e, float x, float y, Action * _act,
--				sf::Vector2f vector, Controller * ctrl = NULL,  float h = 1.0, float w = 1.0)
--
-- PARAMETERS: sprite - The sprite for the projectile
--				map	- The cell map to place the projectile in
--				e	- The entity that created the projectile
--				x	- X location on the cell map to place the projectile
--				y	- Y location on the cell map to place the projectile
--				act - Action to take when a collision occurs
--				vector	- The vector the projectile is moving in
--				ctrl - The controller for the projectile
--				h - Height
--				w - Width
--
-- RETURNS: void
--
-- NOTES:
--        Constructor for a Projectile
--
----------------------------------------------------------------------------------------------------------------------*/
Projectile::Projectile(SGO &_sprite, Map *map, Entity * e, float x, float y, Action * _act, sf::Vector2f vector, Controller * ctrl = NULL,  float h = 1.0, float w = 1.0) :
	VEntity(_sprite, map, x, y, ctrl, h, w), act(_act), heading(vector)
{
    std::cout << act << std::endl;
    float hy = hypot( vector.x , vector.y );
    heading = sf::Vector2f(vector.x / hy, vector.y / hy);
	shooter = e;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: move
--
-- DATE: March 15, 2015
--
-- REVISIONS: April 6, 2015
--
-- DESIGNER: Marc Vouve
--			Thomas Tallentire
--
-- PROGRAMMER: Marc Vouve
--				Thomas Tallentire
--
-- INTERFACE: Entity * Projectile::move(float x, float y, bool force = false)
--
-- PARAMETERS: x - X location to move the projectile to
--				y - Y location to move the projectile to
--				force - Whether or not to force the movement
--
-- RETURNS: Enity * - The Entity the projectile hits.
--
-- NOTES:
--        This function moves the projecile, if the projectile hits something, call onHit.
--
----------------------------------------------------------------------------------------------------------------------*/
/*Entity * Projectile::move(float x, float y, bool force = false)
{
    Entity *entity;

	entity = Entity::move(x, y, force);

	if (entity != nullptr)
	{
		if (onHit != NULL)
			onHit(shooter, entity);
	}


	return entity;
}*/

void Projectile::onCreate()
{
	drawable = true;
	Manager::ProjectileManager::dequeue(this);
}

void Projectile::onDestroy()
{
	VEntity::onDestroy();
    //getController()->addEvent(new MoveEvent(-1, -1, 1, 1, true));
	//drawable = false;

	Manager::ProjectileManager::enqueue(this);
}


void Projectile::onUpdate(float t)
{
	Entity *hit;

    if(TimeToLive > 0.0f)
    {
		if(top == -100)
			return;
        act->onUpdate(this, t);
        //std::cout << "X: " << left << "Y: " << top << "TimeToLive: " << TimeToLive << " Time Removed: " << t << std::endl;
        TimeToLive -= t;
    }
    else
    {
        onDestroy();
    }

    // Process events.
    std::vector<Marx::Event*>* eventQueue = getController()->getEvents();
    for(std::vector<Marx::Event*>::iterator it = eventQueue->begin(); it != eventQueue->end(); ++it )
    {
        switch((*it)->type)
        {
            case ::Marx::MOVE:
			{
                MoveEvent * ev = static_cast<MoveEvent*>(*it);
                sf::Vector2f vec(ev->getX(), ev->getY());
            	if ((hit = rMove( vec, t, true )) != nullptr)
				{
					if(hit != shooter)
					{
						if (Manager::ProjectileManager::getServer())
						{
							act->onHit(shooter, hit);

						}
						onDestroy();
					}
				}
			}
        }
    }
	getController()->clearEvents();

}

void Projectile::setTarget(sf::Vector2f t)
{
	heading = t;
}

void Projectile::setCurrentPos(float x, float y )
{
	this->left = x;
	this->top = y;
}

void Projectile::setAct(Action * act)
{
    TimeToLive = act->getTTL();	// Time to live must be updated within this class. Action should not change it's own time to live.
}

float Projectile::getTTL()
{
	return TimeToLive;
}


void Projectile::setTTL(float t)
{
	TimeToLive = t;
}

Entity* Projectile::getShooter()
{
	return shooter;
}

sf::Vector2f Projectile::getVector()
{
	return heading;
}
