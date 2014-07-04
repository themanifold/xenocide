#include "mem_check.h"

#include "items.h"
#include "types.h"
#include "global.h"
#include "map.h"
#include "options.h"
#include "level.h"
#include "parser.h"
#include "tutorial.h"
#include "system.h"
#include "sounds.h"

#include <math.h>
#include <list>
using namespace std;

#include <assert.h>

extern CMyScreen screen;
extern COptions options;
extern CDescriptions descriptions;
extern DEFINITIONS definitions;
extern CLevel level;
extern CTutorial tutorial;
extern CSounds sounds;

/*
  activities - DESCRIPTION
  g - get
  b - build robot
  d - drop
  t - throw
  w - wield
  L - load ammo (can be loaded with ammo)
  N - uNload ammo (some items can have load ammo but not option to unload)
  A - armor (wear - put on)
  u - use
  a - activate f.e. grenade
  r - ready
  m - mend
  p - program CPU
  
  // properties
  
  l - loadable (for ammo)
  i - countable - for packages
  
  

*/
bool CItem::property_activate()
{
  if (this->activities.find('a')!=-1)
    return true;
  return false;
}

bool CItem::property_program()
{
	if (this->activities.find('p')!=-1)
		return true;
	return false;
}

bool CItem::property_drop()
{
  if (this->activities.find('d')!=-1)
    return true;
  return false;
}

bool CItem::property_buildrobot()
{
	if (this->activities.find('b')!=-1)
		return true;
	return false;
}

bool CItem::property_throw()
{
  if (this->activities.find('t')!=-1)
    return true;
  return false;
}

bool CItem::property_wield()
{
  if (this->activities.find('w')!=-1)
    return true;
  return false;
}

bool CItem::property_use()
{
	if (this->activities.find('u')!=-1)
		return true;
	return false;
}

bool CItem::property_controller() // item sterujacy
{
  if (this->activities.find('!')!=-1)
    return true;
  return false;
}

bool CItem::property_load_ammo()
{
  if (this->activities.find('L')!=-1)
    return true;
  return false;
}

bool CItem::property_unload_ammo()
{
  if (this->activities.find('N')!=-1)
    return true;
  return false;
}

bool CItem::property_put_on()
{
  if (this->activities.find('A')!=-1)
    return true;
  return false;
}

bool CItem::property_to_load()
{
  if (this->activities.find('l')!=-1)
    return true;
  return false;
}

bool CItem::property_join()
{
  if (this->activities.find('i')!=-1)
    return true;
  return false;
}

bool CItem::property_get()
{
  if (this->activities.find('g')!=-1)
    return true;
  return false;
}

bool CItem::property_ready()
{
	if (this->activities.find('r')!=-1)
		return true;
	return false;
}

bool CItem::property_mend()
{
	if (this->activities.find('m')!=-1)
		return true;
	return false;
}


bool CItem::ChangePosition(int x, int y)
{
	if (pX()==x && pY()==y)
		return false;

	if (on_ground)
	{
		level.map.DecraseNumberOfItems(pX(),pY());
		level.map.IncraseNumberOfItems(x,y);
	}
	CTile::ChangePosition(x,y);
	return true;
}

bool GAS::ChangePosition(int x, int y)
{
	if (pX()==x && pY()==y)
		return false;
	
	level.map.DecraseNumberOfGases(pX(),pY());
	level.map.IncraseNumberOfGases(x,y);
	CTile::ChangePosition(x,y);
	return true;
}

bool CItem::IsDead()
{
	return is_dead;
}

bool CItem::IsLoaded()
{
	if (property_load_ammo())
	{
		CRangedWeapon *ranged_weapon = IsShootingWeapon();  
		if (ranged_weapon!=NULL)
		{
			if (ranged_weapon->ammo.quantity==0)
				return false;
		}
	}
	return true;
}


CHandWeapon * CItem::IsHandWeapon()
{
	return dynamic_cast<CHandWeapon *>(this);
};

CRangedWeapon * CItem::IsShootingWeapon()
{
	return dynamic_cast<CRangedWeapon *>(this);
}

CGrenade * CItem::IsGrenade()
{
	return dynamic_cast<CGrenade *>(this);
}

CPill * CItem::IsPill()
{
	return dynamic_cast<CPill *>(this);
}

CAmmo * CItem::IsAmmo()
{
	return dynamic_cast<CAmmo *>(this);
}

CBaseArmor * CItem::IsArmor()
{
	return dynamic_cast<CBaseArmor *>(this);
}

CCorpse * CItem::IsCorpse()
{
	return dynamic_cast<CCorpse *>(this);
}

CTrash * CItem::IsGarbage()
{
	return dynamic_cast<CTrash *>(this);
}

CRepairKit * CItem::IsRepairSet()
{
	return dynamic_cast<CRepairKit *>(this);
}

CProgrammator * CItem::IsProgrammator()
{
	return dynamic_cast<CProgrammator *>(this);
}

CBattery * CItem::IsBattery()
{
	return dynamic_cast<CBattery *>(this);
}

CCountable * CItem::IsCountable()
{
	return dynamic_cast<CCountable *>(this);
}

CProcessor * CItem::IsRobotCPU()
{
	return dynamic_cast<CProcessor *>(this);
}

CRobotLeg * CItem::IsRobotLeg()
{
	return dynamic_cast<CRobotLeg *>(this);
}

CRobotShell * CItem::IsRobotShell()
{
	return dynamic_cast<CRobotShell *>(this);
}



CItem::CItem()
{
	ClassName = "ITEM";

   category = 0;
   color = 7;   
   tile = '?';

   DMG=0;
   HIT=0;
   DEF=0;
   on_ground = false;
   energy_activated = -1;
   activities="dt"; // drop throw
   inventory_letter='*';
   purpose = PURPOSE_NONE;   
   weight=1;
   required_strength = 1;
   price=0;
   owner=NULL;
   in_robot_shell=NULL;
   properties = TYPE_NORMAL;
   is_dead=false;
   hit_points = 1;
   max_hit_points = 1;
   skill_to_use=SKILL_MELEE_WEAPONS;
}

CItem::~CItem()
{
	if (on_ground==true) // lays on ground
	{
		level.map.DecraseNumberOfItems(pX(),pY());
	}
}

CAmmo::~CAmmo()
{
}


CBattery::CBattery()
{
	ClassName = "BATTERY";
	tile = '$';
	activities+="g";
	max_capacity=0;
	regeneration_speed=0;
	capacity=0;	
}	

int CItem::uses_energy(bool by_activation) // 0 - no changes, 1 - turned self off
{
	if (by_activation && energy_activated!=1)
		return 0;

	if (!by_activation && !(properties&TYPE_ENERGY))
		return 0;

	// if item can by powered

	if (owner==NULL)
	{
		this->turn_energy_off();
		return 1;
	}

	if (owner->use_energy(1)==false)
	{
		this->turn_energy_off();
		if (this->owner == level.player)
		{
			screen.console.add(string("Your ") + this->show_name() + " is out of power!",13);
		}			
		return 1;
	}
	return 0;	
}

int CItem::evaluate_item_for_h2h_combat()
{
	int value;

	value = HIT*3 + DEF*3 + DMG;
	value *= 10;
	if (properties&TYPE_ARMOR_PERCING)
		value*=2;
	if (properties&TYPE_RADIOACTIVE)
		value=(value*3)/2;
	if (properties&TYPE_ELECTRIC)
		value=(value*3)/2;
	if (properties&TYPE_ENERGY)
		value=(value*3)/2;
	if (properties&TYPE_CHEM_POISON)
		value=(value*3)/2;
	if (properties&TYPE_PARALYZE)
		value=(value*3)/2;
	if (properties&TYPE_STUN)
		value=(value*3)/2;
	
	return value;
}

int CRangedWeapon::evaluate_item_for_ranged_combat()
{
	int value;
	value = (PWR + ammo.PWR)*20 + (ACC + ammo.ACC)*10;

	if (ammo.properties&TYPE_ARMOR_PERCING)
		value*=2;
	if (ammo.properties&TYPE_ELECTRIC)
		value=(value*3)/2;
	if (ammo.properties&TYPE_ENERGY)
		value=(value*3)/2;
	if (ammo.properties&TYPE_RADIOACTIVE)
		value=(value*3)/2;
	if (ammo.properties&TYPE_CHEM_POISON)
		value=(value*3)/2;
	if (ammo.properties&TYPE_PARALYZE)
		value=(value*3)/2;
	if (ammo.properties&TYPE_STUN)
		value=(value*3)/2;

	return value;	
}


string CItem::article_a()
{
   char begin;
   string vowels=string("aeiouyAEIOUY");
   string digits=string("0123456789");
   begin=*show_name().c_str();
   if (digits.find(begin)!=-1)
   {
     return ("");
   }
   
   if (vowels.find(begin)!=-1)
   {
     return string("an ");
   }
   else
     return string("a ");
}

string CItem::article_the()
{
   char begin;
   string digits=string("0123456789");
   begin=*show_name().c_str();
   if (digits.find(begin)!=-1)
   {
     return string("");
   }
   return string("the ");
}


void CItem::print_item_with_letter(int x,int y,string additional_text)
{
       string text;
       text="[ ] x "+ this->show_name();
       print_text(x,y,text+additional_text);
       set_color(14);
       print_character(x+1,y,this->inventory_letter);
	   this->draw_at(x+4,y);
}

// enemy - our target

