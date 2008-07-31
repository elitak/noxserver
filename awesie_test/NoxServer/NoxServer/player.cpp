#include "player.h"
#include "socket_mgr.h"
#include "world.h"
#include "world_packet.h"
#include "world_session.h"

#include <cmath>

player::player(world_session& session) : _session(session), unit(0x2c9), m_moving_timer(0)
{
	memset(&m_player_info, 0, sizeof(m_player_info));
	m_player_info.flag = 0x20;
}

void player::update_view(bool all)
{
	b2AABB aabb;

	if(!m_body)
		return;

	aabb.lowerBound.Set(m_body->GetPosition().x - 30, m_body->GetPosition().y - 25);
	aabb.upperBound.Set(m_body->GetPosition().x + 30, m_body->GetPosition().y + 25);
	
	update_view(aabb);
}
void player::update_view(b2AABB& aabb)
{
	// basically, the whole can't see objects behind walls thing
	// makes it so that we have to scan over the entire screen region
	// and do a ray cast against all walls (exclude short/windows) and all
	// objects with class obstacle

	const uint32 max_shapes = 512;
	b2Shape *shapes_buffer[512];
	int actual_shapes = world::instance->get_the_world().Query(aabb, shapes_buffer, max_shapes);

	object_map new_view;

	for(int i = 0; i < actual_shapes; i++)
	{
		object* o = (object*)shapes_buffer[i]->GetUserData();
		
		// probably a wall
		if(o == NULL)
			continue;

		if(!o->is_static())
		{
			bool new_object;
			object_map::iterator iter = m_current_view.find(o->get_extent());

			if(iter == m_current_view.end())
			{
				new_object = true;
			}
			else
			{
				// remove from current view
				m_current_view.erase(iter);
				// if object was updated, then put in queue
				if(o->is_dirty())
					new_object = true;
				else
					new_object = false;
			}
			
			b2Segment segment;
			segment.p1.Set(this->get_position_x(), this->get_position_y());
			segment.p2.Set(o->get_position_x(), o->get_position_y());

			bool behind_wall = false;

			// now do a ray cast
			// caution: this is close to n-squared run-time
			for(int j = 0; j < actual_shapes; j++)
			{
				object* obstacle = (object*)shapes_buffer[j]->GetUserData();

				// probably a wall
				if(obstacle == NULL || obstacle->get_object_info()->classes & CLASS_OBSTACLE)
				{
					float lambda;
					b2Vec2 normal;
					if(shapes_buffer[j]->TestSegment(shapes_buffer[j]->GetBody()->GetXForm(), &lambda, &normal, segment, 1.0f))
					{
						behind_wall = true;
						break;
					}
				}
			}

			if(!behind_wall)
			{
				// add to new view
				new_view[o->get_extent()] = o;

				if(new_object)
					m_update_queue.insert(o);
			}
		}
	}

	// everything that is left in m_current_view, is no longer in view
	// and should be taken out
	BOOST_FOREACH(object_map::value_type p, m_current_view)
	{
		world_packet packet(MSG_OBJECT_OUT_OF_SIGHT);
		// send object extent
		packet << (uint16)p.first;
		get_session().send_packet(packet);
	}

	// new_view is now the current view
	new_view.swap(m_current_view);
}

void player::update_player()
{
	// we don't really need to call update_view every frame, do we?
	static bool _update_view = false;
	_update_view = !_update_view;

	if(_update_view)
		update_view();

	SendUpdatePacket();
}
void player::update(uint32 diff)
{
	// call base update
	unit::update(diff);

	// update all timers
	if(m_moving_timer > diff)
		m_moving_timer -= diff;
	else if(m_moving_timer > 0)
		stop();
}
void player::stop()
{
	m_moving_timer = 0;

	if(m_body)
		m_body->SetLinearVelocity(b2Vec2(0, 0));
}
void player::attack()
{
	if(get_session().is_observing())
		get_session().set_observing(false);
}