bool CRangedWeapon::fire_missile(int x1, int y1, int x2, int y2,CMonster * enemy, int distance)
{
	struct CPosition position[100];
	int cells, index;
	int tile_hit;
	int val;
	CMonster *monster;
	bool terrain_hit;
	string descript;

	// use energy if weapon is activated (like laser scopes etc.)
	this->uses_energy(); 

	int power_multiplyer=1;

	if (properties&TYPE_ENERGY)
	{
		switch (fire_type)
		{
		case FIRE_DOUBLE:
			power_multiplyer=2;
			break;
		case FIRE_TRIPLE:
			power_multiplyer=3;
			break;
		case FIRE_BURST:
			power_multiplyer=6;
			break;
		}
	}
	else
		ammo.quantity--;

	cells=generate_bresenham_line(x1, y1, x2, y2, (CPosition *) position,100);
	int hit_power,hit_power_modifier;

	if (properties&TYPE_ENERGY)
		hit_power=random( this->PWR )/2 + this->PWR/2;
	else
		hit_power=random( this->PWR + this->ammo.PWR )/2 + this->ammo.PWR;

	hit_power*=power_multiplyer;
	terrain_hit=false;

	for (index=1;index<99;index++)
	{
		if (properties&TYPE_ENERGY)
		{
			set_color(this->color);
			int hx=position[index].x;
			int hy=position[index].y;
			bool was_seen=false;
			if (owner==level.player || level.map.seen_by_player(hx,hy))
			{
				if (owner==level.player)
					level.map.setSeenByPlayer(hx,hy);
				print_character(hx,hy,'*'); 
				myrefresh();
			}
		}
		if (index>=distance)
			terrain_hit=true;

		// HIT THE CELL
		if (level.map.blockMissile(position[index].x,position[index].y) || terrain_hit)
		{
			set_color(14);

			// get random item
			int counter,randomized;
			ptr_list on_cell; ptr_list::iterator m;
			level.list_of_items_from_cell(&on_cell,position[index].x,position[index].y);
			CItem *przedm; przedm=NULL;
			randomized=random(on_cell.size()+1);
			if (randomized!=0)
				for (m=on_cell.begin(),counter=0;counter<randomized;m++,counter++)
					przedm=(CItem *)*m;
			on_cell.clear();
			// hit the item
			if (przedm!=NULL)
			{
				przedm->damage_it(random(hit_power)/2+hit_power/2);
			}

			// draw hit
			if (level.map.blockMissile(position[index].x,position[index].y))
				tile_hit=level.map.getTile(position[index].x,position[index].y);
			else
				tile_hit='*';
			// damage terrain
			level.map.damage(position[index].x,position[index].y,random(hit_power)/2+hit_power/2);

			if (level.map.seen_by_player(position[index].x,position[index].y) ||
				level.map.seen_by_camera(position[index].x,position[index].y) )
			{
				print_character(position[index].x,position[index].y,tile_hit); // !!!
				myrefresh();
			}
			return false;
		}

		monster=level.monster_on_cell(position[index].x,position[index].y);
		// hit the monster?
		if (monster!=NULL)
		{      
			if (monster==enemy || enemy==NULL || enemy==this->owner) 
			{
				if (monster->evade_missile(NULL)==false || enemy==this->owner)
				{
					descript = ammo.name + " hits " + monster->name;
					descriptions.add_attack("HIT",descript);
					val = monster->hit_by_item(hit_power, (CItem *)&ammo);
					if (val>0)
						descriptions.add_attack("HIT"," for " + IntToStr(val) + " damage");
					else if (val==0)
					{
						string text = descriptions.get_attack("HIT");
						if (text.find("damaging")==-1)
							descriptions.add_attack("HIT"," without damage");
					}
					else
						descriptions.add_attack("HIT"," killing it");
					if (level.map.seen_by_player(position[index].x,position[index].y) ||
						level.map.seen_by_camera(position[index].x,position[index].y))
					{
						set_color(4);
						print_character(position[index].x,position[index].y,'*'); // !!!
						myrefresh();
					}
					if (!(properties&TYPE_ENERGY)) // energy goes further
						return true;
				}
			}
			if (monster!=enemy) // shooting above/by this one
			{
				if (enemy==NULL || lower_random(monster->size, monster->size + enemy->size*2)) // niestety, trafiony ten
				{
					if (monster->evade_missile(NULL)==false)
					{
						descript = ammo.name + " hits " + monster->name;
						descriptions.add_attack("HIT",descript);
						val = monster->hit_by_item(hit_power, (CItem *)&ammo);
						if (val>0)
							descriptions.add_attack("HIT"," for " + IntToStr(val) + " damage");
						else if (val==0)
						{
							string text = descriptions.get_attack("HIT");
							if (text.find("damaging")==-1)
								descriptions.add_attack("HIT"," without damage");
						}
						else
							descriptions.add_attack("HIT"," killing");

						if (level.map.seen_by_player(position[index].x,position[index].y) ||
							level.map.seen_by_camera(position[index].x,position[index].y))
						{
							set_color(4);
							print_character(position[index].x,position[index].y,'*'); // !!!
							myrefresh();
						}
						if (!(properties&TYPE_ENERGY)) // energy goes further
							return false;
					}
				}
			}
		}
	}
	return false;
}

void CRangedWeapon::print_item_with_letter(int x,int y,string additional_text)
{
       if (ammo.quantity>0)
         CItem::print_item_with_letter(x,y," <" + ammo.show_name() + "> " + additional_text);
       else
         CItem::print_item_with_letter(x,y, additional_text);
}

string CItem::show_name()
{
	if (level.player!=NULL)
	{
		map_of_unknown_items::iterator mit;
		mit=definitions.constantly_unknown_items.find(name);
		if (mit!=definitions.constantly_unknown_items.end())
		{
			// is name hidden?
			if (level.player->is_item_known(name))
			{
				return name;
			}
			else
				return (*mit).second.name;
		}
		
	}
	return name;
}

string CGrenade::show_name()
{
   string name;
   name=CItem::show_name();
   if (this->active==true)
    name+=" (ACTIVATED)";
   return name;
}

string CHandWeapon::show_name()
{
	string name;
	name=CItem::show_name();
	string to_add;
	if (properties&TYPE_ARMOR_PERCING)
		to_add="AP";
	if (properties&TYPE_EXPLOSIVE)
	{
		if (to_add!="")
			to_add+="/EX";
		else
			to_add="EX";
	}
	if (properties&TYPE_ELECTRIC)
	{
		if (to_add!="")
			to_add+="/EM";
		else
			to_add="EM";
	}
	if (properties&TYPE_SMOKE)
	{
		if (to_add!="")
			to_add+="/SM";
		else
			to_add="SM";
	}
	if (properties&TYPE_RADIOACTIVE)
	{
		if (to_add!="")
			to_add+="/BP";
		else
			to_add="BP";
	}
	if (properties&TYPE_CHEM_POISON)
	{
		if (to_add!="")
			to_add+="/CP";
		else
			to_add="CP";
	}
	if (properties&TYPE_PARALYZE)
	{
		if (to_add!="")
			to_add+="/PA";
		else
			to_add="PA";
	}
	if (to_add!="")
	{
		name+=string(" (") + to_add + ")";
	}

	if (this->energy_activated==true)
		name+=" (ACTIVATED)";
	return name;
}

string CBaseArmor::show_name()
{
	string name;
	name=CItem::show_name();
	if (this->energy_activated==true)
		name+=" (ACTIVATED)";
	return name;
}


string CProcessor::show_name()
{
	string name;
	if (group_affiliation == GROUP_HERO)
		name = "Friendly ";
	else if (group_affiliation == GROUP_ENEMIES)
		name = "Enemy ";
	name+=CItem::show_name();
	return name;
}

void CCorpse::every_turn()
{
   if (IsDead())
	   return;

   CItem::every_turn();
   
   rotting_state++;
   if (rotting_state==1000)
   {
      rename(of_what+" Corpse (rotten)");
   }
   if (rotting_state==2000)
   {
      color=7;
      this->weight/=10;
      rename(of_what+" Skeleton");
   }
   if (rotting_state==10000)
     this->death();
}

CTrash::CTrash()
{
	ClassName = "TRASH";
	
	activities+='g';
	tile=';';
}

CRepairKit::CRepairKit()
{
	ClassName = "REPAIR_KIT";
	
	regeneration_time=-1;
	regeneration_progress=-1;
	activities+="gm";
	tile='=';
}

CProgrammator::CProgrammator()
{
	ClassName = "PROGRAMMATOR";
	
	activities+="gp";
	tile='=';
}

CProcessor::CProcessor()
{
	ClassName = "PROCESSOR";
	
	activities+="gm";
	tile='-';
	color = 3;
	program="";
	frequency=20;
	where_placed = NULL;
	group_affiliation = GROUP_NEUTRAL;
}

CRobotLeg::CRobotLeg()
{
	ClassName = "ROBOT_LEG";
	
	activities+="gm";
	tile='-';
	color = 11;

	move_power = 10;
}

CRobotShell::CRobotShell()
{
	ClassName = "ROBOT_SHELL";

	last_robot_name = "My Robot";
	
	activities+="gbm";
	tile='R';
	color = 12;

	max_number_move_slots=0;
	max_number_action_slots=0;

	ARM=0;

	cpu = NULL;
}


CCorpse::CCorpse()
{
   ClassName = "CORPSE";

   activities+='g';
   tile='%';
   rotting_state=0;
}

int CCorpse::evaluate_item()
{
   return price;
}

int CGrenade::evaluate_item()
{
	if (active)
		return 0;
	return price + PWR + RNG;
}


int CBaseArmor::evaluate_item()
{
	return ARM*10+MOD_STR+MOD_DEX+MOD_SPD;
}


int CPill::evaluate_item()
{
	if (purpose == PURPOSE_INCRASE_RADIOACTIVE ||
		purpose == PURPOSE_INCRASE_CHEM_POISON ||
		purpose == PURPOSE_DECRASE_HP ||
		purpose == PURPOSE_DECRASE_SPEED ||
		purpose == PURPOSE_DECRASE_STRENGTH ||
		purpose == PURPOSE_DECRASE_ENDURANCE ||
		purpose == PURPOSE_DECRASE_INTELLIGENCE ||
		purpose == PURPOSE_DECRASE_METABOLISM ||
		purpose == PURPOSE_DECRASE_FOV ||
		purpose == PURPOSE_DECRASE_HP)
		return 0;
	else
		return price;
}


CHandWeapon::CHandWeapon()
{
	ClassName = "HAND_WEAPON";

	energy_DMG = 0;
	energy_HIT = 0;
	energy_DEF = 0;
	energy_properties=0;	
	energy_real_properties=TYPE_NORMAL;		

	tile='(';
   color=7;
   activities+="gwrm"; // wield
   skill_to_use=SKILL_MELEE_WEAPONS;   
}

int CHandWeapon::turn_energy_off()
{
	if (energy_activated==true)
	{
		DEF -= energy_DEF;
		HIT -= energy_HIT;
		DMG -= energy_DMG;
		energy_activated=false;
		properties = energy_real_properties;
		return true;
	}
	return false;
}

int CHandWeapon::turn_energy_on()
{
	if (energy_activated==false)
	{
		DEF += energy_DEF;
		HIT += energy_HIT;
		DMG += energy_DMG;
		energy_activated=true;
		properties = properties | energy_properties;
		return true;
	}
	return false;
}

int CRangedWeapon::turn_energy_off()
{
	if (energy_activated==true)
	{
		PWR -= energy_PWR;
		ACC -= energy_ACC;
		energy_activated=false;
		return true;
	}
	return false;
}

int CRangedWeapon::turn_energy_on()
{
	if (energy_activated==false)
	{
		PWR += energy_PWR;
		ACC += energy_ACC;
		energy_activated=true;
		return true;
	}
	return false;
}
//////////////////////////////////////////////////////////////////////////
int CBaseArmor::turn_energy_off()
{
	if (energy_activated==true)
	{
		ARM -= energy_ARM;
		MOD_STR -= energy_MOD_STR;
		MOD_DEX -= energy_MOD_DEX;
		MOD_SPD -= energy_MOD_SPD;
		energy_activated=false;
		properties = energy_real_properties;
		return true;
	}
	return false;
}

int CBaseArmor::turn_energy_on()
{
	if (energy_activated==false)
	{
		ARM += energy_ARM;
		MOD_STR += energy_MOD_STR;
		MOD_DEX += energy_MOD_DEX;
		MOD_SPD += energy_MOD_SPD;
		energy_activated=true;
		properties = properties | energy_properties;
		return true;
	}
	return false;
}


bool CItem::compare_activities_with (CItem *object)
{
   if (this->activities == object->activities)
     return true;
   else
     return false;
}

CCountable::CCountable()
{
	activities+="i";
	quantity = 1;
}	

CAmmo::CAmmo()
{
	ClassName = "AMMO";

   tile='|';
   activities+="gl"; // loadable oraz ilosciowe
   color=3;
   weight=1;
   hit_points=1;
   DMG=-10;
   ACC=0;
   PWR=0;
   properties = 0;
   in_weapon = NULL;
   ammo_type=TYPE_NORMAL;
   price = 1;
}

CBaseArmor::CBaseArmor()
{
	ClassName = "BASE_ARMOR";

   MOD_STR=0;  
   MOD_DEX=0;  
   MOD_SPD=0; 

   energy_ARM=0;
   energy_MOD_STR=0;
   energy_MOD_DEX=0;
   energy_MOD_SPD=0;
   energy_properties=0;	
   energy_real_properties=TYPE_NORMAL;	   
	
   tile='[';
   activities+="gAm"; // wear
   color=7;
   weight=0;   
   properties=TYPE_NORMAL;
   price = 1;
}

NO_ARMOR::NO_ARMOR()
{
	ClassName = "NO_ARMOR";
	ARM=0;
}

CArmor::CArmor()
{
	ClassName = "ARMOR";
}

void CCountable::add_and_destroy_object(CItem *object)
{
	  CCountable *am = object->IsCountable();
	  assert (object!=NULL);

      am->quantity += this->quantity;
      death(); // unicestwienie tego itemu
}

bool CAmmo::compare_activities_with (CItem *object)
{	
   CAmmo *am = object->IsAmmo();
   if (am!=NULL)
   {
			 if ( am->properties  == this->properties &&
				  am->ammo_type == this->ammo_type )
			 return true;
   }
   return false;
}

bool CPill::compare_activities_with (CItem *object)
{	
	CPill *p = object->IsPill();
	if (p!=NULL)
	{
		if ( p->purpose  == this->purpose &&
			p->PWR == this->PWR )
			return true;
	}
	return false;
}


string CCountable::show_name()
{
	string temp;

	if (quantity==1)
		return CItem::show_name();
	
	temp = IntToStr(quantity) + " x " + CItem::show_name();
	return temp;
}

string CAmmo::show_name()
{
   string name,to_add;
   name = CCountable::show_name();

   to_add="";
   if (properties&TYPE_ARMOR_PERCING)
    to_add="AP";
   if (properties&TYPE_EXPLOSIVE)
   {
      if (to_add!="")
        to_add+="/EX";
      else
        to_add="EX";
   }
   if (properties&TYPE_ELECTRIC)
   {
	   if (to_add!="")
		   to_add+="/EM";
	   else
		   to_add="EM";
   }
   if (properties&TYPE_SMOKE)
   {
      if (to_add!="")
        to_add+="/SM";
      else
        to_add="SM";
   }
   if (properties&TYPE_RADIOACTIVE)
   {
      if (to_add!="")
        to_add+="/BP";
      else
        to_add="BP";
   }
   if (properties&TYPE_CHEM_POISON)
   {
      if (to_add!="")
        to_add+="/CP";
      else
        to_add="CP";
   }
   if (properties&TYPE_PARALYZE)
   {
      if (to_add!="")
        to_add+="/PA";
      else
        to_add="PA";
   }
   if (to_add!="")
   {
	   name+=string(" (") + to_add + ")";
   }
  
   return name;
}

bool CRangedWeapon::unload_ammo(CIntelligent *backpack_owner)
{
   ptr_list::iterator m,_m;
   CItem *item;
   bool found;
   CAmmo *am;

   if (this->ammo.quantity==0)
      return false;
   
 // 1. Find this ammo in backpack
   found=false;

   for(m=backpack_owner->backpack.begin(),_m=backpack_owner->backpack.end(); m!=_m; m++)
   {
       item=(CItem *)*m;
	   am = item->IsAmmo();
       if ( am!=NULL ) // ammo
         if (this->ammo.name == item->name)
         {
           found=true;
           break;
         }
   }
   if (found==true)
   {
      am->quantity += this->ammo.quantity;
      this->ammo.quantity=0;
   }
   else
   {
      // create item
	   am = (CAmmo *) ammo.duplicate();

      this->ammo.quantity=0;
      backpack_owner->pick_up_item(am,false);
   }
   return true;
 
}

bool CRepairKit::can_be_used()
{
	if (regeneration_time>0)
	{
		if (regeneration_progress!=regeneration_time)
			return false;
	}
	return true;
}

bool CRepairKit::fix(CItem *item, CMonster *who_fixes)
{
	if (purpose==PURPOSE_REPAIR_HP)
	{
		int points=random(who_fixes->skill[SKILL_MECHANIC]/4)+who_fixes->skill[SKILL_MECHANIC]/4;
		
		if (item->IsCountable())
			return false;
		if (item->IsRobotCPU())
			points/=4;
		else if (item->IsGrenade())
			points/=2;
		else if (item->IsRobotLeg())
			points/=2;
		
		item->fix_damage(points);
	}
	else if (purpose==PURPOSE_REPAIR_FIX_ARMOR)
	{
		CBaseArmor *pan;
		pan = item->IsArmor();
		if (pan==NULL || (pan->MOD_SPD>=0 && pan->MOD_STR>=0 && pan->MOD_DEX>=0))
			return false;
		
		// choose one negative to fix
		while (1)
		{
			int a=random(3);
			if (a==0)
			{
				if (pan->MOD_SPD<0)
				{
					if (pan->MOD_SPD<=-5)
						pan->MOD_SPD+=5;
					else
						pan->MOD_SPD=0;
					break;
				}
			}
			else if (a==1 && pan->MOD_DEX<0)
			{
				pan->MOD_DEX++;
				break;
			}
			else if (a==2 && pan->MOD_STR<0)
			{
				pan->MOD_STR++;
				break;
			}
		}

	}

	if (this->regeneration_time<0)
		death();
	else
	{
		regeneration_progress=0;
	}

	if (who_fixes==level.player)
		level.player->skill_used(SKILL_MECHANIC);	
	return true;
}


bool CItem::fix_with_item(CItem *parts, CMonster *who_fixes)
{
	CRepairKit *repairset;
	repairset = parts->IsRepairSet();
	if (repairset==NULL)
		return false;

	if (repairset->can_be_used())
	{
		repairset->fix(this, who_fixes);
		if (who_fixes==level.player)
		{
			level.player->skill_used(SKILL_MECHANIC);
		}
		return true;
	}	
	return false;
}

bool CBaseArmor::fix_with_item(CItem *parts, CMonster *who_fixes)
{
	CRepairKit *repairset;
	repairset = parts->IsRepairSet();
	if (repairset!=NULL)
		return CItem::fix_with_item(parts, who_fixes);
	CBaseArmor *ar;
	ar = parts->IsArmor();
	if (ar!=NULL) 
	{
		this->max_hit_points = (this->max_hit_points + ar->max_hit_points)/2;

		if (this->name == ar->name)
			this->hit_points += ar->hit_points;
		else
		{
			float quality1 = (float) this->hit_points/(float) this->max_hit_points;
			float quality2 = (float) ar->hit_points/(float) ar->max_hit_points;

			float quality=quality1+quality2;

			quality1 = quality1/quality;
			quality2 = quality2/quality;

//			int a = (int)((float) (quality1*(float) this->BLK));
//			int b = (int)((float) (quality2*(float) ar->BLK));
			
			this->hit_points += (int)(float)(quality2*ar->hit_points);
			this->max_hit_points = (int)(float)(quality1*this->max_hit_points + quality2*ar->max_hit_points);
			this->ARM = (int)(float)(quality1*this->ARM + quality2*ar->ARM);
//			this->BLK = (int)(float)(quality1*this->BLK + quality2*ar->BLK);

			if (name.find("Modified ")==-1)
				rename("Modified " + name);
		}

		if (this->hit_points > this->max_hit_points)
			this->hit_points = this->max_hit_points;

		parts->death();
		return true;
	}

	return false;
}


int CRangedWeapon::load_ammo(CAmmo *a_ammo)
{
   if (this->ammo_type!=a_ammo->ammo_type)
     return 1; /// niezgodny type amunicji
   if (this->ammo.quantity>0)
     return 2; /// weapon juz zaladowana
     
   ammo=*a_ammo; // przekopiowanie amunicji do magazynka
   ammo.in_weapon = this;
   ammo.owner = NULL;
   ammo.in_robot_shell = NULL;
   
   if (this->magazine_capacity < a_ammo->quantity)
   { // nie all wlazly
     ammo.quantity=magazine_capacity;
     a_ammo->quantity-=magazine_capacity;
     return 0; // bez problemow zaladowane
   }
   else /// all pociski wlazly do magazynka
   {
     ammo.quantity=a_ammo->quantity;
     a_ammo->quantity=0;

     // pozbywamy sie zerowej liczby pociskow
     a_ammo->death(); // a nastepnie niszczymy go
     return 0; // magazynek przestaje istniec
   }
}

CRangedWeapon::CRangedWeapon()
{
	ClassName = "RANGED_WEAPON";

   tile='\\';
   energy_PWR=0;
   energy_ACC=0;
   color=3;
   activities+="gwLNrm"; // get wield Load uNload
   ammo.quantity=0;
   ammo.weight=0;
   hit_points=1;
   max_hit_points=1;
   ACC=0;
   PWR=0;
   fire_type=FIRE_SINGLE;
   category_of_magazine = 0;
}


int CAmmo::evaluate_item()
{
   int value;
   value=price+PWR*10+ACC*10;
   value=value*quantity;
   return value;
}

int CRangedWeapon::evaluate_item()
{
   int value;
   value=price+ACC*10;
   value=value*(hit_points/max_hit_points);
   return value;
}



int CHandWeapon::evaluate_item()
{
   int value;
   value=price+DMG*10+HIT*10+DEF*10;
   value=value*(hit_points/max_hit_points);
   return value;
}


int CItem::used_by_on(CMonster *who, CMonster *on_who, int a_skill)
{
   int power;
   int damage;
   int skill;

   power = (who->strength.GetValue() + who->skill[a_skill]/10) * this->DMG;
   power /= 10;

   if (power<0)
     power=0;

   int result = on_who->hit_by_item(random(power),this);
   if (result>=0)
   {
	   if (who->seen_now || on_who->seen_now)
	   {
		   CItem *sound=(CItem *) definitions.find_item(this->name);
		   if (sound!=NULL)
			   sounds.PlaySoundOnce(sound->sound_used);
	   }
   }
   if (result>0)
   {
	   if (this->properties&TYPE_VAMPIRIC)
	   {
		   if (&who->unarmed==this && (on_who->IsAnimal() || on_who->IsIntelligent()))
		   {		   
			   if (who->heal(random(result)/2+result/2)==true && who->seen_now==true)
			   {			   
				   descriptions.add_attack("HIT",", drinking blood,");
			   }
		   }
	   }
   }
   return result;
}

int CItem::calculate_weight()
{
   return weight;
}

int CCountable::calculate_weight()
{
	return weight*quantity;
}

int CRobotShell::calculate_weight()
{
	int total=weight;

	if (cpu!=NULL)
		total += cpu->calculate_weight();

	ptr_list::iterator m,_m;
	CItem *t;

	for (m=move_slots.begin(),_m=move_slots.end();m!=_m;m++)
	{
		t = (CItem *)*m;
		total+=t->calculate_weight();
	}

	for (m=action_slots.begin(),_m=action_slots.end();m!=_m;m++)
	{
		t = (CItem *)*m;
		total+=t->calculate_weight();
	}
	
	return total;
}

int CRangedWeapon::calculate_weight()
{
   return weight+ammo.calculate_weight();
}

void CItem::print_attributes_vertical(int x, int y, int *changex, int *changey)
{
  print_text(x,y,IntToStr(calculate_weight()) + " grams");
  print_text(x,y+2,"HP: "+IntToStr(hit_points)+"/"+IntToStr(max_hit_points));
}

void CTrash::print_attributes_vertical(int x,int y, int *changex, int *changey)
{
	CItem::print_attributes_vertical(x,y,changex,changey);
	*changex=12;
	*changey=4;
}

void CProgrammator::print_attributes_vertical(int x,int y, int *changex, int *changey)
{
	CItem::print_attributes_vertical(x,y,changex,changey);
	*changex=12;
	*changey=4;
}