bool player::dequip(object *obj)
{
	if(!unit::dequip(obj))
	{
		return false;
	}

	world_packet packet(MSG_REPORT_DEQUIP);
	packet << (uint16)obj->get_extent();
	get_session().send_packet(packet);

	return true;
}

bool player::equip(object *obj)
{
	if(!unit::equip(obj))
	{
		return false;
	}

	world_packet packet(MSG_REPORT_EQUIP);
	packet << (uint16)obj->get_extent();
	get_session().send_packet(packet);

	return true;
}

bool player::drop(object *obj, uint32 max_dist, float x, float y)
{
	InventoryType::iterator iter = m_inventory.find(obj);
	if(iter == m_inventory.end())
		return false;

	//dequip
	//caution: we may want to reconsider this based on how dequip is coded
	//if(!dequip(obj))
	//	return false;
	dequip(obj);
	
	//put x,y within max_dist
	float delta_x = x - this->get_position_x();
	float delta_y = y - this->get_position_y();

	float magnitude = sqrt(delta_x*delta_x + delta_y * delta_y);
	if(max_dist > 0 && magnitude > max_dist)
	{
		x = ((delta_x / magnitude) * max_dist) + this->get_position_x();
		y = ((delta_y / magnitude) * max_dist) + this->get_position_y();
	}

	// we don't care what this returns since it will set x,y to
	// either collision point or to the original point
	this->can_see_point(x, y, &x, &y);

	if(unit::drop(obj, x, y))
	{
		world_packet packet(MSG_REPORT_DROP);
		packet << (uint16)obj->get_extent();
		packet << (uint16)0;
		get_session().send_packet(packet);

		return true;
	}
	else
		return false;
}

bool player::pickup(object *obj, uint32 max_dist)
{
	if(obj->is_in_inventory())
		return false;

	float delta_x = obj->get_position_x() - this->get_position_x();
	float delta_y = obj->get_position_y() - this->get_position_y();

	float lensq = delta_x*delta_x + delta_y*delta_y;

	if(max_dist > 0 && lensq > (max_dist*max_dist))
		return false;

	// we have to do this before the pickup handlers get called and equip items
	// what is a better place to call the handler?
	world_packet packet(MSG_REPORT_MODIFIABLE_PICKUP);
	packet << (uint16)obj->get_extent();
	packet << (uint16)obj->get_type();
	packet << (uint32)0xFFFFFFFF;

	get_session().send_packet(packet);

	if(unit::pickup(obj))
	{
		return true;
	}
	else
		return false;
}

void player::jump()
{
}

void player::laugh()
{
}

void player::running_jump()
{
}

void player::move_towards(float x, float y, float speed)
{
	if(!m_body)
		return;

	m_moving_timer = 100;

	float cur_x = get_position_x();
	float cur_y = get_position_y();

	float delta_x = x - cur_x;
	float delta_y = y - cur_y;

	float magnitude = sqrt(delta_x*delta_x + delta_y*delta_y);

	float unit_x = delta_x/magnitude;
	float unit_y = delta_y/magnitude;

	speed = speed * SCALING_FACTOR;
	m_body->SetLinearVelocity(b2Vec2(unit_x * speed, unit_y * speed));
}

void player::run()
{
	move_towards(get_session().m_mouse_x, get_session().m_mouse_y, 120.0f);
}

void player::taunt()
{
}

void player::point()
{
}

void player::equip_secondary(object *obj)
{
}

bool player::is_ability_ready(uint8 ability)
{
	return true;
}
void player::_BuildUpdatePacket(world_packet& packet)
{
	packet.set_opcode(MSG_UPDATE_STREAM);
	if(packet.size()) // If we are the first in the stream, then we can't/don't need this first byte
		packet << (uint8)0x00;  // This is going to be here until we figure a way to make efficient use of relative coords.
	packet << (uint8)0xFF;
	packet << (uint16)get_extent();
	packet << (uint16)get_type();
	packet << (uint16)get_position_x();
	packet << (uint16)get_position_y();
	packet << (uint8)get_angle(); // we need to OR this with 0x80 depending on the m_action
	packet << (uint8)m_action;
}