void CBattery::print_attributes_vertical(int x,int y, int *changex, int *changey)
{
	CItem::print_attributes_vertical(x,y,changex,changey);
	print_text(x,y+3,string("CAP: ") + IntToStr(max_capacity) + " EP");
	if (regeneration_speed>0)
		print_text(x,y+4,string("REGEN: ") + IntToStr(regeneration_speed) + "/1");
	else if (regeneration_speed<0)
		print_text(x,y+4,string("REGEN: 1/") + IntToStr(-regeneration_speed));

	*changex=12;
	*changey=6;
}


void CRepairKit::print_attributes_vertical(int x,int y, int *changex, int *changey)
{
	CItem::print_attributes_vertical(x,y,changex,changey);

	y+=4;
	string text;

	if (regeneration_time>0)
	{
		int progress = 100*regeneration_progress/regeneration_time;
		text = "LOAD: ";
		text+=IntToStr(progress) + "%";
		print_text(x,y++,text);
		*changey=7;
	}
	else
		*changey=6;
	
	
	*changex=12;
}

void CPill::print_attributes_vertical(int x,int y, int *changex, int *changey)
{
	CItem::print_attributes_vertical(x,y,changex,changey);
	*changex=12;
	*changey=5;
}

void CCorpse::print_attributes_vertical(int x,int y, int *changex, int *changey)
{
  CItem::print_attributes_vertical(x,y,changex,changey);
  *changex=12;
  *changey=4;
}

void CHandWeapon::print_attributes_vertical(int x,int y, int *changex, int *changey)
{
           CItem::print_attributes_vertical(x,y,changex,changey);
           y+=4;
           string text;

		   if (this->DMG>=0)
			   text=string("h2h DMG: +");
		   else
			   text=string("h2h DMG: ");
		   text+=IntToStr(this->DMG);
		   print_text(x,y,text);

           if (this->HIT>=0)
               text=string("h2h HIT: +");
           else
               text=string("h2h HIT: ");
           text+=IntToStr(this->HIT);
           print_text(x,y+1,text);
           

           if (this->DEF>=0)
               text=string("h2h DEF: +");
           else
               text=string("h2h DEF: ");
           text+=IntToStr(this->DEF);
           print_text(x,y+2,text);

           *changex=12;
           *changey=7;
}

void CRangedWeapon::print_attributes_vertical(int x,int y, int *changex, int *changey)
{
           CItem::print_attributes_vertical(x,y,changex,changey);
           y+=4;
           string text;
           if (this->ACC>=0)
               text=string("rng ACC: +");
           else
               text=string("rng ACC: ");
           text+=IntToStr(this->ACC);
           print_text(x,y,text);

           if (this->PWR>=0)
               text=string("rng PWR: +");
           else
               text=string("rng PWR: ");
           text+=IntToStr(this->PWR);
           print_text(x,y+1,text);

           if (this->HIT>=0)
               text=string("h2h HIT: +");
           else
               text=string("h2h HIT: ");
           text+=IntToStr(this->HIT);
           print_text(x,y+3,text);
           

           if (this->DEF>=0)
               text=string("h2h DEF: +");
           else
               text=string("h2h DEF: ");
           text+=IntToStr(this->DEF);
           print_text(x,y+4,text);

           if (this->DMG>=0)
               text=string("h2h DMG: +");
           else
               text=string("h2h DMG: ");
           text+=IntToStr(this->DMG);
           print_text(x,y+5,text);

           
           if (ammo.quantity>0)
           {
             print_text(x,y+7,ammo.show_name());
             ammo.print_attributes_vertical(x,y+8,changex,changey);
             // set size to name
             if (ammo.show_name().size()>*changex)
               *changex=ammo.show_name().size();
             *changey+=12;
           }
           else
           {
             text="Ammo: 0/";
             text+=IntToStr(this->magazine_capacity);
             print_text(x,y+7,text);
             *changex=12;
             *changey=12;
           }
}

void CAmmo::print_attributes_vertical(int x,int y, int *changex, int *changey)
{
			print_text(x,y,IntToStr(calculate_weight()) + " grams");
			string text;
		   *changey=1;
           if (this->ACC!=0)
		   {
			   text=string("rng ACC: ");
			   if (this->ACC>0)
				   text+="+";
			   text+=IntToStr(this->ACC);
			   print_text(x,y+*changey,text);
			   (*changey)++;
		   }

           if (this->PWR!=0)
		   {
			   if (properties&TYPE_ENERGY)
				   text=string("usg PWR: ");
			   else
					text=string("rng PWR: ");
			   if (this->PWR>0)
				   text+="+";
			   text+=IntToStr(this->PWR);
			   print_text(x,y+*changey,text);
		   }


           *changex=12;
		   *changey+=2;
}

void CGrenade::print_attributes_vertical(int x,int y, int *changex, int *changey)
{
           CItem::print_attributes_vertical(x,y,changex,changey);
           y+=4;
           string text;
           text=string("PWR: ") + IntToStr(this->PWR);
           print_text(x,y,text);

           text=string("RNG: ") + IntToStr(this->RNG);
           print_text(x,y+1,text);
           if (this->active==true)
             text=string("ACTIVATED!");
           else
             text=string("inactive");
           print_text(x,y+3,text);

           *changex=12;
           *changey=8;
}

void CArmor::print_attributes_vertical(int x,int y, int *changex, int *changey)
{
           CItem::print_attributes_vertical(x,y,changex,changey);
           y+=4;
           string text;
           text=string("ARM: ") + IntToStr(this->ARM);
           print_text(x,y,text);

		   int zmy=0;

		   if (MOD_STR!=0)
		   {
			   zmy++;
			   if (MOD_STR>0)
				text=string("STR: ") + "+" + IntToStr(this->MOD_STR);
			   else 
				text=string("STR: ") + IntToStr(this->MOD_STR);
			   print_text(x,y+zmy,text);
		   }
		   if (MOD_DEX!=0)
		   {
			   zmy++;
			   if (MOD_DEX>0)
				text=string("DEX: ") + "+" + IntToStr(this->MOD_DEX);
			   else
				text=string("DEX: ") + IntToStr(this->MOD_DEX);
			   print_text(x,y+zmy,text);
		   }
		   if (MOD_SPD!=0)
		   {
			   zmy++;
			   if (MOD_SPD>0)
				text=string("SPD: ") + "+" + IntToStr(this->MOD_SPD);
			   else
				text=string("SPD: ") + IntToStr(this->MOD_SPD);
			   print_text(x,y+zmy,text);
		   }
		   
           *changex=12;
           *changey=6+zmy;
}

void CProcessor::print_attributes_vertical(int x,int y, int *changex, int *changey)
{
	CItem::print_attributes_vertical(x,y,changex,changey);
	y+=4;
	string text;
	text=string("QUAL: ") + IntToStr(this->quality);
	print_text(x,y,text);
	
	text=string("FREQ: ") + IntToStr(this->frequency);
	print_text(x,y+1,text);
	
	text=string("PROG.SIZE: ") + IntToStr(this->program.size());
	print_text(x,y+2,text);
	
	*changex=12;
	*changey=8;
}

void CRobotLeg::print_attributes_vertical(int x,int y, int *changex, int *changey)
{
	CItem::print_attributes_vertical(x,y,changex,changey);
	y+=4;
	string text;
	text=string("MOVE PWR: ") + IntToStr(this->move_power);
	print_text(x,y,text);
		
	*changex=15;
	*changey=7;
}


void CRobotShell::print_attributes_vertical(int x,int y, int *changex, int *changey)
{
	CItem::print_attributes_vertical(x,y,changex,changey);
	y+=4;
	string text;
	text=string("MOVE SLOTS: ") + IntToStr(this->max_number_move_slots);
	print_text(x,y,text);

	ptr_list::iterator m;
	int a;
	CItem *t;
	for (a=0, m=move_slots.begin(); a<move_slots.size(); a++)
	{
		t = (CItem *)*m;
		text = t->show_name().substr(0,23);
		print_text(x,y+1+a,text);		
	}

	text=string("ACTION SLOTS: ") + IntToStr(this->max_number_action_slots);
	print_text(x,y+2+move_slots.size(),text);

	for (a=0, m=action_slots.begin(); a<action_slots.size(); a++,m++)
	{
		t = (CItem *)*m;
		text = t->show_name().substr(0,23);
		print_text(x,y+3+move_slots.size() + a,text);
	}
	
		
	*changex=23;
	*changey=8 + action_slots.size() + move_slots.size();
}

void CItem::death()
{
   if (this->is_dead)
	   return;
   activities = "";

   if (level.player!=NULL)
   {
	   if (this == level.player->last_activated)
	   {
		   level.player->last_activated = NULL;
	   }
   }

   if (owner!=NULL)
   {
	   if (this==owner->weapon)
		   owner->weapon = &owner->unarmed;
	   
	   if (this==owner->armor)
	   {
		   CIntelligent *intel = owner->IsIntelligent();
		   if (intel!=NULL)
			  intel->remove_armor();
		   else
		   {
			   assert(0);
			   owner->armor = &owner->no_armor;   // tego nie powinno byc
		   }
	   }
	   
	   CIntelligent *intel = owner->IsIntelligent();
	   if (intel!=NULL)
	   {
		   intel->take_out_from_backpack(this);
		   if (this==intel->ready_weapon)
				intel->ready_weapon = NULL;
	   }
   } 
   // not in backpack and not shell 
   if (this->in_robot_shell!=NULL) // is item in a robot shell?
   {
	   CRobotShell *shell = in_robot_shell->IsRobotShell();
	   assert(shell!=NULL);

	   if (this==shell->cpu)
	   {
		   shell->uninstall_CPU();
	   }
	   else
	   {
		   ptr_list::iterator m,_m;
		   CItem *temp, *found=NULL;
		   // znalezienie itemu na listach
		   m=shell->action_slots.begin();
		   _m=shell->action_slots.end();
		   for (;m!=_m;m++)
		   {
			   temp = (CItem *) *m;
			   if (temp==this)
			   {
				   found = this;
				   shell->uninstall_from_action_slot(this);
				   break;
			   }
		   }
		   if (found==NULL)
		   {
			   m=shell->move_slots.begin();
			   _m=shell->move_slots.end();
			   for (;m!=_m;m++)
			   {
				   temp = (CItem *) *m;
				   if (temp==this)
				   {
					   shell->uninstall_from_move_slot(this);
					   break;
				   }
			   }
			   
		   } // endof if			   
	   } // endof else it's not processor
   }

   level.remove_from_items_on_map(this); // if is on level, then remove
   // we don't remove now from all items list, to prevent damage of ptr_list in iteration of time_for_items
   level.items_to_delete.push_back(this);
   is_dead=true;
}

int NO_ARMOR::damage_it(int value)
{   
   return owner->cause_damage(value);
}

int CItem::damage_it(int value)
{
  if (value<=0)
	  return DAMAGE_NONE;

  hit_points-=value;
      
  if (hit_points<=0)
  {
    this->death();
    return DAMAGE_DEATH;
  }
  return value;
}

int CItem::fix_damage(int value)
{
	if (value<=0)
		return 0;
	
	hit_points+=value;
	
	if (hit_points>max_hit_points)
	{
		hit_points=max_hit_points;
		return 1;
	}
	return 1;
}


void CBaseArmor::death()
{
	if (owner!=NULL && this==owner->armor)
	{
		CIntelligent *intel = owner->IsIntelligent();
		if (intel!=NULL)
			intel->set_armor(&owner->no_armor);
	}	       
	
	CItem::death();
}

void CHandWeapon::death()
{
	if (owner!=NULL && this==owner->weapon)
	{
		CIntelligent *intel = owner->IsIntelligent();
		if (intel!=NULL)
			intel->set_weapon(&owner->unarmed);
	}	       
	
	CItem::death();
}

int CRangedWeapon::damage_it(int value)
{
  int which_damage = CItem::damage_it(value);
  if (this->IsDead())
  {
	 if (owner!=NULL && this==owner->weapon)
	 {
		 CIntelligent *intel = owner->IsIntelligent();
		 if (intel!=NULL)
			 intel->set_weapon(&owner->unarmed);
	 }	       
  }
  return which_damage;
}


int CGrenade::damage_it(int value)
{
  hit_points-=value;
  if (hit_points<=0)
  {
     if (properties&TYPE_SENSOR)
     {
	   // remove sensor type from item
       PROPERTIES mask;
       mask=TYPE_SENSOR;
       mask=~mask;
       properties=properties&mask;
     }
     active=false;
     works_now=false;
     activate();
     counter=0;
  }
  return 0;
}


int CCountable::damage_it(int value)
{
  if (value<=0)
	  return DAMAGE_NONE;

  quantity-=value;
  if (quantity<=0)
  {
     death();
	 return DAMAGE_DEATH;
  }
  return value;
}

int CCountable::fix_damage(int value)
{
	return 0;
}


void CRangedWeapon::incrase_fire_mode()
{
   switch (fire_type)
   {
     case FIRE_SINGLE:
         if (available_fire_types&FIRE_DOUBLE)
         {
           fire_type=FIRE_DOUBLE;
           break;
         }
     case FIRE_DOUBLE:
         if (available_fire_types&FIRE_TRIPLE)
         {
           fire_type=FIRE_TRIPLE;
           break;
         }
     case FIRE_TRIPLE:
         if (available_fire_types&FIRE_BURST)
         {
           fire_type=FIRE_BURST;
           break;
         }

   }
}

void CRangedWeapon::decrase_fire_mode()
{
   switch (fire_type)
   {
     case FIRE_BURST:
         if (available_fire_types&FIRE_TRIPLE)
         {
           fire_type=FIRE_TRIPLE;
           break;
         }
     case FIRE_TRIPLE:
         if (available_fire_types&FIRE_DOUBLE)
         {
           fire_type=FIRE_DOUBLE;
           break;
         }
     case FIRE_DOUBLE:
         if (available_fire_types&FIRE_SINGLE)
         {
           fire_type=FIRE_SINGLE;
           break;
         }

   }
}


CGrenade::CGrenade()
{
	ClassName = "GRENADE";

   tile='`';
   color=5;
   activities+="gam"; // activate
   counter=-1;
   active=false;
   works_now=false;
   properties=TYPE_NORMAL;
   weight=400;
   hit_points=20;
   max_hit_points=20;
   PWR=0;
   RNG=0;
   price = 1; // aby nie 0, bo potworki nie podnosza wtedy
}

int CGrenade::create_shield()
{
    int x,y;
    int x1,y1;
    int dist;

    for (x=-RNG;x<=RNG;x++)
	{
		for (y=-RNG;y<=RNG;y++)
		{
			x1=pX()+x;
			y1=pY()+y;
			dist=distance(pX(),pY(),x1,y1);
			if (DLY==0 && dist==2) // dla personal shield
				dist--;

			if (dist <= RNG )
			{
				level.map.addShield(x1,y1);
			}
		}
	}
	return 0;
}

int CGrenade::remove_shield()
{
    int x,y;
    int x1,y1;
    int dist;

    for (x=-RNG;x<=RNG;x++)
    for (y=-RNG;y<=RNG;y++)
    {
         x1=pX()+x;
         y1=pY()+y;
         dist=distance(pX(),pY(),x1,y1);
		 if (DLY==0 && dist==2) // dla personal shield
			 dist--;

         if (dist <= RNG )
         {
           level.map.removeShield(x1,y1);
         }
    }
    return 0;
}

void CItem::every_turn()
{
	if (purpose == PURPOSE_TEMP_MISSILE)
	{
		death();
	}
	if (owner==NULL && energy_activated==true)
	{
		turn_energy_off();
	}
	if (owner!=NULL)
	{
		ChangePosition(owner->pX(),owner->pY());
	}
	else if (in_robot_shell!=NULL)
	{
		ChangePosition(in_robot_shell->pX(),in_robot_shell->pY());
	}	
}

void CArmor::every_turn()
{
	CItem::every_turn();
	if (owner!=NULL && this->energy_activated)
	{
		if (energy_MOD_DEX>0 || energy_MOD_STR || energy_MOD_SPD || energy_ARM)
			uses_energy();
	}	
}

void CRepairKit::every_turn()
{
	if (regeneration_time>0)
		regeneration_progress++;

	if (regeneration_progress>regeneration_time)
		regeneration_progress=regeneration_time;
}

void CBattery::every_turn()
{
	if (owner!=NULL && owner->energy_points.GetValue()<max_capacity)
	{
		if (regeneration_speed>0)
		{
			owner->add_energy(regeneration_speed);
		}
		else if (regeneration_speed<0)
		{
			if (random(-regeneration_speed)==0)
				owner->add_energy(1);
		}
	}	
}

void CGrenade::death()
{
	if (active && properties&TYPE_POWER_SHIELD)
	{
		remove_shield();		   
		active=false;
	}
	return CItem::death();
}

bool CGrenade::ChangePosition(int x, int y)
{	
	if (active && works_now && properties&TYPE_POWER_SHIELD)
		remove_shield();		   
	bool result = CItem::ChangePosition(x,y);
	if (active && works_now && properties&TYPE_POWER_SHIELD)
		create_shield();	
	return result;
}

void CGrenade::every_turn()
{
   if (IsDead())
	   return;

   CItem::every_turn();

   bool exploded = false;

   // if not active and not works now or it's sensor and works now
   // and no one have it in backpack
   // or if someone has and it's not a power shield
   if (((active==true && !works_now) || (properties&TYPE_SENSOR && works_now)) && (owner==NULL || properties&TYPE_POWER_SHIELD))
   {
	   counter--;
	   if (counter<=0)
	   {
		   works_now=true;
		   if (properties&TYPE_SENSOR && works_now)
		   {
			   if (is_monster_around()==false)
			   {
				   counter=0;
				   return;
			   }		   
		   }              
		   if (properties&TYPE_STUN && !(properties&TYPE_SMOKE))
		   {
			   if (level.map.seen_by_player(pX(),pY()) ||
				   level.map.seen_by_player(pX(),pY()))		   
			   {
				   level.player->add_known_item(name);
			   }		   
			   stun_explosion();
			   exploded = true;
		   }
		   if (properties&TYPE_EXPLOSIVE)
		   {
			   if (level.map.seen_by_player(pX(),pY()))		   
			   {
				   level.player->add_known_item(name);
			   }		   
			   explode();
			   exploded = true;
		   }
		   else if (properties&TYPE_ELECTRIC)
		   {
			   if (level.map.seen_by_player(pX(),pY()))		   
			   {
				   level.player->add_known_item(name);
			   }		   
			   emp_explosion();
			   exploded = true;
		   }
		   else if (properties&TYPE_INCENDIARY)
		   {
			   if (level.map.seen_by_player(pX(),pY()))		   
			   {
				   level.player->add_known_item(name);
			   }		   
			   fire_explosion();
			   exploded = true;
		   }
		   else if (properties&TYPE_POWER_SHIELD) // power shield sie uaktywnia od razu
		   {
			   if (level.map.seen_by_player(pX(),pY()))		   
				   level.player->add_known_item(name);
			   create_shield();
			   CGrenade *sound=(CGrenade *) definitions.find_item(name);
			   if (sound!=NULL)
				sounds.PlaySoundOnce(sound->sound_explosion);
			   return;
		   }
		   if (properties&TYPE_SMOKE)
		   {
			   counter=PWR;     
			   if (level.map.seen_by_player(pX(),pY()))	
			   {
				   CGrenade *sound=(CGrenade *) definitions.find_item(name);
				   if (sound!=NULL)
					   sounds.PlaySoundOnce(sound->sound_explosion);

			   }
		   }

	   }     
   }
   if (exploded == true)
   {
		this->death();
		return;
   }
   
   if (works_now)
   {
	   if (properties&TYPE_POWER_SHIELD) // power shield sie uaktywnia od razu
	   {
		   if (--PWR < 0)
			   death();
		   return;
	   }
       else if (properties&TYPE_SMOKE)
       {
		   counter--;
		   if (level.map.seen_by_player(pX(),pY()))		   
		   {
			   level.player->add_known_item(name);
		   }		   
           if (counter>=0)
             produce_gas();
           else
             this->death();
       }
   }
}

bool CItem::is_monster_around()
{
   if (level.map.IsMonsterOnMap(pX(),pY()))
     return true;
   if (level.map.IsMonsterOnMap(pX(),pY()-1))
     return true;
   if (level.map.IsMonsterOnMap(pX(),pY()+1))
     return true;
   if (level.map.IsMonsterOnMap(pX()-1,pY()))
     return true;
   if (level.map.IsMonsterOnMap(pX()+1,pY()))
     return true;
   if (level.map.IsMonsterOnMap(pX()-1,pY()-1))
     return true;
   if (level.map.IsMonsterOnMap(pX()+1,pY()-1))
     return true;
   if (level.map.IsMonsterOnMap(pX()-1,pY()+1))
     return true;
   if (level.map.IsMonsterOnMap(pX()+1,pY()+1))
     return true;
   return false;
}

int CGrenade::produce_gas()
{
    level.add_gas(this->pX(),this->pY(),properties,this->RNG*5);
	return 0;
}

int CGrenade::activate()
{
    if (active!=true)
    {
       counter=DLY;
       active=true;
       hit_points = 1; // active jest bardzo wrazliwy (aby mozna bylo strzalem np. mine rozwalic)
	   return true;
    }
  return false;
}

int CHandWeapon::activate()
{
	if (energy_activated==-1)
		return 0;

	if (energy_activated==false)
	{
		this->turn_energy_on();
		if (uses_energy()==true)
			return false;
		return true;
	}
	else if (energy_activated==true)
	{
		this->turn_energy_off();
		return true;
	}
	return false;
}

int CRangedWeapon::activate()
{
	if (energy_activated==-1)
		return 0;
	
	if (energy_activated==false)
	{
		this->turn_energy_on();
		if (uses_energy()==true)
			return false;
		return true;
	}
	else if (energy_activated==true)
	{
		this->turn_energy_off();
		return true;
	}
	return false;
}

int CBaseArmor::activate()
{
	if (energy_activated==-1)
		return 0;
	
	if (energy_activated==false)
	{
		this->turn_energy_on();
		if (uses_energy()==true)
			return false;
		return true;
	}
	else if (energy_activated==true)
	{
		this->turn_energy_off();
		return true;
	}
	return false;
}