void player::_BuildNewPlayerPacket(world_packet& packet)
{
	packet.initialize(MSG_NEW_PLAYER, 0x80, packet.get_endpoint(), 0x80);
	packet << (uint16)get_extent();
	packet.append((uint8*)(&m_player_info), 0x7E);
}
void player::_BuildClientStatusPacket(world_packet& packet)
{
	packet.initialize(MSG_REPORT_CLIENT_STATUS, 0x0, packet.get_endpoint(), 6);
	packet << (uint16)get_extent();
	packet << (uint32)get_session().is_observing();
}

void player::_BuildResetAbilityPacket(world_packet& packet, uint8 ability)
{
	packet.initialize(MSG_RESET_ABILITIES, 0, packet.get_endpoint());
	packet << (uint8)ability;
}
void player::_BuildAbilityStatePacket(world_packet& packet, uint8 ability)
{
	packet.initialize(MSG_REPORT_ABILITY_STATE, 0, packet.get_endpoint());
	packet << (uint8)ability;
	packet << (uint8)(is_ability_ready(ability));
}
void player::SendUpdatePacket()
{
	world_packet packet(MSG_UPDATE_STREAM, 0, get_session().get_endpoint());
	// every object that starts stream must have the two bytes after the x,y
	/*packet << (uint8)0xFF;
	packet << (uint8)0x00;
	packet << (uint8)0x00;
	packet << (uint8)0x00;
	packet << (uint8)0x00;
	packet << (uint16)3000;
	packet << (uint16)3000;
	packet << (uint8)0;
	packet << (uint8)0;*/

	m_update_queue.erase(this);
	// Always update ourselves, we have to be first due to reasons above
	_BuildUpdatePacket(packet);

	for(update_queue::iterator iter = m_update_queue.begin(); iter != m_update_queue.end(); ++iter)
		if(*iter)
			(*iter)->_BuildUpdatePacket(packet);
	m_update_queue.clear();

	// Three zeroes denotes end of MSG_UPDATE_STREAM
	packet << (uint8)0x00;
	packet << (uint8)0x00;
	packet << (uint8)0x00;

	get_session().send_packet(packet);
}