int CGrenade::calculate_explosion(int *data)
{
    int x,y;
    int index;
    int x1,y1;
    int dist;
    int power;
    struct CPosition position[100]; // tyle sie miescie na ekranie po przekatnej

    for (x=0;x<MAPWIDTH;x++)
     for (y=0;y<MAPHEIGHT;y++)
       data[x+y*MAPWIDTH]=0;
     

    // count bresenhamem places, gdzie dociera fala wybuchu

    for (x=-RNG;x<=RNG;x++)
    for (y=-RNG;y<=RNG;y++)
    {
     if (x==-RNG || x==RNG || y==-RNG || y==RNG)
     {
       generate_bresenham_line(this->pX(),this->pY(),this->pX()+x,this->pY()+y, (CPosition *) position, RNG );
       for (index=0;index<RNG;index++) // kolejne pola
       {
         x1=position[index].x;
         y1=position[index].y;         
         
         if (!level.map.onMap(x1,y1))
           break;

         dist=distance(pX(),pY(),x1,y1);
         if (dist >= RNG )
           data[x1+y1*MAPWIDTH]=-1;
           
         if (data[x1+y1*MAPWIDTH]==-1) // to pole it_blocks
           break;
           
         // is something blocks it
         if (level.map.blockMissile(x1,y1)==true)
         {
           if (level.map.onMap(position[index+1].x,position[index+1].y))
             data[position[index+1].x+(position[index+1].y)*MAPWIDTH]=-1;
         }
         power=PWR-dist*(PWR/RNG);
         
         if (power<1)
           power=1;
           
         data[x1+y1*MAPWIDTH]=power;
       } // endof for
     } // endof if
    } // endof for
  return 0;
}

int CGrenade::explode()
{
    int x,y;
    ptr_list on_cell;
    ptr_list::iterator m,_m;
    CItem *temp;

    CMonster *monster;

    int data[MAPWIDTH*MAPHEIGHT+1];
    calculate_explosion((int*) &data);

    if (screen.draw_explosion(data, PWR+1))
	{
		CGrenade *sound=(CGrenade *) definitions.find_item(name);
		if (sound!=NULL)
			sounds.PlaySoundOnce(sound->sound_explosion);
	}

	// add damage on cells, where explosion reach
    on_cell.clear();
    
    for (x=0;x<MAPWIDTH;x++)
     for (y=0;y<MAPHEIGHT;y++)
     {
       if (data[x+y*MAPWIDTH]>0)
       {
         // add smoke
         level.add_gas(x,y,TYPE_SMOKE,random(data[x+y*MAPWIDTH]*20));

         // do damage
         monster=level.monster_on_cell(x,y);
         if (monster!=NULL)
           monster->hit_by_explosion(random(data[x+y*MAPWIDTH]/2)+data[x+y*MAPWIDTH]/2);
           
         // damage itms
         level.list_of_items_from_cell(&on_cell,x,y);
         for(m=on_cell.begin(),_m=on_cell.end(); m!=_m; m++)
         {
           temp=(CItem *)*m;
           temp->damage_it(random(data[x+y*MAPWIDTH]/2)+data[x+y*MAPWIDTH]/2);
         }
         on_cell.clear();
         
         // damage terrain
         level.map.damage(x,y,random(data[x+y*MAPWIDTH]/2)+data[x+y*MAPWIDTH]/2);
       }
     }
     
  return 0;
}


int CGrenade::fire_explosion()
{
    int x,y;

    CMonster *monster;

    int data[MAPWIDTH*MAPHEIGHT+1];

    calculate_explosion((int*) &data);

	bool fire_visible=level.map.seen_by_player(pX(),pY());

	for (x=0;x<MAPWIDTH;x++)
		for (y=0;y<MAPHEIGHT;y++)
		{
			if (data[x+y*MAPWIDTH]>0)
			{
				if (level.map.seen_by_player(x,y))
					fire_visible=true;

				// dodanie ognia		  
				if (random(6)!=0)
				{
					monster=level.monster_on_cell(x,y);
					if (monster!=NULL)
						monster->hit_changes_status(TYPE_INCENDIARY,random(PWR));

					if (!level.map.blockMove(x,y))
						level.add_gas(x,y,TYPE_INCENDIARY,random(PWR));
				}
				else if (!level.map.blockMove(x,y))			 
					level.add_gas(x,y,TYPE_SMOKE,random(PWR*50));
			}
		}
	if (fire_visible)
	{
		CGrenade *sound=(CGrenade *) definitions.find_item(name);
		if (sound!=NULL)
			sounds.PlaySoundOnce(sound->sound_explosion);
	}

  return 0;
}

int CGrenade::stun_explosion()
{
    int x,y;
    CMonster *monster;

    int data[MAPWIDTH*MAPHEIGHT+1];
    calculate_explosion((int*) &data);

    for (x=0;x<MAPWIDTH;x++)
     for (y=0;y<MAPHEIGHT;y++)
     {
       if (data[x+y*MAPWIDTH]>0)
       {
         monster=level.monster_on_cell(x,y);
         if (monster!=NULL)
			monster->hit_changes_status(TYPE_STUN,(random(data[x+y*MAPWIDTH]/2)+data[x+y*MAPWIDTH]/2));
       }
     }
	 if (screen.draw_stunning_explosion(data))
	 {
		 CGrenade *sound=(CGrenade *) definitions.find_item(name);
		 if (sound!=NULL)
			 sounds.PlaySoundOnce(sound->sound_explosion);
	 }


  return 0;
}

int CGrenade::emp_explosion()
{
	int x,y;
	CMonster *monster;

	int data[MAPWIDTH*MAPHEIGHT+1];
	calculate_explosion((int*) &data);

	if (screen.draw_emp_explosion(data, PWR+1))
	{
		CGrenade *sound=(CGrenade *) definitions.find_item(name);
		if (sound!=NULL)
			sounds.PlaySoundOnce(sound->sound_explosion);
	}

	for (x=0;x<MAPWIDTH;x++)
		for (y=0;y<MAPHEIGHT;y++)
		{
			if (data[x+y*MAPWIDTH]>0)
			{
				monster=level.monster_on_cell(x,y);
				if (monster!=NULL)
				{
					if (monster->IsRobot())
						monster->hit_changes_status(TYPE_ELECTRIC,(random(data[x+y*MAPWIDTH]/2)+data[x+y*MAPWIDTH]/2));
				}
			}
		}
		return 0;
}


void FIRE::every_turn()
{
   if (is_dead)
	   return;

   if (random(20)==0)
     density--;

   if (density<=0 || level.map.blockMove(pX(),pY()))
   {
     death();
     return;
   }

   level.add_gas(pX(),pY(),TYPE_SMOKE,random(8)+5);
   
   act_on_monster();
   act_on_items();

/// move fire
   if (random(40)==0)
   {
	   int flow;
	   
	   flow=random(density);

	   switch(random(8))
	   {
		 case 0:
		 if (level.add_gas(pX()-1,pY(),properties,flow))
			 density-=flow;
		 break;
		 case 1:
		 if (level.add_gas(pX()+1,pY(),properties,flow))
			 density-=flow;
		 break;
		 case 2:
		 if (level.add_gas(pX(),pY()+1,properties,flow))
			 density-=flow;
		 break;
		 case 3:
		 if (level.add_gas(pX(),pY()-1,properties,flow))
			 density-=flow;
		 break;
		 case 4:
		 if (level.add_gas(pX()-1,pY()-1,properties,flow))
			 density-=flow;
		 break;
		 case 5:
		 if (level.add_gas(pX()+1,pY()-1,properties,flow))
			 density-=flow;
		 break;
		 case 6:
		 if (level.add_gas(pX()-1,pY()+1,properties,flow))
			 density-=flow;
		 break;
		 case 7:
		 if (level.add_gas(pX()+1,pY()+1,properties,flow))
			 density-=flow;
		 break;
	   }
   }
}


void GAS::every_turn()
{
   if (this->is_dead)
	   return;

   act_on_monster();   

   int half,x,y;
   if (density<=0)
   {
     death();
     return;
   }
   density--;
   half=density/2;

   x=this->pX();
   y=this->pY();

    if (half>1)
    {
     if (random(6)==0)
     level.add_gas(x-1,y,properties,random(half));
     if (random(6)==0)
     level.add_gas(x+1,y,properties,random(half));
     if (random(6)==0)
     level.add_gas(x,y+1,properties,random(half));
     if (random(6)==0)
     level.add_gas(x,y-1,properties,random(half));

     if (random(6)==0)
     level.add_gas(x-1,y-1,properties,random(half));
     if (random(6)==0)
     level.add_gas(x+1,y-1,properties,random(half));
     if (random(6)==0)
     level.add_gas(x-1,y+1,properties,random(half));
     if (random(6)==0)
     level.add_gas(x+1,y+1,properties,random(half));
   
    }
   density=random(density/3)+density/2;
}

void GAS::death()
{
   if (this->is_dead)
	   return;

   this->is_dead = true;
   level.map.backBlockLOS(this->pX(),this->pY());
   ChangePosition(-1,-1);
   // don't delete from gases_on_map directly, to avoid iteration damage in ptr_list during time_for_gases
   level.gases_to_delete.push_back(this);
}

GAS::GAS()
{
	ClassName = "GAS";

	tile='&';
	is_dead = false;
}

FIRE::FIRE()
{
	ClassName = "FIRE";
}

void FIRE::death()
{
   GAS::death();
}


void GAS::display()
{
     if (density>50)
     {
       if (density>300)
       {
        level.map.setBlockLOS(this->pX(),this->pY());
       }
       else
        level.map.backBlockLOS(this->pX(),this->pY());
     
       if (density>200)
	   {
		   if (properties&TYPE_CHEM_POISON)
		   {
			   color=2;
		   }
		   else if (properties&TYPE_RADIOACTIVE)
		   {
			   color=5;
		   }
		   else if (properties&TYPE_STUN)
		   {
			   color=7;
		   }
		   else if (properties&TYPE_PARALYZE)
		   {
			   color=1;
		   }
		   else // np. TYPE_SMOKE
		   {
			   color=8;
		   }
	   }
       else
	   {
		   if (properties&TYPE_CHEM_POISON)
		   {
			   color=10;
		   }
		   else if (properties&TYPE_RADIOACTIVE)
		   {
			   color=13;
		   }
		   else if (properties&TYPE_STUN)
		   {
			   color=15;
		   }
		   else if (properties&TYPE_PARALYZE)
		   {
			   color=9;
		   }
		   else // other f.e. TYPE_SMOKE
		   {
			   color=7;
		   }
	   }
       CTile::display();
       level.map.setLastTile(pX(),pY(),' ');
     }
}


void FIRE::display()
{
     char kolory[5]={8,4,12,14,15};
     CTile::display();

     if (density>20)
       color=kolory[ random(2)+3 ];
     else if (density>10)
       color=kolory[ random(2)+2 ];
     else if (density>4)
       color=kolory[ random(2)+1 ];
     else color=kolory[ random(2)+0 ];
     
     CTile::display();     
}


CONTROLLER::CONTROLLER()
{
	ClassName = "CONTROLLER";
	tile = '$';

	invisible = true;
	activities = '!';
	price = 0;
}

CPill::CPill()
{
	ClassName = "PILL";
	tile = ',';

	DMG=-10;
	HIT=-10;
	DEF=-10;
	hit_points = 3;
	max_hit_points = 3;
	weight = 10;
	price = 100;
	
	activities += "gu";
}

int CPill::use_on(CMonster *who)
{
	int power;
	if (who==level.player)
	{
		screen.console.add("You eat the pill.",11);		
		sounds.PlaySoundOnce("data/sounds/other/swallow.wav");
	}
	
	power = random(PWR/2) + PWR/2;
	switch(purpose) {
	case PURPOSE_INCRASE_HP:
		if (who->hit_points.GetValue() != who->hit_points.max)
		{
			if (who==level.player)
			{
				level.player->stop_repeating();
				screen.console.add("Small robots are reconstructing you.",11);		
				level.player->add_known_item(name);
			}
			else if (who->seen_now)
				screen.console.add(who->name + " is reconstructed.",11);
			who->heal(power);
		}
		else if (who==level.player)
			screen.console.add("Nothing seems to happen.",11);				
		break;
	case PURPOSE_DECRASE_HP:
		if (who==level.player)
		{
			level.player->stop_repeating();
			screen.console.add("You writhe in pain!",11);		
			level.player->add_known_item(name);
		}
		else if (who->seen_now)
			screen.console.add(who->name + " writhes in pain.",11);
		who->cause_damage(power);
		break;
	case PURPOSE_INCRASE_RADIOACTIVE:
		if (who==level.player)
		{
			screen.console.add("You feel radiated!!",11);		
			level.player->add_known_item(name);
		}
		else if (who->seen_now)
			screen.console.add(who->name + " looks radiated.",11);		
		who->state[STATE_RADIOACTIVE]+=power;
		break;
	case PURPOSE_DECRASE_RADIOACTIVE:
		if (who->state[STATE_RADIOACTIVE]>0)
		{
			if (who==level.player)
			{
				level.player->stop_repeating();
				screen.console.add("You feel not so radiated.",11);		
				level.player->add_known_item(name);
			}
			else if (who->seen_now)
				screen.console.add(who->name + " looks not so radiated.",11);		
			who->state[STATE_RADIOACTIVE]-=power;
			if (who->state[STATE_RADIOACTIVE]<0)
				who->state[STATE_RADIOACTIVE]=0;
		}
		else if (who==level.player)
			screen.console.add("Nothing seems to happen.",11);		
		break;
	case PURPOSE_INCRASE_CHEM_POISON:
		if (who==level.player)
		{
			level.player->stop_repeating();
			screen.console.add("Your blood is burning!",11);		
			level.player->add_known_item(name);
		}
		else if (who->seen_now)
			screen.console.add(who->name + " looks poisoned.",11);		
		who->state[STATE_CHEM_POISON]+=power;
		break;
	case PURPOSE_DECRASE_CHEM_POISON:
		if (who->state[STATE_CHEM_POISON]>0)
		{
			if (who==level.player)
			{
				level.player->stop_repeating();
				screen.console.add("Your blood no longer burns as much as it did a moment ago.",11);		
				level.player->add_known_item(name);
			}
			else if (who->seen_now)
				screen.console.add(who->name + " looks not so poisoned.",11);		
			who->state[STATE_CHEM_POISON]-=power;
			if (who->state[STATE_CHEM_POISON]<0)
				who->state[STATE_CHEM_POISON]=0;
		}
		else if (who==level.player)
			screen.console.add("Nothing seems to happen.",11);		
		
		break;
	case PURPOSE_INCRASE_SPEED:
		if (who->state[STATE_SPEED]<40)
		{
			if (who==level.player)
			{
				level.player->stop_repeating();
				screen.console.add("Everything seems to slow down.",11);		
				level.player->add_known_item(name);
			}
			else if (who->seen_now)
				screen.console.add(who->name + " seems to move faster.",11);		
			who->state[STATE_SPEED]+=power;
		}
		else if (who==level.player)
			screen.console.add("Nothing seems to happen.",11);		
		
		break;
	case PURPOSE_DECRASE_SPEED:
		if (who->state[STATE_SPEED]>-40)
		{
			if (who==level.player)
			{
				level.player->stop_repeating();
				screen.console.add("Everything seems to move faster.",11);		
				level.player->add_known_item(name);
			}
			else if (who->seen_now)
				screen.console.add(who->name + " seems to move slower.",11);		
			who->state[STATE_SPEED]+=power;
		}
		else if (who==level.player)
			screen.console.add("Nothing seems to happen.",11);		
		
		break;
	case PURPOSE_INCRASE_STRENGTH:
		if (who->state[STATE_STRENGTH]<60)
		{
			if (who==level.player)
			{
				level.player->stop_repeating();
				screen.console.add("You feel stronger.",11);		
				level.player->add_known_item(name);
			}
			else if (who->seen_now)
				screen.console.add(who->name + " seems to be stronger.",11);		
			who->state[STATE_STRENGTH]+=power;
		}
		else if (who==level.player)
			screen.console.add("Nothing seems to happen.",11);		
		
		break;
	case PURPOSE_DECRASE_STRENGTH:
		if (who->state[STATE_STRENGTH]>60)
		{
			if (who==level.player)
			{
				level.player->stop_repeating();
				screen.console.add("You feel weaker.",11);		
				level.player->add_known_item(name);
			}
			else if (who->seen_now)
				screen.console.add(who->name + " seems to be weaker.",11);		
			who->state[STATE_STRENGTH]+=power;
		}
		else if (who==level.player)
			screen.console.add("Nothing seems to happen.",11);		
		
		break;
	}
	quantity--;
	if (quantity<=0)
		death();
	return 0;
}

void CPill::every_turn()
{
	CItem::every_turn();	
}

void CONTROLLER::every_turn()
{	
   if (IsDead())
	   return;

	if (purpose == PURPOSE_AUTO_DOOR_OPEN)
	{
			if (is_monster_around()==true)
			{
				if (level.map.open(pX(),pY())==true) // otworzyly sie
				{
					if (level.map.seen_by_player(pX(),pY()))
					{
						if (!level.player->is_repeating_action() || 
							 level.player->is_repeating_action() && !(abs(pX()-level.player->pX())<2 && abs(pY()-level.player->pY())<2)
						   )
						screen.console.add("The door draws aside.",7);
						sounds.PlaySoundOnce("data/sounds/other/openclose.wav");
					}
				}
			}
			else if (level.map.isClosable(pX(),pY()))
			{
				if (level.map.close(pX(),pY())==true)
				{
					if (level.map.seen_by_player(pX(),pY()))
					{
						if (!level.player->is_repeating_action())
							screen.console.add("The door has closed.",7);
						sounds.PlaySoundOnce("data/sounds/other/openclose.wav");
					}
				}
			}
			if (level.map.getPercentDamage(pX(),pY())>5) // > 5%
				death();
	}
	else if (purpose == PURPOSE_WEAK_CORRIDOR_IN_MINES)
	{
		if (level.map.getPercentDamage(pX(),pY())>5) // > 5%
		{
			CMonster *on_cell;

			level.map.CollapseCeiling(this->pX(),this->pY());			

			for (int a=-1;a<=1;a++)
				for (int b=-1;b<=1;b++)
				{
					on_cell = level.monster_on_cell(this->pX()+a,this->pY()+b);
					if (on_cell!=NULL)
					{
						if (on_cell->seen_now)
						{
							screen.console.add(on_cell->name + " has been crushed by collapsed ceiling.",4);
						}
						// monster on cell die
						if (a==0 && b==0)
							on_cell->death();					
						else
							on_cell->cause_damage(50+random(50));					
					}
				}
			death();			
		}
	}
	else if (purpose == PURPOSE_SMOKE_GENERATOR && level.is_player_on_level && level.map.seen_by_player(pX(),pY()))
	{
		if (random(30)==0)
		{
			level.add_gas(pX(),pY(),TYPE_SMOKE,20000);			
		}
	}
	else if (purpose == PURPOSE_TUTORIAL_ADVANCE)
	{
		if (level.player!=NULL)
		{
			if ( abs(level.player->pX()-pX())<2 && abs(level.player->pY()-pY())<2 )
			{
				tutorial.show_next();
				death();
			}
		}
	}
	
		
}

CItem * CTrash::duplicate()
{
	CTrash *new_one;
	new_one=new CTrash;
	*new_one=*this;

	map_of_unknown_items::iterator mit;
	mit=definitions.constantly_unknown_items.find(name);
	if (mit!=definitions.constantly_unknown_items.end())
	{
		new_one->color = (*mit).second.color;
	}

	new_one->owner=NULL;
	new_one->in_robot_shell=NULL;
	new_one->rename(this->name);

	level.add_to_items_on_map(new_one);
	level.all_items.push_back(new_one);
	
	return (CItem *) new_one;
}

CItem * CRepairKit::duplicate()
{
	CRepairKit *new_one;
	new_one=new CRepairKit;
	*new_one=*this;
	
	map_of_unknown_items::iterator mit;
	mit=definitions.constantly_unknown_items.find(name);
	if (mit!=definitions.constantly_unknown_items.end())
	{
		new_one->color = (*mit).second.color;
	}
	
	new_one->regeneration_progress = new_one->regeneration_time;
	new_one->owner=NULL;
	new_one->in_robot_shell=NULL;
	new_one->rename(this->name);
	
	level.add_to_items_on_map(new_one);
	level.all_items.push_back(new_one);
	
	return (CItem *) new_one;
}

CItem * CProgrammator::duplicate()
{
	CProgrammator *new_one;
	new_one=new CProgrammator;
	*new_one=*this;
	
	map_of_unknown_items::iterator mit;
	mit=definitions.constantly_unknown_items.find(name);
	if (mit!=definitions.constantly_unknown_items.end())
	{
		new_one->color = (*mit).second.color;
	}
	
	new_one->owner=NULL;
	new_one->in_robot_shell=NULL;
	new_one->rename(this->name);
	
	level.add_to_items_on_map(new_one);
	level.all_items.push_back(new_one);	
	return (CItem *) new_one;
}

CItem * CBattery::duplicate()
{
	CBattery *new_one;
	new_one=new CBattery;
	*new_one=*this;
	
	map_of_unknown_items::iterator mit;
	mit=definitions.constantly_unknown_items.find(name);
	if (mit!=definitions.constantly_unknown_items.end())
	{
		new_one->color = (*mit).second.color;
	}
	
	new_one->owner=NULL;
	new_one->in_robot_shell=NULL;
	new_one->rename(this->name);
	new_one->capacity = random(new_one->max_capacity/2) + new_one->max_capacity/2;
	
	level.add_to_items_on_map(new_one);
	level.all_items.push_back(new_one);	
	return (CItem *) new_one;
}

CItem * CProcessor::duplicate()
{
	CProcessor *new_one;
	new_one=new CProcessor;
	*new_one=*this;
	map_of_unknown_items::iterator mit;
	mit=definitions.constantly_unknown_items.find(name);
	if (mit!=definitions.constantly_unknown_items.end())
	{
		new_one->color = (*mit).second.color;
	}
	
	new_one->owner=NULL;
	new_one->in_robot_shell=NULL;	
	new_one->rename(this->name);
    level.add_to_items_on_map(new_one);
    level.all_items.push_back(new_one);
	return (CItem *) new_one;
}

CItem * CRobotLeg::duplicate()
{
	CRobotLeg *new_one;
	new_one=new CRobotLeg;
	*new_one=*this;
	map_of_unknown_items::iterator mit;
	mit=definitions.constantly_unknown_items.find(name);
	if (mit!=definitions.constantly_unknown_items.end())
	{
		new_one->color = (*mit).second.color;
	}
	
	new_one->owner=NULL;
	new_one->in_robot_shell=NULL;
	new_one->rename(this->name);
    level.add_to_items_on_map(new_one);
    level.all_items.push_back(new_one);
	return (CItem *) new_one;
}

CItem * CRobotShell::duplicate()
{
	CItem *temp, *temp2;
	ptr_list::iterator m,_m;

	CRobotShell *new_one;
	new_one=new CRobotShell;
	*new_one=*this;
	map_of_unknown_items::iterator mit;
	mit=definitions.constantly_unknown_items.find(name);
	if (mit!=definitions.constantly_unknown_items.end())
	{
		new_one->color = (*mit).second.color;
	}
	
	new_one->owner=NULL;
	new_one->in_robot_shell=NULL;
	new_one->rename(this->name);
	
	new_one->action_slots.clear();
	new_one->move_slots.clear();

	// copy legs

	for (m=move_slots.begin(),_m=move_slots.end();m!=_m;m++)
	{
		temp = (CItem *) *m;
		temp2 = temp->duplicate();

		level.remove_last_item_on_map();
		new_one->install_in_move_slot(temp2);
	}

	// copy "hands"

	for (m=action_slots.begin(),_m=action_slots.end();m!=_m;m++)
	{
		temp = (CItem *) *m;
		temp2 = temp->duplicate();
		level.remove_last_item_on_map();
		new_one->install_in_action_slot(temp2);
	}

	// and cpu

	if (cpu!=NULL)
	{
		new_one->cpu=NULL;
		temp2 = cpu->duplicate();
		level.remove_last_item_on_map();
		new_one->install_CPU(temp2);
	}
	
    level.add_to_items_on_map(new_one);
    level.all_items.push_back(new_one);
	return (CItem *) new_one;
}