void player::respawn()
{
	// Set respawn states for the player
     uint16 type;
	 object* item;
	/*switch(plrInfo.pclass)
	{
	case PLAYER_CLASS_WARRIOR:
          {
		        ResetAbilityDelays();
				    m_health = sGameConfig.GetFloatDefault("WarriorMaxHealth",150);
                    m_mana = sGameConfig.GetFloatDefault("WarriorMaxMana",0);
				    m_speed = sGameConfig.GetFloatDefault("WarriorMaxSpeed",21000);
                    m_strength = sGameConfig.GetFloatDefault("WarriorMaxStrength",125);
                    m_weight = 3000;

                    type = sThingBin.Thing.Object.GetIndex("Longsword");
                    Pickup(NewPickup(type));
					break;
          }

	case PLAYER_CLASS_CONJURER: 
					m_health = sGameConfig.GetFloatDefault("ConjurerMaxHealth",100);
                         m_mana = sGameConfig.GetFloatDefault("ConjurerMaxMana",150);
					m_speed = sGameConfig.GetFloatDefault("ConjurerMaxSpeed",18500);
                         m_strength = sGameConfig.GetFloatDefault("ConjurerMaxStrength",125);
                         m_weight = 3000;
                         type = sThingBin.Thing.Object.GetIndex("StreetShirt");
                         Pickup(NewPickup(type));
					break;

	case PLAYER_CLASS_WIZARD: 
					m_health = sGameConfig.GetFloatDefault("WizardMaxHealth",75);
                         m_mana = sGameConfig.GetFloatDefault("WizardMaxMana",125);
					m_speed = sGameConfig.GetFloatDefault("WizardMaxSpeed",17500);
                         m_strength = sGameConfig.GetFloatDefault("WizardMaxStrength",125);
                         m_weight = 3000;

                         type = sThingBin.Thing.Object.GetIndex("StreetShirt");
                         if(type)
                              Pickup(NewPickup(type));
                         type = sThingBin.Thing.Object.GetIndex("WizardRobe");
                         if(type)
                              Pickup(NewPickup(type));
					break;
	default:break;
	}
     type = sThingBin.Thing.Object.GetIndex("StreetPants");
     if(type)
          Pickup(NewPickup(type));
     type = sThingBin.Thing.Object.GetIndex("StreetSneakers");
     if(type)
          Pickup(NewPickup(type));
     m_max_speed = m_speed;
     m_max_mana = m_mana;
	m_max_health = m_health;

	// Get a random spawn point and set position to that
	SetPosition(GridPair(3000, 2900));
	ForceUpdateAll();

	ResetEnchants();
	SetEnchant(ENCHANT_INVULNERABLE);*/

	set_position(3000, 2900);

	m_health = m_max_health = 150;

	get_session()._SendPlayerRespawnOpcode();
	world_packet packet;
	_BuildTotalHealthPacket(packet);
	socket_mgr::instance->get_world_socket().send_to_all(packet);
    _BuildTotalManaPacket(packet);
    socket_mgr::instance->get_world_socket().send_to_all(packet);
	_BuildStatsPacket(packet);
	socket_mgr::instance->get_world_socket().send_to_all(packet);

	_BuildDequipPacket(packet, true, 0xFFFFFFFF);
	get_session().send_packet(packet);
	_BuildDequipPacket(packet, false, 0xFFFFFFFF);
	get_session().send_packet(packet);

	type = ThingBin::instance->Thing.Object.GetIndex("StreetPants");
	if(type)
	{
		item = new world_object(type);
		object_mgr::instance->add_object(item);

		pickup(item, 0);
	}
	type = ThingBin::instance->Thing.Object.GetIndex("StreetSneakers");
	if(type)
	{
		item = new world_object(type);
		object_mgr::instance->add_object(item);

		pickup(item, 0);
	}
}
void player::_BuildHealthPacket(world_packet &packet)
{
	packet.initialize(MSG_REPORT_HEALTH);
	packet << get_extent();
	packet << (uint8)(m_health);
}
void player::_BuildMyHealthPacket(world_packet &packet)
{
	packet.initialize(MSG_REPORT_PLAYER_HEALTH);
	packet << (uint16)m_health;
}
void player::_BuildTotalHealthPacket(world_packet &packet)
{
	packet.initialize(MSG_REPORT_TOTAL_HEALTH);
	packet << get_extent();
	packet << (uint16)m_health;
	packet << (uint16)m_max_health;
}
void player::_BuildManaPacket(world_packet &packet)
{
     packet.initialize(MSG_REPORT_MANA);
     packet << get_extent();
     packet << (uint16)(m_mana);
}
void player::_BuildTotalManaPacket(world_packet &packet)
{
     packet.initialize(MSG_REPORT_TOTAL_MANA);
     packet << get_extent();
     packet << (uint16)m_mana;
     packet << (uint16)m_max_mana;
}
void player::_BuildStatsPacket(world_packet &packet)
{
	packet.initialize(MSG_REPORT_STATS);
	packet << get_extent();
	packet << (uint16)m_health;
	packet << (uint16)m_mana; //mana
	packet << (uint16)m_weight; //weight
	packet << (uint16)m_speed; //speed
	packet << (uint16)m_strength; //strength
	packet << (uint8)10; //level
}