CItem * CGrenade::duplicate()
{
   CGrenade *new_one;
   new_one=new CGrenade;
   *new_one=*this;

   map_of_unknown_items::iterator mit;
   mit=definitions.constantly_unknown_items.find(name);
   if (mit!=definitions.constantly_unknown_items.end())
   {
	   new_one->color = (*mit).second.color;
   }
   
   new_one->owner=NULL;
   new_one->in_robot_shell=NULL;
   new_one->rename(this->name);
   level.add_to_items_on_map(new_one);
   level.all_items.push_back(new_one);
   return (CItem *) new_one;
}

CItem * CONTROLLER:: duplicate()
{
   CONTROLLER *new_one;
   new_one=new CONTROLLER;
   *new_one=*this;
   new_one->owner=NULL;
   new_one->in_robot_shell=NULL;
   new_one->rename(this->name);
   level.add_to_items_on_map(new_one);
   level.all_items.push_back(new_one);
   return (CItem *) new_one;
}

CItem * CPill:: duplicate()
{
	CPill *new_one;
	new_one=new CPill;
	*new_one=*this;

	map_of_unknown_items::iterator mit;
	mit=definitions.constantly_unknown_items.find(name);
	if (mit!=definitions.constantly_unknown_items.end())
	{
		new_one->color = (*mit).second.color;
	}
	
	new_one->owner=NULL;
	new_one->in_robot_shell=NULL;
	new_one->rename(this->name);
    level.add_to_items_on_map(new_one);
    level.all_items.push_back(new_one);
	return (CItem *) new_one;
}


CItem * CRangedWeapon::duplicate()
{
   CRangedWeapon *new_one;
   new_one=new CRangedWeapon;
   *new_one=*this;

   map_of_unknown_items::iterator mit;
   mit=definitions.constantly_unknown_items.find(name);
   if (mit!=definitions.constantly_unknown_items.end())
   {
	   new_one->color = (*mit).second.color;
   }
   
   new_one->owner=NULL;
   new_one->in_robot_shell=NULL;
   new_one->rename(this->name);   
   new_one->ammo.in_weapon = new_one;
   level.add_to_items_on_map(new_one);
   level.all_items.push_back(new_one);
   return (CItem *) new_one;
}

CItem * CAmmo::duplicate()
{
   CAmmo *new_one;
   new_one=new CAmmo;
   *new_one=*this;

   map_of_unknown_items::iterator mit;
   mit=definitions.constantly_unknown_items.find(name);
   if (mit!=definitions.constantly_unknown_items.end())
   {
	   new_one->color = (*mit).second.color;
   }
   
   new_one->owner=NULL;
   new_one->in_robot_shell=NULL;
   new_one->in_weapon=NULL;
   new_one->rename(this->name);   
   level.add_to_items_on_map(new_one);
   level.all_items.push_back(new_one);
   return (CItem *) new_one;
}

CItem * CArmor::duplicate()
{
   CArmor *new_one;
   new_one=new CArmor;
   *new_one=*this;

   map_of_unknown_items::iterator mit;
   mit=definitions.constantly_unknown_items.find(name);
   if (mit!=definitions.constantly_unknown_items.end())
   {
	   new_one->color = (*mit).second.color;
   }
   
   new_one->owner=NULL;
   new_one->in_robot_shell=NULL;
   new_one->rename(this->name);   
   level.add_to_items_on_map(new_one);
   level.all_items.push_back(new_one);
   return (CItem *) new_one;
}

CItem * NO_ARMOR::duplicate()
{
   NO_ARMOR *new_one;
   new_one=new NO_ARMOR;
   *new_one=*this;

   map_of_unknown_items::iterator mit;
   mit=definitions.constantly_unknown_items.find(name);
   if (mit!=definitions.constantly_unknown_items.end())
   {
	   new_one->color = (*mit).second.color;
   }
   
   new_one->owner=NULL;
   new_one->in_robot_shell=NULL;
   new_one->rename(this->name);   
   level.add_to_items_on_map(new_one);
   level.all_items.push_back(new_one);
   return (CItem *) new_one;
}

CItem * CBaseArmor::duplicate()
{
   CBaseArmor *new_one;
   new_one=new CBaseArmor;
   *new_one=*this;

   map_of_unknown_items::iterator mit;
   mit=definitions.constantly_unknown_items.find(name);
   if (mit!=definitions.constantly_unknown_items.end())
   {
	   new_one->color = (*mit).second.color;
   }
   
   new_one->owner=NULL;
   new_one->in_robot_shell=NULL;
   new_one->rename(this->name);   
   level.add_to_items_on_map(new_one);
   level.all_items.push_back(new_one);
   return (CItem *) new_one;
}

CItem * CHandWeapon::duplicate()
{
   CHandWeapon *new_one;
   new_one=new CHandWeapon;
   *new_one=*this;

   map_of_unknown_items::iterator mit;
   mit=definitions.constantly_unknown_items.find(name);
   if (mit!=definitions.constantly_unknown_items.end())
   {
	   new_one->color = (*mit).second.color;
   }
   
   new_one->owner=NULL;
   new_one->in_robot_shell=NULL;
   new_one->rename(this->name);
   level.add_to_items_on_map(new_one);
   level.all_items.push_back(new_one);
   return (CItem *) new_one;
}

CItem * CCorpse::duplicate()
{
   CCorpse *new_one;
   new_one=new CCorpse;
   *new_one=*this;

   map_of_unknown_items::iterator mit;
   mit=definitions.constantly_unknown_items.find(name);
   if (mit!=definitions.constantly_unknown_items.end())
   {
	   new_one->color = (*mit).second.color;
   }
   
   new_one->owner=NULL;
   new_one->in_robot_shell=NULL;
   new_one->rename(this->name);
   level.add_to_items_on_map(new_one);
   level.all_items.push_back(new_one);
   return (CItem *) new_one;
}

void FIRE::act_on_monster()
{
   int random_value;
   CMonster *monster;
   monster=level.monster_on_cell(pX(),pY());
   if (monster!=NULL)
   {
     random_value = random(density/4) + density/4;
     if (random_value!=0)
       monster->hit_changes_status(TYPE_INCENDIARY,random_value);
   }
}

void GAS::act_on_monster()
{
	CMonster *monster;
	monster=level.monster_on_cell(pX(),pY());
	if (monster!=NULL)
	{
		if (properties!=TYPE_SMOKE) // gdy nie tylko sam dym
		{
			if (density>200)
				density=200;
			monster->hit_changes_status(this->properties,density/40);
		}
	}
}

void FIRE::act_on_items()
{
    ptr_list on_cell;
    ptr_list::iterator m,_m;
    CItem *item;
    int random_value;

    level.list_of_items_from_cell(&on_cell,pX(),pY());
    for(m=on_cell.begin(),_m=on_cell.end(); m!=_m; m++)
    {
      item=(CItem *)*m;
      random_value=random(density/3);
      if (random_value>0)
        item->damage_it(random_value);
    }
}

bool CRobotShell::install_CPU(CItem *item)
{
	CIntelligent *CPU_owner=NULL;
	CProcessor *proc;

	if (item==NULL)
		return false;	

	proc=item->IsRobotCPU();
	if (proc==NULL)
		return false;

	if (proc->owner!=NULL)
		CPU_owner = proc->owner->IsIntelligent();

	if (CPU_owner!=NULL)
		CPU_owner->drop_item(proc,false);

	// lays on map, so move to robot
	level.remove_from_items_on_map(proc);
	cpu = proc;
	cpu->in_robot_shell = this;
	return true;
}

bool CRobotShell::install_in_action_slot(CItem *item)
{
	CIntelligent *item_owner=NULL;

	if (item==NULL)
		return false;
	
	if (action_slots.size()>=max_number_action_slots)
		return false;

	if (item->owner!=NULL)
		item_owner = item->owner->IsIntelligent();
	
	if (item_owner!=NULL)
		item_owner->drop_item(item,false);
	
	// lays on map, so move to robot
	level.remove_from_items_on_map(item);	

	action_slots.push_back(item);
	item->in_robot_shell = this;	
	return true;
}

bool CRobotShell::install_in_move_slot(CItem *item)
{
	CIntelligent *item_owner=NULL;
	CRobotLeg *leg;
	
	if (item==NULL)
		return false;

	leg = item->IsRobotLeg();
	if (leg==NULL)
		return false;
	
	if (move_slots.size()>=max_number_move_slots)
		return false;
	
	if (item->owner!=NULL)
		item_owner = item->owner->IsIntelligent();
	
	if (item_owner!=NULL)
		item_owner->drop_item(item,false);
	
	// lays on map, so move to robot
	level.remove_from_items_on_map(item);	
	
	move_slots.push_back(item);
	item->in_robot_shell = this;		
	return true;
}

bool CRobotShell::uninstall_CPU()
{
	if (cpu==NULL)
		return false;

	separate_item(cpu);	
	cpu = NULL;	
	CRobot *rob;
	rob = owner->IsRobot();
	if (rob!=NULL) 
	{
		if (rob->shell == this)
		{
			rob->death();
		}
	}
	return true;
}

bool CRobotShell::uninstall_from_action_slot(CItem *item)
{
	if (item==NULL)
		return false;

	separate_item(item);

	action_slots.remove(item);	
	return true;
}

bool CRobotShell::uninstall_from_move_slot(CItem *item)
{
	if (item==NULL)
		return false;
	
	separate_item(item);
	move_slots.remove(item);
	return true;
}

bool CRobotShell::separate_item(CItem *item)
{
	// if shell has an owner, then put it to the owner's backpack

	item->in_robot_shell=NULL;
	
	// lays on map, so move to robot
	if (this->owner!=NULL)
	{
		item->ChangePosition(owner->pX(),owner->pY());
		level.add_to_items_on_map(item);
		CIntelligent *intel = owner->IsIntelligent();
		if (intel!=NULL)
			intel->pick_up_item(item,false);
	}
	else
	{
		item->ChangePosition(pX(),pY());	
		level.add_to_items_on_map(item);	
	}
	return true;
}

void CRobotShell::death()
{
	CRobot *rob = NULL;
	if (owner!=NULL)
		rob = owner->IsRobot();
	if (rob!=NULL)
	{
		if (rob->seen_now)
		{
			CMonster *sound=(CMonster *) definitions.find_monster(rob->name);
			if (sound!=NULL)
				sounds.PlaySoundOnce(sound->sound_dying);		

		}
		rob->death();
	}
	owner = NULL;	

	CItem *temp;
	// uninstall all
	uninstall_CPU();
	while(action_slots.size()>0)
	{
		temp = (CItem *) *action_slots.begin();
		uninstall_from_action_slot(temp);
	}
	while(move_slots.size()>0)
	{
		temp = (CItem *) *move_slots.begin();
		uninstall_from_move_slot(temp);
	}
	
	CItem::death();
}
