// #define PLAYER_IS_IMMORTAL
// #define SHOW_TURN_NUMBER

#include "mem_check.h"

#include "monster.h"
#include "map.h"
#include "keyboard.h"
#include "actions.h"
#include "system.h"
#include "global.h"
#include "directions.h"
#include "options.h"
#include "attrib.h"
#include "level.h"
#include "places.h"
#include "parser.h"
#include "sounds.h"

#include <list>
using namespace std;

extern void make_screenshot(string filename,bool put_date);

#include <stdlib.h> // dla exit(0)

// z pliku global.cpp
extern CGame game;
extern CKeyboard keyboard;
extern CLevel level;
extern CMyScreen screen;
extern DEFINITIONS definitions;
extern CPlaces places;
extern class COptions options;
extern CDescriptions descriptions;
extern CSounds sounds;

extern void turn_on_wait_for_key();
extern void turn_off_wait_for_key();

CHero :: CHero()
{
	ClassName = "HERO";
	UniqueNumber = PLAYER_UNIQUE_NUMBER;

  tile='@';
  color=14;

  size=100;
  weight=80000;

  stop_repeating();

  rename("Player");
   // player x and y position
  ChangePosition(-1,-1);

  hit_points.val=23;
  hit_points.max=23;

  strength.val=12;
  dexterity.val=8;
  endurance.val=10;
  intelligence.val=10;
  speed.val=30;

  unarmed.rename("Bare Hands");
  unarmed.DMG=5;
  unarmed.HIT=3;
  unarmed.DEF=1;

  no_armor.rename("Skin");
  no_armor.ARM=1;
  no_armor.owner=this;

  last_activated=NULL;
  enemy=NULL;
  
  experience=0;
  level_at_experience=500;
  exp_level=1;
  free_skill_pointes=150;
  adaptation_points = 0;

  ID_of_level = "";

  next_action_time=0;
  turns_of_rest=0;

  group_affiliation = GROUP_HERO;

  for (int a=0;a<NUMBER_OF_RESISTS;a++)
     experience_in_skill[a]=0;

  action_to_repeat=REPEAT_ACTION_NONE;  
}


void CHero :: give_experience(unsigned long value)
{
  experience += value;
  free_skill_pointes += value;
  while( experience >= level_at_experience)
    got_level_up();
}

void CHero :: got_level_up()
{
  exp_level++;

// incrase attributes
  screen.console.add("* You've just reached a new level! *\n",10);
  sounds.PlaySoundOnce("data/sounds/other/levelup.wav",255);

  if (exp_level%3==0)
  {
	  strength.val++;
	  strength.max++;
	  dexterity.val++;
	  dexterity.max++;
	  speed.val++; 
	  speed.max++; 
	  screen.console.add("* You feel stronger and faster! *\n",10);
  }
  if (exp_level%4==0)
  {
	  endurance.val++; 
	  endurance.max++; 
	  screen.console.add("* You are hardened *\n",10);
  }
  level_at_experience*=2;
  int random_value = random(3)+1;

// proportional incrase of metabolism.
  int new_metabolism = ((metabolism.max+1)*(hit_points.max+random_value))/hit_points.max; 
  metabolism.val += (new_metabolism - metabolism.max);
  metabolism.max = new_metabolism;

  hit_points.max += random_value;
  hit_points.val += random_value;

// add adaptation points

  if (exp_level%3==0)
	adaptation_points+=(endurance.val/4);
}


void CHero :: show_attributes()
{
   string text,exp;
   set_color(7);
   text="HP: " + IntToStr(hit_points.GetValue()) + "/" + IntToStr(hit_points.max) + " EP: ";
   if (energy_points.val>0)
	   text+=IntToStr(energy_points.GetValue());
   else
	   text+='-';
   print_text(0,47,text);

   if (have_low_hp())
   {
	   if (have_critically_low_hp())
	   {
		   sounds.PlayHeartBeat();
		   set_color(4);
	   }
	   else
	   {
		   set_color(14);
		   sounds.StopHeartBeat();
	   }
	   text=IntToStr(hit_points.GetValue());
	   print_text(4,47,text);
	   set_color(7);	   
   }
   else
	   sounds.StopHeartBeat();
   
   if (options.number[OPTION_EXPERIENCE]==false)
		exp = " EXP: " + IntToStr(exp_level) + "/" + IntToStr(experience) + " (" + IntToStr(free_skill_pointes) + ")";
   else
		exp = " EXP: " + IntToStr(exp_level) + "\\" + IntToStr(level_at_experience - experience) + " (" + IntToStr(free_skill_pointes) + ")";

   text="STR: " + IntToStr(strength.GetValue()) + " DEX: " + IntToStr(dexterity.GetValue());
   int speed_pos = text.size();
   text += " SPD: " + IntToStr(speed.GetValue()) + " END: " + IntToStr(endurance.GetValue()) + " INT: " + IntToStr(intelligence.GetValue()) + exp;
   print_text(20,47,text);
   if (isBurdened())
   {
	   if (isStrained())
		   set_color(12);
	   else
		   set_color(14);	   
	   print_text(21+speed_pos,47,"SPD");
	   set_color(7);	   
   }

#ifdef SHOW_TURN_NUMBER
   print_text(70,48, "T: " +IntToStr(next_action_time));
#endif

   // show level name

   print_text(79-level.levels[ID_of_level].name.size(),49,level.levels[ID_of_level].name);
   
   if (weapon!=NULL)
   {
	   text="x " + (weapon->CItem::show_name()).substr(0,25);
	  
      if (weapon->property_load_ammo())
      {		
		text="x " + (weapon->CItem::show_name()).substr(0,17);
		CRangedWeapon *ranged_weapon = weapon->IsShootingWeapon();
		if (ranged_weapon!=NULL)
		{
			text+=" " + IntToStr ( ranged_weapon->ammo.quantity );
			text+="/" + IntToStr ( ranged_weapon->magazine_capacity);
			text+=" ";

			switch( ranged_weapon->fire_type)
			{
			  case FIRE_SINGLE:
				text+="(1)";
				break;
			  case FIRE_DOUBLE:
				text+="(2)";
				break;
			  case FIRE_TRIPLE:
				text+="(3)";
				break;
			  case FIRE_BURST:
				text+="(B)";
				break;
			}
		} // endof if
      }

	  if (weapon->energy_activated==1)
		set_color(13);
	  else
  	    set_color(7);
	  
      print_text(20,48,text);
	  weapon->draw_at(20,48);
   }

// armor always !=NULL
   text=(armor->CItem::show_name()).substr(0,25);   

   if (armor->hit_points < armor->max_hit_points/2)
   {
	   if (armor->hit_points <= armor->max_hit_points/4)
	   {
			   set_color(4);
	   }
	   else
	   {
		   if (armor->energy_activated==1)
			   set_color(5);
		   else
			   set_color(14);
	   }
   }
   else
   {
	   if (armor->energy_activated==1)
		   set_color(13);
	   else
		   set_color(7);
   }

   print_text(53,48,text);
   armor->draw_at(51,48);
   
   // print enemy
   if (enemy!=NULL)
   {
      set_color(7);
      print_text(20,49,"Target: ");
	  
	  if (enemy->have_low_hp())
	  {
		  if (enemy->have_critically_low_hp())
			  set_color(4);
		  else
			  set_color(14);
	  }
	  else
		  set_color(9);
	  
		  
      print_text(28,49,enemy->name);
   }

   // print status
   if (IsRadiated())
   {
       set_color(14);
	   print_text(0,49,"RAD");
   }
   if (IsPoisoned())
   {
       set_color(2);
	   print_text(5,49,"Pois");
   }
   if (IsBurning())
   {
       set_color(4);
	   print_text(10,49,"Burn");
   }
   else if (IsFreezed())
   {
       set_color(15);
	   print_text(10,49,"Cold");
   }
   if (IsSlowed())
   {
       set_color(1);
	   print_text(15,49,"Slow");
   }
   else if (IsHasted())
   {
       set_color(9);
	   print_text(15,49,"Fast");
   }
   
}

void CHero :: display()
{
   CTile :: display();
   show_attributes();
}

void CHero::stop_repeating()
{
	turns_of_rest=0;
	travel_dest_x = -1;
	travel_dest_y = -1;
	action_to_repeat=REPEAT_ACTION_NONE;
    inventory_repeat_break=true;
}

void CHero::repeat_action(ERepeatAction a_to_repeat)
{
	action_to_repeat=a_to_repeat;
}


EAction CHero :: get_action()
{	
	  if (IsStunned())
	  {
		  level.map.UnSeenMap();
		  level.map.HideFromPlayerSight();
		  
		  ptr_list::iterator m,_m;
		  CMonster *temp;
		  
		  for(m=level.monsters.begin(),_m=level.monsters.end(); m!=_m; m++)
		  {
			  temp=(CMonster *)*m;
			  if (temp!=level.player)
				  temp->seen_now = false;
		  }			  
		  return ACTION_WAIT_STUNNED;
	  }

	  // repeating

	  if (action_to_repeat==REPEAT_ACTION_REST)
	  {
		  if (turns_of_rest>0)
		  {
			  turns_of_rest--;
			  return ACTION_WAIT;
		  }
		  else
			  repeat_action(REPEAT_ACTION_NONE);
	  }

	  // show screen before player movement
	  // do fov calculation
	  look_around();

	  if (action_to_repeat==REPEAT_ACTION_EXPLORE)
	  {
		  // set_direction_to_unexplored
		  if (set_direction_to_closest_unexplored())
		  {
			  screen.draw();
			  myrefresh();
			  delay(20);
			  return ACTION_MOVE;
		  }
		  else
		  {
			screen.console.add("Exploration finished.",7);
			return ACTION_NOTHING;
		  }
	  }
	  if (action_to_repeat==REPEAT_ACTION_TRAVEL)
	  {
		  if (set_direction_to_cell_by_shortest_path(travel_dest_x,travel_dest_y,true))
		  {
			  screen.draw();
			  myrefresh();
			  delay(20);
			  return ACTION_MOVE;
		  }
		  else
		  {
			  stop_repeating();
			  return ACTION_NOTHING;
		  }
	  }

	  if (options.number[OPTION_AIM_IF_NO_ENEMY]==true && enemy==NULL)
		  select_closest_enemy();
	  
	  if (options.number[OPTION_AUTO_AIM]==true)
		  select_closest_enemy();

	  if (level.player->is_repeating_action()==false)
	  {
		  screen.clear_all();						
		  screen.draw();
		  screen.console.clean();
		  
		  // print items pod nogami po of movement
		  if (last_pX!=pX() || last_pY!=pY())
		  {
			  level.player->show_laying_items();
		  }
		  
		  screen.console.show();
		  screen.console.zero();
	  }

	  int key=0;
	  if (is_repeating_action()==false)
	  {
		  if (options.number[OPTION_CURSOR]==true)
		  {
			  show_cursor();
			  print_text(level.player->pX(),level.player->pY(),"");
			  set_cursor(level.player->pX(),level.player->pY());
		  }		  
		  key=keyboard.wait_for_key();
		  if (options.number[OPTION_CURSOR]==true)
		  {
			  hide_cursor();
		  }		  
	  }      

      if (key==keyboard.n)
      {
        direction=_kn;
        return ACTION_MOVE;
      }
      else if (key==keyboard.ne)
      {
        direction=_kne;
        return ACTION_MOVE;
      }
      else if (key==keyboard.e)
      {
        direction=_ke;
        return ACTION_MOVE;
      }
      else if (key==keyboard.se)
      {
        direction=_kse;
        return ACTION_MOVE;
      }
      else if (key==keyboard.s)
      {
        direction=_ks;
        return ACTION_MOVE;
      }
      else if (key==keyboard.sw)
      {
        direction=_ksw;
        return ACTION_MOVE;
      }
      else if (key==keyboard.w)
      {
        direction=_kw;
        return ACTION_MOVE;
      }
      else if (key==keyboard.nw)
      {
        direction=_knw;
        return ACTION_MOVE;
      }
      else if (key==keyboard.quit)
      {
		  screen.console.add_and_zero("Are you sure you want to QUIT WITHOUT SAVING? (Yes/No).\n",7);
		  if (keyboard.yes_no()==true)
			  return ACTION_QUIT;

		  return ACTION_NOTHING;		  
      }
      else if (key==keyboard.save && level.ID!="Tutorial")
      {
		  screen.console.add_and_zero("Are you sure you want to save and quit? (Yes/No).\n",7);
		  if (keyboard.yes_no()==true)
			  return ACTION_SAVE;
		  
		  return ACTION_NOTHING;		  
      }
      else if (key==keyboard.exchange)
      {
		  set_ready_weapon();
		  return ACTION_NOTHING;
      }	  
      else if (key==keyboard.up)
      {
        return ACTION_STAIRS_UP;
      }
      else if (key==keyboard.down)
      {
        return ACTION_STAIRS_DOWN;
      }
      else if (key==keyboard.rest)
      {
		  return ACTION_REST;
      }
	  else if (key==keyboard.explore)
	  {
		  return ACTION_EXPLORE;
	  }
	  else if (key==keyboard.travel)
	  {
		  return ACTION_TRAVEL;
	  }
      else if (key==keyboard.wait)
      {
        return ACTION_WAIT;
      }
      else if (key==keyboard.get)
      {
        return ACTION_GET;
      }
      else if (key==keyboard.look)
      {
        return ACTION_LOOK;
      }
	  else if (key==keyboard.show_visible)
	  {
		  print_visible_monsters_and_items();
		  return ACTION_NOTHING;
	  }	  
	  else if (key==keyboard.messagebuffer)
	  {
		  return ACTION_SHOW_MESSAGE_BUFFER;
	  }
      else if (key==keyboard.inventory || action_to_repeat==REPEAT_ACTION_INVENTORY)
      {
        return ACTION_INVENTORY;
      }
      else if (key==keyboard.fire)
      {
        return ACTION_FIRE;
      }
      else if (key==keyboard.help)
      {
        return ACTION_SHOW_HELP;
      }
      else if (key==keyboard.char_info)
      {
        return ACTION_SHOW_INFO;
      }
      else if (key==keyboard.inc_fire)
      {
        return ACTION_INC_FIRE;
      }
      else if (key==keyboard.dec_fire)
      {
        return ACTION_DEC_FIRE;
      }
      else if (key==keyboard.options)
      {
        return ACTION_OPTIONS;
      }
      else if (key==keyboard.reload)
      {
        return ACTION_RELOAD;
      }
      else if (key==keyboard._throw)
      {
        return ACTION_THROW;
      }
      else if (key==keyboard.activate_weapon)
      {
		  return ACTION_ACTIVATE_WEAPON;
      }
      else if (key==keyboard.activate_armor)
      {
		  return ACTION_ACTIVATE_ARMOR;
      }	  
      else if (key==keyboard.escape)
      {
         enemy=NULL; // zero enemy
         return ACTION_NOTHING;
      }
      else if (key==keyboard.nearest)
      {
		  if (enemy==NULL)
            select_closest_enemy();
		  else // go through all of enemies
		  {
			  ptr_list::iterator m,_m;
			  CMonster *temp, *first=NULL, *next=NULL;
			  bool wybierz_kolejnego=false;
			  
			  for(m=level.monsters.begin(),_m=level.monsters.end(); m!=_m; m++)
			  {
				  temp=(CMonster *)*m;
				  if (temp->seen_now && this->is_enemy(temp))
				  {
					  if (wybierz_kolejnego==true)
					  {
						  next=temp;
						  break;
					  }
					  if (first==NULL)
						  first=temp;
					  if (temp==enemy)
						  wybierz_kolejnego=true;
				  }
			  }
			  if (next!=NULL)
				  enemy=next;
			  else
				  enemy=first;
		  }
         return ACTION_NOTHING;
      }
      else if (key==keyboard.open)
      {
        return ACTION_OPEN;
      }
      else if (key==keyboard.close)
      {
        return ACTION_CLOSE;
      }
      else if (key==keyboard.attack)
      {
		  return ACTION_ATTACK;
      }
	  
      return ACTION_NOTHING;      
}

bool CHero::print_visible_monsters_and_items()
{
	ptr_list::iterator m,_m;
	CMonster *monster;
	CItem *item;
	screen.console.add("\nSeen monsters: ",7);
	bool something_seen=false;

	for(m=level.monsters.begin(),_m=level.monsters.end(); m!=_m; m++)
	{
		monster=(CMonster *)*m;
		if (monster==this)
			continue;

		if (monster->seen_now)
		{
			if (is_enemy(monster)) // enemy
			{
				screen.console.add(string()+monster->tile,monster->color);
				screen.console.add(monster->name,7);
				something_seen=true;
			} 
		}	
	}
	if (!something_seen)
		screen.console.add("none.",7);

	// seen items
	something_seen=false;
	screen.console.add("\nSeen items: ",7);
	for(m=level.items_on_map.begin(),_m=level.items_on_map.end(); m!=_m; m++)
	{
		item=(CItem *)*m;

		if (level.map.seen_by_camera(item->pX(),item->pY()) || (level.map.seen(item->pX(),item->pY()) && distance(item->pX(),item->pY(),pX(),pY())<fov_radius.GetValue()))
		{
			if (!item->property_controller() && !item->invisible)
			{

				screen.console.add(string()+item->tile,item->color);
				screen.console.add(item->show_name(),7);
				something_seen=true;
			}
		}	
	}
	if (!something_seen)
		screen.console.add("none.\n",7);
	else
		screen.console.add("\n",7);
	return true;
}

bool CHero::look_around()
{
	level.map.HideFromPlayerSight();
	level.fov.Start(&level.map,level.player->pX(),level.player->pY(),level.player->fov_radius.GetValue());
	CMonster::look_around();
	string to_print;
	bool was_stop=false;

	for(int x=0; x<MAPWIDTH; x++)
		for(int y=0; y<MAPHEIGHT; y++)
		{
			if ( (level.map.seen(x,y) && distance(x,y,level.player->pX(),level.player->pY()) < level.player->fov_radius.GetValue()) ||
				  level.map.seen_by_camera(x,y))
			{
				// stop repeating if player sees a new item			 
				if (action_to_repeat!=REPEAT_ACTION_NONE)
				{
					if (!level.map.seen_once(x,y) && level.map.GetNumberOfItems(x,y)!=0)
					{
						ptr_list items;
						level.list_of_items_from_cell(&items,x,y);
						ptr_list::iterator m,_m;
						CItem *item;
						for (m=items.begin(),_m=items.end();m!=_m;++m)
						{
							CItem *item = (CItem *) *m;
							if (!item->property_controller() && !item->invisible) // gdy nie jest to sterujacy i gdy nie invisible
							{
								if (was_stop)
									to_print+=string(", ");
								else 
								{
									to_print+=string("Stop: ");
									was_stop=true;
								}
								to_print+=item->show_name();								
							}
						}
					}
				}
				level.map.setSeenByPlayer(x,y);			  		  
			}
		}		

    // seen monsters
	ptr_list::iterator m,_m;
	CMonster *current_monster;
	
	for(m=level.monsters.begin(),_m=level.monsters.end(); m!=_m; m++)
	{
		current_monster=(CMonster *)*m;	
		if (level.map.seen_by_player(current_monster->pX(),current_monster->pY()))
		{
			current_monster->seen_now=true;

			if (is_repeating_action())
			{
				if (current_monster!=level.player && current_monster->is_enemy(level.player)) // if player sees monster, then stop repeating
				{
					if (was_stop)
						to_print+=string(", ");
					else 
					{
						to_print+=string("Stop: ");
						was_stop=true;
					}
					to_print+=current_monster->name;								
				}
			}
			
			// to experience - player get for killing if seen recently
			current_monster->seen_last_time_in_turn=level.turn;
		}
		else // when not seen
		{
			current_monster->seen_now=false;
			if (current_monster==level.player->enemy) // current target
				level.player->enemy=NULL;
		}		
	}
	if (to_print.size()>0)
	{
		stop_repeating();
		to_print+='.';
		screen.console.add(to_print,7);
	}
	return true;
}



CItem *CHero :: choose_item_from_backpack(int character)
{
   ptr_list::iterator m,_m;
   CItem *temp;
   for(m=backpack.begin(),_m=backpack.end(); m!=_m; m++)
   {
       temp=(CItem *)*m;
       if (temp->inventory_letter == character)
         return temp;
   }
   return NULL;
}

void CHero :: show_laying_items()
{
   ptr_list::iterator m,_m;
   CItem *temp;
   ptr_list items;

   // check stairs
   STAIRS *stairs = level.map.StairsAt(pX(),pY());
   if (stairs!=NULL)
	   screen.console.add(stairs->name +"\n",15);

   // print items
   level.list_of_items_from_cell(&items,pX(),pY());
   if (items.size()==0) // when none
   {
     return;
   }
   else if (items.size()==1)
   {
     temp=(CItem *)*items.begin();
     screen.console.add(string("There's ") + temp->article_a() + temp->show_name() + " lying here.\n",7);
   }
   else if (items.size()<4)
   {
     screen.console.add("Things that are here:\n",7);
     for(m=items.begin(),_m=items.end(); m!=_m; m++)
     {
       temp=(CItem *)*m;
       screen.console.add(temp->article_a() + temp->show_name() + "\n",7);
     }
   }
   else
   {
     screen.console.add("Several items are lying here...",7);
   }
}

CItem * CHero :: choose_item_to_pick_up()
{
   ptr_list items;
   level.list_of_items_from_cell(&items,pX(),pY());
   if (items.size()==0)
   {
     screen.console.add("There is nothing to get.",7);
     return NULL;
   }
   
   if (items.size()==1)
     return (CItem *)*items.begin();

   // there are more
	return choose_item_from_list(items,"Select an item to get");
}

bool CHero::throw_item_to (int x, int y, CItem *item)
{
	CCountable *countable = item->IsCountable();
	if (countable!=NULL && countable->quantity>1)
	{
		int posy=12;
		struct SScreen_copy scr_copy;
		store_screen(&scr_copy);
		
		string text = " How many of " + item->show_name() + " to throw? ";
		screen.draw_box(2,40-text.size()/2-2,posy,40+text.size()/2+1,posy+2);
		set_color(15);
		print_text(40-text.size()/2,posy,text);
		set_color(2);
		print_character(40-text.size()/2-1,posy,'[');
		print_character(40+text.size()/2,posy,']');
		
		set_color(7);
		int value = keyboard.get_value(38,posy+1,4);

		restore_screen(&scr_copy);
		if (value==0)
			return false;
		
		if (value!=-1 && value<countable->quantity) // split countable
		{
			// duplicate item
			CCountable *new_one;
			int old_number = countable->quantity;
			countable->quantity=value;
			countable->ChangePosition(-1,-1);

			new_one = (CCountable *) countable->duplicate();
			
			countable->quantity=old_number-value;
			item=new_one;
		}		
	}		
	return CIntelligent::throw_item_to (x,y,item);
}

int CHero::drop_item(CItem *item,bool visible_dropping)
{
	CCountable *countable = item->IsCountable();
	item->ChangePosition(pX(),pY());
	if (countable!=NULL && countable->quantity>1)
	{
		int posy=12;
		struct SScreen_copy scr_copy;
		store_screen(&scr_copy);
		
		string text = " How many of " + item->show_name() + " to drop? ";
		screen.draw_box(2,40-text.size()/2-2,posy,40+text.size()/2+1,posy+2);
		set_color(15);
		print_text(40-text.size()/2,posy,text);
		set_color(2);
		print_character(40-text.size()/2-1,posy,'[');
		print_character(40+text.size()/2,posy,']');

		set_color(7);
		int value = keyboard.get_value(38,posy+1,4);

		restore_screen(&scr_copy);
		if (value==0)
			return false;

		if (value!=-1 && value<countable->quantity) // split countable
		{
			// duplicate item
			CCountable *new_one;
			int old_number = countable->quantity;
			countable->quantity=value;
			new_one = (CCountable *) countable->duplicate();
			countable->quantity=old_number-value;

			if (visible_dropping)
				if (this->seen_now==true)
				{
					text=this->name + " has dropped " + new_one->article_the() + new_one->show_name() + ".";
					screen.console.add(text,3,false);
					return true;
				}							
		}
	}
	return CIntelligent::drop_item(item,visible_dropping);
}


int CHero :: get_action_for_item(CItem *item)
{
       CItem *temp;
       int character;
       
       while (1)
       {
         character=keyboard.wait_for_key();
         if (character=='x')
         {
			 descriptions.show_description(item->name);
         }
         if (character=='u' && item->property_use())
         {
			 use_item(item);
			 return true;
         }
         if (character=='d' && item->property_drop())
         {
            drop_item(item,true);
            return true;
         }
         if (character=='t' && item->property_throw())
         {
            last_activated=item;
            if (do_action(ACTION_THROW)!=0)
              return true;
         }
         if (character=='p' && item->property_put_on())
         {
            if (item!=armor)
              set_armor(item);
            else
              set_armor(&no_armor);
            return true;
         }
         if (character=='w' && item->property_wield())
         {
            if (item!=weapon)
              set_weapon(item);
            else
              set_weapon(&unarmed);
            return true;
         }
         if (character=='a' && item->property_activate())
         {
            last_activated=item;
            screen.console.add(name + " has activated the " + item->show_name() + ".\n",3,false);
            item->activate();
            return true;
         }
         if (character=='n' && item->property_unload_ammo())
         {
			 if (item->unload_ammo(this)==true)
			 {
				 screen.console.add(name + " has unloaded the " + item->show_name() + ".\n",3,false);
				 return true;
			 }
         }
         if (character=='r' && item->property_ready())
         {
			 if (choose_ready_weapon(item)==true)
			 {
				 return true;
			 }
         }		 
         if (character=='b' && item->property_buildrobot())
         {
			 if (build_robot(item)==true)
			 {
				sounds.PlaySoundOnce("data/sounds/other/robot_on.wav");
				 return true;
			 }
			 return false;
         }		 
         if (character=='c' && item->property_program())
         {
			 if (reprogram(item->IsProgrammator())==true)
			 {
				 sounds.PlaySoundOnce("data/sounds/other/reprogram.wav");
				 return true;
			 }
			 return false;
         }		 
         if (character=='o' && item->property_buildrobot())
         {
			 if (turn_robot_on(item)!=1)
			 {
				 sounds.PlaySoundOnce("data/sounds/other/autorepair.wav");
				 return false;
			 }
			 return true;
         }		 
         if (character=='l' && item->property_load_ammo())
         {
			if (item->IsLoaded())
			{
				continue;
			}
            show_backpack(item, INV_SHOW_MATCH_AMMO);
            
            set_color(10);
            print_text(32-item->show_name().size()/2,0,string(" Select an ammo for ") + item->show_name() + " ");
            myrefresh();
            // choose ammo from backpack
            while (1)
            {
               character=keyboard.wait_for_key();
               if ((character>='a' && character<='z') || (character>='A' && character<='Z'))
               {
                  temp=choose_item_from_backpack(character);
                  if (temp!=NULL) // when something selected
                  {
					CAmmo *am = temp->IsAmmo();
                    if (am != NULL  ) // is it really an ammo?
                    {
						   switch( item->load_ammo(am))
						   {
							 case 1:
								  continue;
							 case 0:
								  screen.console.add(name + " has loaded the " + item->show_name() + ".\n",3,false);
								  return true;
						   }
                    } // endof if                       
                  } // endof if temp!=NULL
               }
               else if (character==keyboard.escape || character==keyboard.readmore || character==keyboard.readmore2)
               {
                  return false;
               }
            } // endof while(1)
            
            return true;
         }

         if (character=='m' && item->property_mend() && (item->hit_points!=item->max_hit_points || (item->IsRepairSet())) )
         {
			 CRepairKit *repset=item->IsRepairSet();
			 if (repset!=NULL)
			 {
				 if (repset->can_be_used())					 
				 {
					 if (repset->purpose==PURPOSE_REPAIR_HP)
					 {
						 show_backpack(item, INV_SHOW_REPAIR_KIT);
						 set_color(10);
						 print_text(26-item->show_name().size()/2,0,string(" Choose an item to repair with ") + item->show_name() + " ");
					 }
					 else if (repset->purpose==PURPOSE_REPAIR_FIX_ARMOR)
					 {
						 show_backpack(item, INV_SHOW_IMPROVE_ARMOR);
						 set_color(10);
						 print_text(26-item->show_name().size()/2,0,string(" Choose an armor to improve with ") + item->show_name() + " ");
					 }
				 }
			 }				 
			 else
			 {
				 show_backpack(item, INV_SHOW_MATCH_ITEM_TO_REPAIR);
				 set_color(10);
				 print_text(25-item->show_name().size()/2,0,string(" Choose an item to use to repair ") + item->show_name() + " ");
			 }
			 myrefresh();			 
			 // choose item for mend
			 while (1)
			 {
				 character=keyboard.wait_for_key();
				 if ((character>='a' && character<='z') || (character>='A' && character<='Z'))
				 {
					 temp=choose_item_from_backpack(character);
					 if (temp!=NULL && temp!=item) // if selected item
					 {
						 if (repset!=NULL)
						 {
							  if (temp->fix_with_item(item,this)==true)
							  {
								  screen.console.add(name + " has repaired the " + temp->show_name() + ".\n",3,false);
								  return true;
							  }
						 }
						 else if (item->fix_with_item(temp,this)==true)
						 {
							 screen.console.add(name + " has repaired the " + item->show_name() + ".\n",3,false);
							 return true;
						 }
						 else
							 continue;
					 } // endof if temp!=NULL
				 }
				 else if (character==keyboard.escape || character==keyboard.readmore || character==keyboard.readmore2)
				 {
					 return false;
				 }
			 } // endof while(1)
		 }			 
         if (character==keyboard.escape || character==keyboard.readmore || character==keyboard.readmore2)
            break;
       }
       return false;
}


int CHero :: show_actions_of_item(CItem *item)
{
   int cornerx;
   int cornery;
   int x,y,zm_x;
   int length, left, right, top, down;
   string text;

   cornerx=30;
   cornery=10;

   length=item->show_name().size();

   text=string("[ x ")+item->show_name()+" ]";

   left=cornerx;
   right=cornerx+text.size()+1;
   top=cornery;

   // count how much place takes printing vertically 
   item->print_attributes_vertical(left,top+2,&zm_x,&y);
   if (y>0)
   {
      if (right-left<16+zm_x)
        right=left+16+zm_x;
   }
   // place in the center of window
   cornerx=40-((right-left)/2);
   right=(cornerx+(right-left));
   left=cornerx;
   
   down=top+y+2;
   
   // show border
   screen.draw_box(2,left,top,right,down);
    
   // print 
   set_color(7);
   item->print_attributes_vertical(right-zm_x-1,top+2,&x,&y);
   
   x=left+2;

   set_color(15);
   print_text(left+1,cornery,text);
   set_color(2);
   print_character(left+1,cornery,'[');
   print_character(left+text.size(),cornery,']');
   
   item->draw_at(left+3,cornery);

   y=cornery+2;

      if (item->property_wield())
      {
		  if (item != weapon)
		  {
			  set_color(14);
			  print_text(x,y,"W");
			  set_color(6);
			  print_text(x+1,y,"ield");
		  }
		  else
		  {
			  set_color(6);
			  print_text(x,y,"Unwield");
			  set_color(14);
			  print_text(x+2,y,"W");
		  }
		  
         y++;
      }
      if (item->property_put_on())
      {
			  set_color(14);
			  print_text(x,y,"P");
			  set_color(6);
			  print_text(x+1,y,"ut on");
         y++;
      }
      if (item->property_drop())
      {
         set_color(14);
         print_text(x,y,"D");
         set_color(6);
         print_text(x+1,y,"rop");
         y++;
      }
      if (item->property_throw())
      {
         set_color(14);
         print_text(x,y,"T");
         set_color(6);
         print_text(x+1,y,"hrow");
         y++;
      }
      if (item->property_use())
      {
		  set_color(14);
		  print_text(x,y,"U");
		  set_color(6);
		  print_text(x+1,y,"se");
		  y++;
      }
      if (item->property_load_ammo() && !item->IsLoaded())
      {
         set_color(14);
         print_text(x,y,"L");
         set_color(6);
         print_text(x+1,y,"oad ammo");
         y++;
      }
      if (item->property_unload_ammo() && item->IsLoaded())
      {
         set_color(6);
         print_text(x,y,"UNload ammo");
         set_color(14);
         print_text(x+1,y,"N");
         y++;
      }
      if (item->property_activate())
      {
		  CHandWeapon *hw = item->IsHandWeapon();
		  CBaseArmor *arm = item->IsArmor();
		  bool to_activate=false;
		  
		  if (hw!=NULL || arm!=NULL || (hw==NULL && arm==NULL))
		  {
			  if ( (hw!=NULL && hw->energy_activated==0) || (arm!=NULL && arm->energy_activated==0) || (hw==NULL && arm==NULL))
				  to_activate=true;
		  }
		  if (to_activate)
		  {
			  set_color(14);
			  print_text(x,y,"A");
			  set_color(6);
			  print_text(x+1,y,"ctivate");			  
			  y++;
		  }
		  else if ( (hw!=NULL && hw->energy_activated==1) || (arm!=NULL && arm->energy_activated==1))
		  {
			  set_color(6);
			  print_text(x,y,"Deactivate");
			  set_color(14);
			  print_text(x+2,y,"A");
			  y++;
		  }
         
      }
      if (item->property_mend())
      {
		  CRepairKit *repset;
		  repset=item->IsRepairSet();
		  if ( item->hit_points!=item->max_hit_points || (repset!=NULL && repset->can_be_used()))
		  {
			  set_color(14);
			  print_text(x,y,"M");
			  set_color(6);
			  print_text(x+1,y,"end");
			  y++;
		  }
      }	  

      if (item->property_program())
      {
		  set_color(6);
		  print_text(x,y,"Program CPU");
		  set_color(14);
		  print_text(x+8,y,"C");
		  y++;
      }	  	  

      if (item->property_buildrobot())
      {
		  set_color(14);
		  print_text(x,y,"B");
		  set_color(6);
		  print_text(x+1,y,"uild Robot");
		  y++;
      }	  
      if (item->property_buildrobot())
      {
		  CRobotShell *r;
		  r=item->IsRobotShell();
		  if (r!=NULL && r->cpu!=NULL)
		  {
			  set_color(6);
			  print_text(x,y,"Turn On");
			  set_color(14);
			  print_text(x+5,y,"O");
			  y++;
		  }
      }	  
      if (item->property_ready() && item!=weapon && item!=ready_weapon)
      {
		  set_color(14);
		  print_text(x,y,"R");
		  set_color(6);
		  print_text(x+1,y,"eady");
		  y++;
      }	  
	  
	  set_color(6);
	  print_text(x,y,"Examine");
	  set_color(14);
	  print_text(x+1,y,"x");
	  y++;
		  
      myrefresh();
	  return 0;
}


int CHero :: pick_up_item(CItem *item, bool see_possible)
{
   string text;
   ptr_list::iterator m,_m;
   CItem *temp;
   char *letters="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
   string additional;
   // check if the item can be taken
   if (item==NULL)
     return false;
   switch (can_pick_up(item))
   {
      case 1:
           screen.console.add(string("This is too heavy for you.\n"),7);
           return false;           
      case 2:
           screen.console.add(string("You have no place to carry it.\n"),7);
           return false;
   }

   CCountable *countable = item->IsCountable();
   if (options.number[OPTION_GETALL]==false && see_possible && countable!=NULL && countable->quantity>1)
   {
	   int posy=12;
	   struct SScreen_copy scr_copy;
	   store_screen(&scr_copy);
	   
	   string text = " How many of " + item->show_name() + " to get? ";
	   screen.draw_box(2,40-text.size()/2-2,posy,40+text.size()/2+1,posy+2);
	   set_color(15);
	   print_text(40-text.size()/2,posy,text);
	   set_color(2);
	   print_character(40-text.size()/2-1,posy,'[');
	   print_character(40+text.size()/2,posy,']');
	   
	   set_color(7);
	   int value = keyboard.get_value(38,posy+1,4);
	   
	   restore_screen(&scr_copy);
	   if (value==0)
		   return false;
	   
	   if (value!=-1 && value<countable->quantity) // split countable
	   {
		   // duplicate item
		   CCountable *new_one;
		   int old_number = countable->quantity;
		   int old_x = countable->pX();
		   int old_y = countable->pY();
		   countable->ChangePosition(-1,-1);		   
		   countable->quantity=value;
		   new_one = (CCountable *) countable->duplicate();
		   countable->quantity=old_number-value;
		   countable->ChangePosition(old_x,old_y);
		   item = new_one;
	   }
   }	   

// Assign letter
   additional=":";

   for(m=this->backpack.begin(),_m=this->backpack.end(); m!=_m; m++)
   {
       temp=(CItem *)*m;
       additional+=temp->inventory_letter;
   }

// find not assigned

   int index;
   for (index=0;index<50;index++) // zalozmy, ze 50 liter, tyle nie bedzie - wyzej jest check, czy moze podniesc item
   {
      if (additional.find(letters[index])==-1)
        break;
   }
   
 // assign letter. Variable max_items_in_backpack guarantee range check
   item->inventory_letter=letters[index];


   if (CIntelligent :: pick_up_item(item,see_possible)==true)
   {
//      text=string("You get ") + item->show_name() +".\n";
//      screen.console.add(text,3);
   }
   return true;
}

// positions
#define INFO_BASIC_X      2
#define INFO_BASIC_Y      3
#define INFO_SKILLS_X     30
#define INFO_SKILLS_Y     3
#define INFO_STATUS_X     2
#define INFO_STATUS_Y     14
#define INFO_INVENTORY_X  30
#define INFO_INVENTORY_Y  20
#define INFO_RESISTS_X    2
#define INFO_RESISTS_Y    24
#define INFO_EXPERIENCE_X 30
#define INFO_EXPERIENCE_Y 28

#define NUMBER_OF_ATTRIBS 5

void CHero :: show_info()
{
     int a,x,y;
     screen.clear_all();
  
     set_color(9);
                      
     set_color(2);
     print_text(0,0, "--------------------------------------------------------------------------------");
     print_text(0,49,"--------------------------------------------------------------------------------");
     set_color(10);
     print_text(33,0," Character info ");

   // print basic info ---------------------

     x=INFO_BASIC_X;
     y=INFO_BASIC_Y;

     screen.draw_box(2,x,y,x+18,y+NUMBER_OF_ATTRIBS+4);
     set_color(10);
     x+=2;
     print_text(x,y++, "BASIC INFO");
     
     y++;

     set_color(7);
     print_text(x,y++,"Name: " + name);
     y++;

     for (a=0;a<NUMBER_OF_ATTRIBS;a++)
     {
       set_color(8);
       print_text(x,y+a,"...............");
     }

	 if (strength.mod!=0)
		 set_color(12);
	 else
		 set_color(7);	 
     print_number_right_align(x+13,y,strength.GetValue(),2);
     print_text(x,y++,"Strength");

	 if (dexterity.mod!=0)
		 set_color(12);
	 else
		 set_color(7);	 
     print_number_right_align(x+13,y,dexterity.GetValue(),2);
     print_text(x,y++,"Dexterity");

	 if (speed.mod!=0)
		 set_color(12);
	 else
		 set_color(7);	 
	 
     print_number_right_align(x+13,y,speed.GetValue(),2);
     print_text(x,y++,"Speed");

	 if (endurance.mod!=0)
		 set_color(12);
	 else
		 set_color(7);	 
	 
     print_number_right_align(x+13,y,endurance.GetValue(),2);
     print_text(x,y++,"Endurance");

	 if (intelligence.mod!=0)
		 set_color(12);
	 else
		 set_color(7);	 
	 
     print_number_right_align(x+13,y,intelligence.GetValue(),2);
     print_text(x,y++,"Intelligence");


     // print skills -------------------------------------------
     
                      x=INFO_SKILLS_X;
                      y=INFO_SKILLS_Y;


                      screen.draw_box(2,x,y,x+23,y+NUMBER_OF_SKILLS+3);
                      x+=2;

                      for (a=0;a<NUMBER_OF_SKILLS;a++)
                      {
                        set_color(8);
                        print_text(x,y+a+2,"...................");
                        set_color(7);
                        print_number_right_align(x+17,y+a+2,skill[a],3);
                      }

                      set_color(10);
                      print_text(x,y++, "SKILLS");
                      y++;
                      set_color(7);
                      print_text(x,y++,"Small Guns");
                      print_text(x,y++,"Big Guns");
                      print_text(x,y++,"Energy Weapons");
                      print_text(x,y++,"Unarmed");
                      print_text(x,y++,"Melee Weapons");
                      print_text(x,y++,"Throwing");
                      print_text(x,y++,"Medicine");
                      print_text(x,y++,"Weapon Knowledge");
                      print_text(x,y++,"Alien Science");
                      print_text(x,y++,"Dodging");
                      print_text(x,y++,"Mechanic");
                      print_text(x,y++,"Programming");
					  

/// status info -------------------------------------

     x=INFO_STATUS_X;
     y=INFO_STATUS_Y;

     screen.draw_box(2,x,y,x+13,y+7);
     x+=2;
     set_color(10);
     print_text(x,y++,"STATUS");
     y++;

     set_color(8);

     char kolory_red[4]= {8,4,12,14};
     char kolory_blue[4]= {8,1,9,11};
     char kolory_green[4]= {8,2,10,14};
     int w;

           if (resist[RESIST_PARALYZE]!=0)
             w=abs(state[STATE_SPEED])/resist[RESIST_PARALYZE];
           else
             w=abs(state[STATE_SPEED]);
       
           if (w>3) w=3;
           if (w==0 && abs(state[STATE_SPEED])>0) w=1;
           set_color( kolory_blue[w] );
           print_text(x,y++,"Paralyzed");


           if (resist[RESIST_CHEM_POISON]!=0)
			   w=state[STATE_CHEM_POISON]/resist[RESIST_CHEM_POISON];
           else
             w=state[STATE_CHEM_POISON];
       
           if (w>3) w=3;
           if (w==0 && state[STATE_CHEM_POISON]>0) w=1;
           set_color( kolory_green[w] );
           print_text(x,y++,"Poisoned");


           
		   if (resist[RESIST_RADIOACTIVE]!=0)
			   w=state[STATE_RADIOACTIVE]/resist[RESIST_RADIOACTIVE];
           else
             w=state[STATE_RADIOACTIVE];
       
           if (w>3) w=3;
           if (w==0 && state[STATE_RADIOACTIVE]>0) w=1;
           set_color( kolory_green[w] );
           print_text(x,y++,"Radiated");


           if (resist[RESIST_BLIND]!=0)
             w=state[STATE_BLIND]/resist[RESIST_BLIND];
           else
             w=state[STATE_BLIND];
       
           if (w>3) w=3;
           if (w==0 && state[STATE_BLIND]>0) w=1;
           set_color( kolory_blue[w] );
           print_text(x,y++,"Blinded");           


		   w=state[STATE_TEMPERATURE];
		   if (w>0)
		   {
				if (resist[RESIST_FIRE]!=0)
					w=w/resist[RESIST_FIRE];
		   }
       
           if (w>3) w=3;
           if (w==0 && state[STATE_TEMPERATURE]>0) w=1;
           set_color( kolory_red[w] );
		   if (state[STATE_TEMPERATURE]>0)
				print_text(x,y++,"Burning");
		   else
				print_text(x,y++,"Freezed");


           // slowed lub speeded up

/// backpack, weapon info -------------------------------------

     x=INFO_INVENTORY_X;
     y=INFO_INVENTORY_Y;

     screen.draw_box(2,x,y,x+27,y+5);
     x+=2;
     set_color(10);
     print_text(x,y++, "INVENTORY");
     y++;

     for (a=0;a<3;a++)
     {
       set_color(8);
       print_text(x,y+a,".......................");
     }

       set_color(7);
       print_number_right_align(x+22,y,this->items_in_backpack,2);
       print_text(x,y++,"Number of items");
       print_number_right_align(x+22,y,this->get_backpack_weight(),2);
       print_text(x,y++,"Invent. weight");
       print_number_right_align(x+22,y,this->get_monster_capacity(),2);
       print_text(x,y++,"Max. max_capacity");
     


     // print resist info -------------------

     x=INFO_RESISTS_X;
     y=INFO_RESISTS_Y;

     screen.draw_box(2,x,y,x+20,y+11);
     x+=2;
     set_color(10);
     print_text(x,y++, "RESISTS");

     for (a=0;a<NUMBER_OF_RESISTS;a++)
     {
            set_color(8);
            print_text(x,y+a+1,"................");
            set_color(7);
            print_number_right_align(x+14,y+a+1,resist[a],3);
     }
     set_color(7);
     y++;
     print_text(x,y++,"Radiation");
     print_text(x,y++,"Chem Poison");
     print_text(x,y++,"Stun");
     print_text(x,y++,"Paralyze");
     print_text(x,y++,"Fire");
     print_text(x,y++,"Cold");
     print_text(x,y++,"Electricity");
     print_text(x,y++,"Blind");
     print_text(x,y++,"Confuse");


/// print experience info -------------------------------------

     x=INFO_EXPERIENCE_X;
     y=INFO_EXPERIENCE_Y;

     screen.draw_box(2,x,y,x+27,y+7);
     x+=2;
     set_color(10);
     print_text(x,y++, "EXPERIENCE");
     y++;

     for (a=0;a<5;a++)
     {
       set_color(8);
       print_text(x,y+a,".......................");
     }

       set_color(7);
       print_number_right_align(x+22,y,exp_level,2);
       print_text(x,y++,"Level");
       print_number_right_align(x+22,y,level_at_experience,2);
       print_text(x,y++,"Exp. for level");
       print_number_right_align(x+22,y,experience,2);
       print_text(x,y++,"Experience");
       print_number_right_align(x+22,y,free_skill_pointes,2);
       print_text(x,y++,"Free skill points");
       print_number_right_align(x+22,y,adaptation_points,2);
       print_text(x,y++,"Adaptation points");
	   
}

void CHero :: show_player_info()
{
     show_info();
     myrefresh();
	 keyboard.wait_for_key();
}


t_time CHero :: do_action( EAction action )
{
   int x,y;
   int returned;
   int key;
   string text;

   if (action==ACTION_NOTHING)
	   return 0;

   CRangedWeapon *ranged_weapon = weapon->IsShootingWeapon();

   last_pX = pX();
   last_pY = pY();   

   switch (action)
   {
      case ACTION_WAIT_STUNNED:
 					  return state[STATE_STUN];
      case ACTION_MOVE:
                      screen.draw_monsters(); // To make monsters know where player went
                      returned=move();
                      if (returned==1 || returned==3) // move or attack
                        return (TIME_MOVE/speed.GetValue())+1;
                      else if (returned==2) // can be opened
                      {
                        cell_in_current_direction(x,y);
						if (action_to_repeat!=REPEAT_ACTION_EXPLORE)
						{
							screen.console.add_and_zero("Do you want to open the " + level.map.getCellName(x,y) + "? (y/n)",7);
							returned=keyboard.wait_for_key();
						}
						else
							returned='y';

                        if (returned=='y' || returned=='Y')
                        {
                          if (open_in_direction()==true)
                            return (TIME_MOVE/speed.GetValue())+1;
                        }
                        else
                        {
                          screen.console.clean();
                          screen.console.show();
                          return 0;
                        }
                      
                      }
                      else
                      {
                        screen.console.add("Something is blocking your way.\n",7);
                        return 0;
                      }
      case ACTION_WAIT:
                      return (TIME_MOVE/speed.GetValue())+1;
      case ACTION_GET:
                      if (pick_up_item ( choose_item_to_pick_up(),true)==true)
                        return (TIME_MOVE/speed.GetValue())+1;
                      else
                        return 0;
      case ACTION_INVENTORY:
					  inventory_repeat_break=false;
                      if (show_inventory()==true)
					  {
						  if (!inventory_repeat_break)
							  repeat_action(REPEAT_ACTION_INVENTORY);
						  return (TIME_MOVE/speed.GetValue())+1;
					  }
                      else
					  {
						  stop_repeating();
						  return 0;
					  }
      case ACTION_FIRE:
                      if (!weapon->IsShootingWeapon())
                      {
                        screen.console.add(string("You can't shot with ") + weapon->article_the() + weapon->show_name() + ".\n",7);
                        return 0;
                      }
                        
                      // if enemy targeted, then fire
                      if (enemy==NULL)
                        enemy=choose_target(&x,&y, "Choose a target for " + weapon->show_name() + ".\n");
                      if (enemy!=NULL)
                      {
                         x=enemy->pX();
                         y=enemy->pY();
						 if (enemy==level.player)
							 enemy=NULL;
                      }
                      
					  if (x!=-1 && y!=-1)
					  {
						  shoot_into(x,y);						  
						  return (TIME_MOVE/speed.GetValue())+1;
					  }
					  return 0;
      case ACTION_SHOW_INFO:
                      show_player_info();
                      return 0;
      case ACTION_SHOW_HELP:
                      screen.clear_all();
                      set_color(11);                      
                      print_text(30,0,"XENOCIDE: The Roguelike");
                      set_color(9);
                      print_text(1,2 ,"DEFAULT KEYBOARD CONTROL:");
                      print_text(1,4 ,"c - close a door");
                      print_text(1,5 ,"e - instant exchange ready and wielded weapon");
                      print_text(1,6 ,"f - aim & fire weapon");
                      print_text(1,7 ,"g - get an item");
                      print_text(1,8 ,"i - inventory");
                      print_text(1,9 ,"l - look");
                      print_text(1,10,"n - aim nearest/next enemy");
                      print_text(1,11,"o - open a door, open a robot to turn it on/off");
                      print_text(1,12,"r - reload your weapon");
                      print_text(1,13,"t - throw last activated item");
                      print_text(1,14,"A - Attack something near you (monster, terrain)");
                      print_text(1,15,"M - show message buffer");
                      print_text(1,16,"Q - Quit game");
                      print_text(1,17,"R - Rest 100 turns, until healed or until important event");
                      print_text(1,18,"S - Save & Quit game");
                      print_text(1,19,"escape - cancel aiming, exit screens");
                      print_text(1,20,"! - change options");
                      print_text(1,21,"@ - character information");
                      print_text(1,22,"] - inc fire type (ranged weapon)");
                      print_text(1,23,"[ - dec fire type (ranged weapon)");
                      print_text(1,24,"; - activate weapon");
                      print_text(1,25,"' - activate armor");
                      print_text(1,26,"< - go up/use staris leading up");
                      print_text(1,27,"> - go down/use stairs leading down");
					  print_text(1,28,"E - Explore level (experimental)");
					  print_text(1,29,"X - Travel to location (experimental)");
					  print_text(1,30 ,"L - print visible monsters and items.");
                      print_text(1,35,"F11 - Take HTML screenshot.");
                      print_text(1,48,"In inventory press letter to gain Item Menu.");
                      myrefresh();
                      keyboard.wait_for_key();
                      screen.clear_all();
                      screen.draw2();
                      return 0;
      case ACTION_SHOW_MESSAGE_BUFFER:
					  screen.console.show_message_buffer(false);
					  return 0;
      case ACTION_INC_FIRE:						
                      if (ranged_weapon!=NULL)
                      {
                        ranged_weapon->incrase_fire_mode();
                        show_attributes();
                        myrefresh();
                      }
                      return 0;
      case ACTION_DEC_FIRE:
                      if (ranged_weapon!=NULL)
                      {
                        ranged_weapon->decrase_fire_mode();
                        show_attributes();
                        myrefresh();
                      }
                      return 0;
      case ACTION_REST:
				      repeat_action(REPEAT_ACTION_REST);
					  turns_of_rest = 100;
					  return 0;
	  case ACTION_EXPLORE:
					  repeat_action(REPEAT_ACTION_EXPLORE);
					  return 0;
	  case ACTION_TRAVEL:
  					  if (choose_travel_destination())
						  repeat_action(REPEAT_ACTION_TRAVEL);
					  return 0;
      case ACTION_OPTIONS:
                       screen.clear_all();
					   set_color(15);
					   print_text(1,1,"OPTIONS");
					   set_color(9);
					   print_text(1,25,"[D] Make them default");
					   set_color(14);
					   print_character(2,25,'D');
					   key=0;					   
                       while(1)
                       {
                         if (key==keyboard.escape)
                         {
						   CToSave::SaveOptions(string("save/") + name + ".opt");
                           break;
                         }
                         if (key=='d' || key=='D')
                         {
							 CToSave::SaveOptions("default.opt");
						 }							 
                         if (key-'0'>=0 && key-'0'<NUMBER_OF_OPTIONS)
                         {
                            options.number[key-'0']=!options.number[key-'0'];
                         }
                         for (int zz=0;zz<NUMBER_OF_OPTIONS;zz++)
                         {
                             switch(zz)
                             {
                               case OPTION_LETTERS:
									text=string("0: Constant letters in inventory: ");
                               break;
                               case OPTION_EXPERIENCE:
									text=string("1: Show experience needed for new level: ");
                               break;
                               case OPTION_AUTO_AIM:
								   text=string("2: Auto aim nearest enemy: ");
								   break;
                               case OPTION_DONT_LET_SWAP:
								   text=string("3: Don't let friendly monsters to swap place with you: ");
								   break;
                               case OPTION_CURSOR:
								   text=string("4: Show cursor on player: ");
								   break;
                               case OPTION_GETALL:
								   text=string("5: Always get the whole package of items: ");
								   break;
                               case OPTION_AIM_IF_NO_ENEMY:
								   text=string("6: Auto aim if no enemy selected: ");
								   break;
                             }
                             if (options.number[zz]==true)
                               text+="true ";
                             else
                               text+="false";
                             set_color(9);                               
                             print_text(1,5+zz,text);
                             set_color(14);                               
                             print_number(1,5+zz,zz);
                         } // endof for
                         myrefresh();
                         key=keyboard.wait_for_key();
                       } // endof while
                        screen.draw2();
                        return 0; 
      
      case ACTION_RELOAD:
                      if (reload_weapon()==true) // gdy pusty magazynek
                         return (TIME_MOVE/speed.GetValue())+1;
					  else if (ranged_weapon!=NULL) // gdy ma amunicje - wyladowanie jej
					  {
						  if (ranged_weapon->property_unload_ammo() && ranged_weapon->unload_ammo(this)==true)
						  {
							screen.console.zero();
							screen.console.add(name + " has unloaded the " + ranged_weapon->show_name() + ".\n",3,false);
							return (TIME_MOVE/speed.GetValue())+1;
						  }
					  }					  
                      else
                      {
                         screen.console.add("You can't reload " + weapon->article_the() + weapon->show_name() + ".\n",7,false);
                         return 0;
                      }
					  return 0;					  
      case ACTION_QUIT:
                      death();
      case ACTION_SAVE:
                      game.SaveGame();
					  return 0;
      case ACTION_STAIRS_UP:
					  if (use_stairs(true)==true)
						return TIME_LEVEL_CHANGE;
					  return 0;
      case ACTION_STAIRS_DOWN:
					  if (use_stairs(false)==true)
						return TIME_LEVEL_CHANGE;
					  return 0;
      case ACTION_LOOK:
                      look_with_cursor();
                      return 0;
      case ACTION_ACTIVATE_ARMOR:
		  if (armor->property_activate())
		  {
			  screen.console.add(name + " has activated the " + armor->show_name() + ".\n",3,false);
			  armor->activate();
			  return (TIME_MOVE/speed.GetValue())+1;
		  }		  
		  return 0;
      case ACTION_ACTIVATE_WEAPON:
		  if (weapon->property_activate())
		  {
			  screen.console.add(name + " has activated the " + weapon->show_name() + ".\n",3,false);
			  weapon->activate();
			  return (TIME_MOVE/speed.GetValue())+1;
		  }		  
		  return 0;
      case ACTION_THROW:
                      if (last_activated!=NULL)
                      {
                         if (last_activated->property_throw())
                         {
                           struct SScreen_copy scr_copy;
                           store_screen(&scr_copy);
						   screen.clear_all();
                           screen.draw2();						   
                           choose_target(&x,&y,"Throwing " + last_activated->article_the() + last_activated->show_name() + ".\n");
                           if (x==-1 || y==-1)
                           {
                              restore_screen(&scr_copy);
                              myrefresh();
                              return 0;
                           }
                           throw_item_to(x,y,last_activated);
                           last_activated=NULL;
                           return (TIME_MOVE/speed.GetValue())+1;
                         }
                      }
                      last_activated=NULL;                      
                      return 0;
                      
                      
      case ACTION_OPEN:
                      screen.console.add_and_zero("In which direction you want to open something?",7);
      
                      if (set_move_direction()==true)
                      {
						int ox,oy;
						CRobot *rob;
						CMonster *monst;
						CItem *przedm;
						CRobotShell *powl;
						cell_in_current_direction(ox,oy);
						ptr_list items_z_pola;
						ptr_list robot_shells;
						ptr_list::iterator om,_om;

						// is it a turning robot on/off?
						if (level.map.IsMonsterOnMap(ox,oy))
						{
							monst=level.monster_on_cell(ox,oy);
							rob = monst->IsRobot();
							if (rob!=NULL)
							{
								if (rob->is_enemy(this))
								{
									rob->tries_to_turn_off(this);
									return (TIME_MOVE/speed.GetValue())+1;								
								}
								else // friendly
								{
									build_robot(rob->shell);
									return (TIME_MOVE/speed.GetValue())+1;								
								}
							}
						}
						// or maybe robot building?
						robot_shells.clear();
						level.list_of_items_from_cell(&items_z_pola,ox,oy);
						if (items_z_pola.size()>0)
						{
							for (om=items_z_pola.begin(),_om=items_z_pola.end();om!=_om;om++)
							{
								przedm = (CItem *) *om;
								powl = przedm->IsRobotShell();
								if (powl!=NULL)
									robot_shells.push_back(powl);
							}
							if (robot_shells.size()==1)
							{
								powl = (CRobotShell *) *robot_shells.begin();
								int wynik_akcji = build_robot(powl);
								switch(wynik_akcji) {
								case 1: // budowa robota
									return (TIME_MOVE/speed.GetValue())+1;
								case 2: // show_inventory
									return (TIME_MOVE/speed.GetValue())+1;
								default:;
									return (TIME_MOVE/speed.GetValue())+1;
								}
																	
							}
							else if (robot_shells.size()>1)
							{
								przedm = choose_item_from_list(robot_shells,"Choose a shell to assembly a new robot");
								if (przedm!=NULL)
								{
									if (build_robot(przedm)==true)
										return (TIME_MOVE/speed.GetValue())+1;								
								}
								else
									return 0;
							}

						}

						// nope, it's door opening
                        if (open_in_direction()==true)
						{
                          return (TIME_MOVE/speed.GetValue())+1;
						}
						else
							screen.console.add("It can't be opened.",7);                        
                      }
                      return 0;
      case ACTION_CLOSE:
                      screen.console.add_and_zero("In which direction you want to close something?",7);
                      if (set_move_direction()==true)
                      {
                        if (close_in_direction()==true)
                          return (TIME_MOVE/speed.GetValue())+1;
                        
                      }
                      screen.console.add("It can't be closed.",7);
                      return 0;
      case ACTION_ATTACK:
					  screen.console.add_and_zero("In which direction you want to attack something?",7);
					  
					  if (set_move_direction()==true)
					  {
						  if (attack_in_direction()==true)
							  return (TIME_MOVE/speed.GetValue())+1;
						  
					  }
					  return 0;
		  
   }
   return 0;
}


bool CHero :: set_move_direction()
{
   int key;

   while(1)
   {
      key=keyboard.wait_for_key();

      if (key==keyboard.n)
      {
        direction=_kn;
        return true;
      }
      else if (key==keyboard.ne)
      {
        direction=_kne;
        return true;
      }
      else if (key==keyboard.e)
      {
        direction=_ke;
        return true;
      }
      else if (key==keyboard.se)
      {
        direction=_kse;
        return true;
      }
      else if (key==keyboard.s)
      {
        direction=_ks;
        return true;
      }
      else if (key==keyboard.sw)
      {
        direction=_ksw;
        return true;
      }
      else if (key==keyboard.w)
      {
        direction=_kw;
        return true;
      }
      else if (key==keyboard.nw)
      {
        direction=_knw;
        return true;
      }
      else if (key==keyboard.escape || key==keyboard.readmore)
        return false;
   } // endof while
}


void CHero :: look_with_cursor()
{
   int key;
   int x,y;
   int last_x,last_y;
   struct SScreen_copy scr_copy;
   CMonster *monst=NULL;
   ptr_list on_cell;
   ptr_list::iterator m,_m;
   CItem *temp;
   int changex, changey;
   
   if (enemy!=NULL)
   {
     x=enemy->pX();
     y=enemy->pY();
   }
   else
   {
     x=this->pX();
     y=this->pY();
   }
   last_x=-1; last_y=-1;
   
   screen.console.zero();
   screen.console.clean();
   screen.console.show();
   
   store_screen(&scr_copy); // remember screen

   changex=0;
   changey=0;
   key=0;
   
   while(1)
   {
      x+=changex;
      y+=changey;
      if (key==keyboard.escape)
      {
         restore_screen(&scr_copy);
         screen.console.clean();
         screen.console.zero();
         screen.console.show();
         return;
      }
      else if (key=='m' || key=='M')
      {
		  if (monst!=NULL)
		  {
			  monst->ShowDescription();
		  }
      }
      else if (key==keyboard.look || key==keyboard.readmore ||
               key==keyboard.readmore2)
      {
         restore_screen(&scr_copy);
         screen.console.clean();
         screen.console.zero();
         screen.console.show();
         return;
      }

     restore_screen(&scr_copy);
      
     // limit of movement
     if (x<0 || x>79 || y<0 || y>39)
     {
       x=last_x; y=last_y;
     }
     else
     {
       if (x!=last_x || y!=last_y)
       {
         on_cell.clear();
         restore_screen(&scr_copy);
         screen.console.zero();
         screen.console.clean();
         screen.console.show();
         store_screen(&scr_copy);
         if (!(level.map.seen_by_player(x,y) || level.map.seen_by_camera(x,y)))
         {
           screen.console.add("You don't see this place.",7);
           screen.console.show();
           myrefresh();
           store_screen(&scr_copy);           
         }
         else
         {
            // look on a new place
            // is it a monster?
			 CRobot *rob;
            monst=level.monster_on_cell(x,y);
            level.list_of_items_from_cell(&on_cell,x,y);
            if (monst!=NULL)
            {
				screen.console.add(string(" ") + monst->name,7);
				rob = monst->IsRobot();
				if (!rob)
				{
					screen.console.add(string(" / Weapon: ") + monst->weapon->show_name() ,7);
					screen.console.add(string(" / Armor: ") + monst->armor->show_name() + "\n",7);
					screen.console.add(string("Health: ") + IntToStr(100*monst->hit_points.val/monst->hit_points.max) + "% ",7);
				}
				else
				{
					screen.console.add(string("\nDamage: ") + IntToStr(100-(100*rob->shell->hit_points/rob->shell->max_hit_points)) + "% ",7);
				}
               screen.console.add("\n-- Hit 'M' for monster description. --",7);
               screen.console.show();
               screen.console.zero();
               myrefresh();
               store_screen(&scr_copy);
            }
            else if (on_cell.size()!=0) // items
            {
               if (on_cell.size()==1)
                  screen.console.add("There is:",7);
               else
                  screen.console.add("There are:",7);
               for(m=on_cell.begin(),_m=on_cell.end(); m!=_m;)
               {
                  temp=(CItem *)*m;
                  if (++m!=_m)
                     screen.console.add(temp->show_name()+",",temp->color);
                  else
                     screen.console.add(temp->show_name()+".",temp->color);
               }
               screen.console.show();
               screen.console.zero();
               myrefresh();
               store_screen(&scr_copy);
            }
            else // terrain
            {
               screen.console.add(level.map.getCellName(x,y),7);
               screen.console.add(string("| ") + IntToStr(level.map.getPercentDamage(x,y)) + "% of damage.",7);
               screen.console.show();
               screen.console.zero();
               myrefresh();
               store_screen(&scr_copy);
            }
         } // you see this

       }
       last_x=x; last_y=y;
     }
     print_character(x,y,'?');
     myrefresh();

	 key = keyboard.get_direction(changex, changey);
     
   } // endof while
}


void CHero :: show_backpack(CItem *passed_item, EInvShowType print_type)
{
   ptr_list::iterator m,_m;
   CItem *temp;
   int pos;
   screen.clear_all();
   set_color(7);
   int print_count;
   char *letters="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
   
   bool used;
   if (this->items_in_backpack==0)
   {
       print_text(1,2,string("You have no items."));
       myrefresh();
       return;
   }
   used=false;
   print_count=0;
   string text;
   set_color(2);
   print_text(0,0, "--------------------------------------------------------------------------------");
   print_text(0,49,"--------------------------------------------------------------------------------");
   set_color(10);   

   string total_weight = IntToStr(get_backpack_weight());
   string total_capacity = IntToStr(get_monster_capacity());

   text =  string(" [ ") +  total_weight + "/" + total_capacity + " ] ";
   print_text(40-text.size()/2,49,text);
   if (isStrained())
   {
	   set_color(4);
   }
   else if (isBurdened())
   {
	   set_color(14);
   }
   print_text(40-text.size()/2+3,49,total_weight);

   pos=2;

   string addit_hit_points;
			   
   if ( !this->is_unarmed() )
   {
	   set_color(15);

	   if (print_type == INV_SHOW_REPAIR_KIT ||
		   print_type == INV_SHOW_MATCH_ITEM_TO_REPAIR)
	   {
		   addit_hit_points = string(" <") + IntToStr(100*weapon->hit_points/weapon->max_hit_points) + "%>";
		   if (weapon->hit_points!=weapon->max_hit_points)
			   set_color(7);
		   else
			   set_color(8);
	   }

	   
	   if (options.number[OPTION_LETTERS]==false)
		   this->weapon->inventory_letter=letters[print_count];
	   this->weapon->print_item_with_letter(1,pos,string("  (your weapon)") + addit_hit_points);
	   used=true;
	   pos++;
	   print_count++;
   }
   if ( this->in_armor() )
   {
	   set_color(15);

	   if (print_type == INV_SHOW_REPAIR_KIT ||
		   print_type == INV_SHOW_MATCH_ITEM_TO_REPAIR)
	   {
		   addit_hit_points = string(" <") + IntToStr(100*armor->hit_points/armor->max_hit_points) + "%>";
		   if (armor->hit_points!=armor->max_hit_points)
			   set_color(7);
		   else
			   set_color(8);
	   }
		   
	   
	   if (options.number[OPTION_LETTERS]==false)
		   this->armor->inventory_letter=letters[print_count];
	   this->armor->print_item_with_letter(1,pos,string("  (your armor)") + addit_hit_points);
	   used=true;
	   pos++;
	   print_count++;
   }
   if ( this->ready_weapon!=NULL )
   {
	   set_color(7);

	   if (print_type == INV_SHOW_REPAIR_KIT ||
		   print_type == INV_SHOW_MATCH_ITEM_TO_REPAIR)
	   {
		   addit_hit_points = string(" <") + IntToStr(100*ready_weapon->hit_points/ready_weapon->max_hit_points) + "%>";
		   if (ready_weapon->hit_points!=ready_weapon->max_hit_points)
			   set_color(7);
		   else
			   set_color(8);
	   }
		   
	   
	   if (options.number[OPTION_LETTERS]==false)
		   ready_weapon->inventory_letter=letters[print_count];
	   ready_weapon->print_item_with_letter(1,pos,string("  (ready)") + addit_hit_points) ;
	   used=true;
	   pos++;
	   print_count++;
   }
   
   if (used==true)
   {
	   pos++;
	   set_color(2);
	   print_text(0,pos, "--------------------------------------------------------------------------------");
	   set_color(10);
	   print_text(30,0," Personal equipment ");
	   print_text(35,pos," Inventory ");
	   pos+=2;
   }
   else
   {
	   set_color(10);
	   print_text(35,0," Inventory ");
   }

   for(m=backpack.begin(),_m=backpack.end(); m!=_m; m++)
   {
       temp=(CItem *)*m;
       if (temp!=this->weapon && temp!=this->armor && temp!=ready_weapon) // juz wypisane
       {
		   CAmmo *am;
		   CRangedWeapon *ranged_weapon;
		   CRepairKit *repset;
		   CBaseArmor *armor;
		   CProgrammator *progr;

		   switch (print_type)
		   {
		   case INV_SHOW_MATCH_AMMO:
			   set_color(8);
			   am = temp->IsAmmo();
			   ranged_weapon = passed_item->IsShootingWeapon();
			   
			   if (am!=NULL)
			   {
				   if (am->ammo_type==ranged_weapon->ammo_type)
					   set_color(7);
			   }
			   break;
		   case INV_SHOW_HIGHLIGHT_CORPSES:
			   if (temp->IsCorpse()!=NULL)
				   set_color(7);
			   else
				   set_color(8);
			   break;
		   case INV_SHOW_MATCH_ITEM_TO_REPAIR:
			   set_color(8);
			   repset=temp->IsRepairSet();
				   
			   if (((repset!=NULL && repset->can_be_used()) || (passed_item->IsArmor() && temp->IsArmor() && temp!=passed_item)))
			   {
					   set_color(7);
			   }
			   break;			   
		   case INV_SHOW_REPAIR_KIT:
			   
			   if (!temp->IsCountable() && temp->hit_points!=temp->max_hit_points)
				   set_color(7);
			   else
				   set_color(8);			   
			   break;			   
		   case INV_SHOW_IMPROVE_ARMOR:

			   set_color(8);
			   
			   armor = temp->IsArmor();
			   if (armor!=NULL)
			   { 
				   if (armor->MOD_DEX<=0 || armor->MOD_STR<=0 || armor->MOD_SPD<=0)
					  set_color(7);
			   }			   
			   break;			   
		   case INV_SHOW_HIGHLIGHT_REAPIR_KITS:			   
			   repset=temp->IsRepairSet();
			   if (repset!=NULL && repset->can_be_used() && repset->purpose==PURPOSE_REPAIR_HP)
				   set_color(7);
			   else
				   set_color(8);			   
			   break;		
		   default:
			   set_color(7);
		   }

		   if (print_type == INV_SHOW_REPAIR_KIT ||
			   print_type == INV_SHOW_MATCH_ITEM_TO_REPAIR)
			   addit_hit_points = string(" <") + IntToStr(100*temp->hit_points/temp->max_hit_points) + "%>";
		   
         if (options.number[OPTION_LETTERS]==false)
            temp->inventory_letter=letters[print_count];
         temp->print_item_with_letter(1,pos,addit_hit_points);
         pos++;
         print_count++;
       }
   }
   myrefresh();
}

int CHero :: show_inventory()
{
   int character;
   ptr_list::iterator m,_m;
   CItem *temp;
   string text;
   show_backpack(NULL, INV_SHOW_SIMPLE);
   while (1)
   {
         character=keyboard.wait_for_key();
         if ((character>='a' && character<='z') || (character>='A' && character<='Z'))
         {
            temp=choose_item_from_backpack(character);
            if (temp!=NULL)
            {
               show_actions_of_item(temp);
               if (get_action_for_item(temp)==true)
                 break;
               show_backpack(NULL, INV_SHOW_SIMPLE);
            }
         }
         if (character==keyboard.escape || character==keyboard.readmore || character==keyboard.readmore2)
         {
            return false;
         }
   }
   return true;
}

int CHero :: shoot_into (int x, int y)
{

  CRangedWeapon *ranged_weapon = weapon->IsShootingWeapon();
  if (ranged_weapon!=NULL && ranged_weapon->ammo.quantity==0)
  {
        screen.console.add(string("Your ") + ranged_weapon->show_name() + " is out of ammo.\n",12);
        screen.console.show();

		CRangedWeapon *sound=(CRangedWeapon *) definitions.find_item(ranged_weapon->name);
		if (sound!=NULL)
			sounds.PlaySoundOnce(sound->sound_gun_empty);
        return false;
  }
  screen.console.clean();
  screen.console.add("You fire.\n",3);
  screen.console.show();
  CIntelligent :: shoot_into(x,y);
  return true;
}

CMonster * CHero :: choose_target(int *target_x, int *target_y, string text)
{
   int key;
   int x,y;
   int last_x,last_y;
   struct SScreen_copy scr_copy;
   int dx,dy;
   
   select_closest_enemy();
   
   if (enemy==NULL)
   {
	   x=this->pX();
	   y=this->pY();
   }
   else
   {
	   x=enemy->pX();
	   y=enemy->pY();
   }

   last_x=x; last_y=y;
   
   store_screen(&scr_copy); 
   
   while(1)
   {
	   screen.console.add_and_zero(text,7);
	   screen.draw_targeting_line(this->pX(),this->pY(),x,y);
   
      key = keyboard.wait_for_key();
	  dx=0;
	  dy=0;

      if (key==keyboard.n)
      {
		  dy--;
      }
      else if (key==keyboard.ne)
      {
		  dy--;
		  dx++;
      }
      else if (key==keyboard.e)
      {
		  dx++;
      }
      else if (key==keyboard.se)
      {
		  dx++;
		  dy++;
      }
      else if (key==keyboard.s)
      {
		  dy++;
      }
      else if (key==keyboard.sw)
      {
		  dy++;
		  dx--;
      }
      else if (key==keyboard.w)      
      {
		  dx--;
      }
      else if (key==keyboard.nw)
      {
		  dx--;
		  dy--;
      }
	  if (dx!=0 || dy!=0)
	  {
		  select_closest_enemy();
		  x+=dx;
		  y+=dy;
	  }
   
      if (key==keyboard.escape)
      {
         *target_x=-1;
         *target_y=-1;
         restore_screen(&scr_copy);
         myrefresh();
         return NULL;
      }
	  else if (key==keyboard.nearest)
	  {
		  ptr_list::iterator m,_m;
		  CMonster *temp, *first=NULL, *next=NULL;
		  bool wybierz_kolejnego=false;
		  
		  for(m=level.monsters.begin(),_m=level.monsters.end(); m!=_m; m++)
		  {
			  temp=(CMonster *)*m;
			  if (temp->seen_now && this->is_enemy(temp))
			  {
				  if (wybierz_kolejnego==true)
				  {
					  next=temp;
					  break;
				  }
				  if (first==NULL)
					  first=temp;
				  if (temp==enemy)
					  wybierz_kolejnego=true;
			  }
		  }
		  if (next!=NULL)
			  enemy=next;
		  else
			  enemy=first;		  

		  if (enemy!=NULL)
		  {
			  x=enemy->pX();
			  y=enemy->pY();
		  }
	  }
      else if (key=='t' || key=='T' || key==keyboard.fire ||
               key==keyboard.readmore || key==keyboard.readmore2)
      {
         *target_x=x;
         *target_y=y;
         restore_screen(&scr_copy);
         myrefresh();
         return level.monster_on_cell(x,y);
      }

     restore_screen(&scr_copy);
      
     // limit of movement
     if (x<0 || x>79 || y<0 || y>39)
     {
       x=last_x; y=last_y;
     }
     else
     {
       last_x=x; last_y=y;
     }     
   } // endof while
}

bool CHero::death()
{
	if (level.ID!="Tutorial")
	{
		myclear();
		screen.draw();
		screen.console.show();
#ifndef PLAYER_IS_IMMORTAL
		game.DeleteSaveGame();
#endif
		delete_file("death.htm");
		make_screenshot("death",true);
		show_info();
		make_screenshot("death",false);
		screen.console.show_message_buffer(true);
		make_screenshot("death",false);

		myclear();
		screen.draw();
		screen.console.show();

		set_color(14);
		sounds.StopHeartBeat();
		sounds.PlaySoundOnce("data/sounds/other/death.wav");
		print_text(1,1,string("YOU HAVE JUST DIED! (press any key to quit)"));
		myrefresh();
		keyboard.wait_for_key();

	}
	game.Quit();
	return true;
}

void CHero :: select_closest_enemy()
{
   int min_distance;
   int dist;
   CMonster *chosen_one=NULL;
   
   ptr_list::iterator m,_m;
   CMonster *temp;

   if (enemy==NULL)
   {
	   min_distance=10000;
   }
   else
   {
	   if (abs(pX()-enemy->pX())<2 && abs(pY()-enemy->pY())<2) // not to change monsters near by
		   min_distance=1;
	   else
		   min_distance=distance(enemy->pX(),enemy->pY(),pX(),pY());
	   chosen_one=enemy;
   }

   for(m=level.monsters.begin(),_m=level.monsters.end(); m!=_m; m++)
   {
       temp=(CMonster *)*m;
       if (temp!=level.player && is_enemy(temp)==true && temp->seen_now)
       {
			 dist = distance(temp->pX(),temp->pY(),pX(),pY());
             
             if (dist<min_distance)
             {
               min_distance=dist;
               chosen_one=temp;
             }
       }
   }
   enemy=chosen_one;
}

bool CHero::use_stairs(bool direction)
{
	int x,y;
	STAIRS * stairs = level.map.StairsAt(pX(),pY());
	if (stairs==NULL)
		return false;

	if (stairs->lead_up==direction)
	{
		if (stairs->to_where == "Genetic Machine")
		{
			places.GeneticLaboratory();
			return true;
		}
		// blok the cell - monsters will not enter the stairs
		x = level.player->pX();
		y = level.player->pY();
		
		level.map.setBlockMove(x,y);
		
		stairs_number_used=stairs->number;
		ID_of_last_level=level.ID;
		level.ChangeLevelTo(stairs->to_where);
		wait(5);
		return true;
	}

	return false;
}

void CHero::train_skill(int a_skill, int value)
{
	int ceiling;
	int procents;
	experience_in_skill[a_skill]+=value;
	ceiling = (skill[a_skill]/10)*(SKILL_MULTIPLIER*8) + 1; 
	skill[a_skill] = (skill[a_skill]/10) * 10;
	procents = experience_in_skill[a_skill]*10/ceiling; 
	if (procents>10)
		procents=10;
	skill[a_skill] += procents;
	if (procents==10) // next 10% achieved
	{
		experience_in_skill[a_skill]=0;

		if (a_skill==SKILL_UNARMED)
		{
			unarmed.DMG++;
			if ((skill[a_skill]/10)%2 == 0)
			{
				unarmed.HIT++;
				unarmed.DEF++;
			}
		}
	}
}

void CHero::skill_used(int a_skill)
{
	if (free_skill_pointes==0)
		return;

	int random_value = skill[a_skill]/10 * SKILL_MULTIPLIER +1;

	if (random_value>free_skill_pointes)
		random_value=free_skill_pointes;

	random_value = random(random_value);

	free_skill_pointes-=random_value;
	train_skill(a_skill,random_value);
}

int CHero::build_robot(CItem *item)
{
	CRobotShell *shell;

	shell = item->IsRobotShell();
	if (shell==NULL) 
		return false;

	string temp;

	// print robot parts
	int character;
	int _state = false;
	string str;

	show_robot_build_screen(shell,-1);

	while (1)
	{
		character=keyboard.getkey();
		if (character=='o' || character=='O')
		{
			// 0 if not robot
			// 1 if done
			// 2 if something done in inventory 
			// 3 if robot turned off
			// -1 if not enough skill to turn
			// -2 if robot has no CPU
			// -3 if now place around to set robot as monster

			if (shell->owner==NULL || (shell->owner!=NULL && !shell->owner->IsRobot()))
			{
				_state = turn_robot_on(shell);
			}
			else // turn off
			{
				CRobot *rob;
				rob = shell->owner->IsRobot();
				
				rob->tries_to_turn_off(this);
				return 3;
			}

			set_color(4);
			if (_state==1) // wlaczono, wiec przerywamy
			{
				for (int a=0;a<2;a++)
					skill_used(SKILL_MECHANIC);
				break;
			}
			else if (_state==-2)
			{           
				str = " Robot needs CPU to work! ";
				print_text(40-str.size()/2,0,str);
			}
			else if (_state==-3)
			{
				str = " There is no place to turn robot on. ";
				print_text(40-str.size()/2,0,str);
			}
			else if (_state==-1) // no skill
			{
				str = " You are not skilled enough to turn on this robot! ";
				print_text(40-str.size()/2,0,str);
				skill_used(SKILL_MECHANIC);				
			}
			keyboard.wait_for_key();
			show_robot_build_screen(shell,-1);							
		}
		else if (character=='r' || character=='R')
		{
			set_color(11);
			print_text(7,2,"                                                         ");
			shell->last_robot_name = keyboard.get_name(7,2,40);
			show_robot_build_screen(shell,-1);				
		}
		else if (character>='a' && character<='l')
		{
			if (shell->owner==NULL || (shell->owner!=NULL && !shell->owner->IsRobot()))
			{
				build_robot_change_field_number(shell,character-'a');
				show_robot_build_screen(shell,-1);				
			}
		}
		else if (character=='m' || character=='M')
		{
			show_robot_build_screen(shell,2);	
			set_color(14);
			print_character(1,4,'S');			
			if (build_robot_fix(shell))
			{
				sounds.PlaySoundOnce("data/sounds/other/autorepair.wav");
				return 2;
			}
			show_robot_build_screen(shell,-1);	
		}		
		else if (character=='p' || character=='P')
		{
			if (reprogram_robot(shell))
			{
				return 2;
			}
		}		
		else if (character==keyboard.escape)
			break;
	}
	return _state==1;
}

bool CHero::reprogram_robot(CRobotShell *shell)
{
	if (shell==NULL || shell->cpu==NULL)
		return false;

	CProgrammator *progr;

	// choose the programming tool if have one
	ptr_list available;
	ptr_list::iterator m,_m;
	build_robot_find_available_parts(available);
	for (m=available.begin(),_m=available.end();m!=_m;++m)
	{
		progr = ((CItem *) (*m))->IsProgrammator();
		if (progr!=NULL && reprogram(progr,shell->cpu))
		{
			sounds.PlaySoundOnce("data/sounds/other/autorepair.wav");
			return true;
		}
	}
	return false;
}

bool CHero::build_robot_fix(CRobotShell *shell)
{
	char character;
	int field_number, kind=0, index;
	ptr_list::iterator m;
	CItem *item=NULL;
	set_color(10);
	print_text(25,0," Choose the robot part to repair ");
	myrefresh();			 
	// choose what to fix
	while (1)
	{
		character=keyboard.wait_for_key();
		if (character>='a' && character<='l')
		{
			field_number = character-'a';
			if (field_number>=shell->max_number_action_slots + shell->max_number_move_slots + 1)
				continue;
			
			if (field_number!=0) // not CPU
			{
				// check kind of field to fix
				if ( field_number-1 < shell->max_number_action_slots)
				{
					kind = 1;
					if (field_number-1 < shell->action_slots.size())
						kind = -1;
				}
				else
				{
					if ( field_number-1 - shell->max_number_action_slots < shell->max_number_move_slots)
					{
						kind = 2;
						if (field_number-1 - shell->max_number_action_slots < shell->move_slots.size())
							kind = -2;
					}			
				}
				
			}
			
			// CPU choosen
			if (field_number==0)
			{
				if (shell->cpu!=NULL)
				{
					item=shell->cpu;
					break;			
				}
			}		
			else if (kind==-1) // action slot selected 
			{
				for (index=0,m=shell->action_slots.begin();index<field_number-1;index++)
					m++;
				
				// fix this item
				item = (CItem *) *m;
				break;
			}
			else if (kind==-2) // move slot selected
			{
				for (index=0,m=shell->move_slots.begin();index<field_number-1-shell->max_number_action_slots;index++)
					m++;
				
				// fix this item
				item = (CItem *) *m;
				break;
			}
			
		}
		else if (character=='s')
		{
			item=shell;
			break;
		}
		else if (character==keyboard.escape || character==keyboard.readmore || character==keyboard.readmore2)
		{
			return false;
		}
	} // endof while(1)
	

	if (item==NULL)
		return false;
	// choose the repair kit
	CItem *temp;
	show_backpack(NULL, INV_SHOW_HIGHLIGHT_REAPIR_KITS);
	set_color(10);
	print_text(25-item->show_name().size()/2,0,string(" Choose a tool to repair the ") + item->show_name() + " ");
	myrefresh();			 
	while (1)
	{
		character=keyboard.wait_for_key();
		if ((character>='a' && character<='z') || (character>='A' && character<='Z'))
		{
			temp=choose_item_from_backpack(character);
			if (temp!=NULL) // when selected
			{
				CRepairKit *repset;
				repset=temp->IsRepairSet();
				if (repset!=NULL && repset->can_be_used())
				{
					if (repset->fix(item,this))
						return true;
				}
			}
		}
		else if (character==keyboard.escape || character==keyboard.readmore || character==keyboard.readmore2)
		{
			return false;
		}
	} // endof while(1)

	return false;
}

void CHero::show_robot_build_screen(CRobotShell *shell, int show_letters)
{
	if (show_letters==-1)
	{
		if (shell->owner==NULL || (shell->owner!=NULL && !shell->owner->IsRobot()))
			show_letters=true;
	}

	myclear();
	string temp = " Robot Assembly ";
    set_color(2);
    print_text(0,0, "--------------------------------------------------------------------------------");
    print_text(0,49,"--------------------------------------------------------------------------------");
    set_color(10);
	print_text(40-temp.size()/2,0,temp);

	set_color(7);	
	print_text(1,2,"Name: ");

	set_color(15);
	print_text(7,2,shell->last_robot_name);
	
	set_color(7);
	print_text(1,4,"Shell: x");
	shell->draw_at(8,4);
	set_color(15);
	print_text(10,4,shell->name + " <" + IntToStr((100*shell->hit_points)/shell->max_hit_points) + "%>");

	set_color(7);
	temp = "Robot weight: ";
	temp+=IntToStr(shell->calculate_weight());
	print_text(1,6,temp);

	set_color(7);
	if (show_letters==1 || (show_letters==2 && shell->cpu!=NULL))
	{
		print_text(1,9,"CPU: a) ");
		set_color(14);
		print_text(6,9,"a");			
	}
	else
		print_text(1,9,"CPU:");
	
	
	if (shell->cpu!=NULL)
	{
		shell->cpu->draw_at(9,9);		
		set_color(15);	
		print_text(11,9,shell->cpu->show_name() + " <" + IntToStr((100*shell->cpu->hit_points)/shell->cpu->max_hit_points) + "%>");
	}
	else
	{
		temp="-------------------";
		set_color(7);	
		print_text(11,9,temp);
	}
	
	int y=11;
	char character='b';
	
	CItem *it;
	ptr_list::iterator m;
	int a;
	
	if (shell->max_number_action_slots>0)
	{
		set_color(7);
		print_text(1,y,"[ACTION SLOTS]");
		y+=2;
		
		m=shell->action_slots.begin();
		for (a=0;a<shell->max_number_action_slots;a++)
		{
			if (a>=shell->action_slots.size())
			{
				set_color(7);
				print_text(6,y,"     -------------------");
			}
			else
			{
				it = (CItem *) *m;
				it->draw_at(9,y);
				set_color(15);	
				print_text(11,y,it->show_name() + " <" + IntToStr(100*it->hit_points/it->max_hit_points) + "%>");
				m++;
			}

			if (show_letters==1 || (show_letters==2 && a<shell->action_slots.size()))
			{
				set_color(7);
				print_text(6,y," ) ");				
				set_color(14);
				print_character(6,y,character);
			}
			character++;
			y++;
		}
		y++;
	}

	if (shell->max_number_move_slots>0)
	{
		set_color(7);
		print_text(1,y,"[MOVE SLOTS]");
		y+=2;	
		
		m=shell->move_slots.begin();
		for (a=0;a<shell->max_number_move_slots;a++)
		{
			if (a>=shell->move_slots.size())
			{
				set_color(7);
				print_text(6,y,"     -------------------");
			}
			else
			{
				it = (CItem *) *m;
				it->draw_at(9,y);
				set_color(15);	
				print_text(11,y,it->show_name() + " <" + IntToStr(100*it->hit_points/it->max_hit_points)+ "%>");
				m++;
			}
			
			if (show_letters==1 || (show_letters==2 && a<shell->move_slots.size()))
			{
				set_color(7);
				print_text(6,y," ) ");				
				set_color(14);
				print_character(6,y,character);
			}				
			character++;
			y++;
		}		
	}	

	// robot printed
	// show options

	screen.draw_box(2,58,3,75,10);

	set_color(6);
	print_text(60,5,"Rename robot");
	set_color(14);
	print_character(60,5,'R');

	set_color(6);
	if (shell->owner==NULL || (shell->owner!=NULL && !shell->owner->IsRobot()))
		print_text(60,6,"Turn robot on");
	else
		print_text(60,6,"Turn robot off");

	set_color(14);
	print_character(71,6,'O');

	set_color(6);
	print_text(60,7,"Mend");
	set_color(14);
	print_character(60,7,'M');


	set_color(6);
	print_text(60,8,"Program CPU");

	if (shell->cpu!=NULL && shell->cpu->group_affiliation==GROUP_ENEMIES)
	{
		set_color(14);
		print_character(69,8,'P');
	}
}

int CHero::build_robot_change_field_number(CRobotShell *shell, int field_number)
{
	ptr_list available_items;
	ptr_list available_CPUs;
	ptr_list available_legs;
	ptr_list::iterator m,_m;
	CItem *temp;
	int index;

	// kind: (change to enums !!!)
	// 1 - free slot action
	// -1 - taken slot action
	// 2 - free slot move
	// -2 - taken slot move
	int kind=0;

	if (field_number>=shell->max_number_action_slots + shell->max_number_move_slots + 1)
		return false;

	if (field_number!=0) // nie CPU
	{
		// sprawdzamy jakiego rodzaju jest number pola w powloce
		if ( field_number-1 < shell->max_number_action_slots)
		{
			kind = 1;
			if (field_number-1 < shell->action_slots.size())
				kind = -1;
		}
		else
		{
			if ( field_number-1 - shell->max_number_action_slots < shell->max_number_move_slots)
			{
				kind = 2;
				if (field_number-1 - shell->max_number_action_slots < shell->move_slots.size())
					kind = -2;
			}			
		}

	}

	// when free, remove
	if (field_number==0)
	{
		if (shell->cpu!=NULL)
		{
			shell->uninstall_CPU();
			return true;			
		}
	}		
	else if (kind==-1) // wybrano zajety slot action. Usuwamy item z niego.
	{
		for (index=0,m=shell->action_slots.begin();index<field_number-1;index++)
			m++;
		shell->uninstall_from_action_slot((CItem *) *m);
		return true;
	}
	else if (kind==-2) 
	{
		for (index=0,m=shell->move_slots.begin();index<field_number-1-shell->max_number_action_slots;index++)
			m++;
		
		// deinstall this item
		shell->uninstall_from_move_slot((CItem *) *m);
		return true;
	}

	// when slot is empty
	build_robot_find_available_parts(available_items);
	
	// shell is not avail to install on itself
	available_items.remove(shell);	

	if (field_number==0) // CPU
	{
		for (m=available_items.begin(),_m=available_items.end();m!=_m;m++)
		{
			temp = (CItem *) *m;
			if (temp->IsRobotCPU())
				available_CPUs.push_back(temp);
		}

		temp = choose_item_from_list(available_CPUs, "Select a processor to install (items around)");
		shell->install_CPU(temp);
		sounds.PlaySoundOnce("data/sounds/other/place_robot_part.wav");
	}
	else if (kind==1) // any item
	{
		temp = choose_item_from_list(available_items, "Select an item to install into action slot (items around)");
		shell->install_in_action_slot(temp);
		sounds.PlaySoundOnce("data/sounds/other/place_robot_part.wav");
	}
	else if (kind==2)
	{
		for (m=available_items.begin(),_m=available_items.end();m!=_m;m++)
		{
			temp = (CItem *) *m;
			if (temp->IsRobotLeg())
				available_legs.push_back(temp);
		}				
		temp = choose_item_from_list(available_legs,"Select an item to install into move slot (items around)");
		shell->install_in_move_slot(temp);
		sounds.PlaySoundOnce("data/sounds/other/place_robot_part.wav");
	}
		
	return true;
}

bool CHero::build_robot_find_available_parts(ptr_list &available_items)
{
	ptr_list::iterator m,_m;

	available_items.clear();

	// add items from player's backpack

	for (m=backpack.begin(),_m=backpack.end();m!=_m;m++)
		available_items.push_back(*m);

	// add items from ground

	level.list_of_items_from_cell(&available_items,pX(),pY());

	if (level.entering_on_cell_possible(pX()-1,pY()))
		level.list_of_items_from_cell(&available_items,pX()-1,pY());

	if (level.entering_on_cell_possible(pX()+1,pY()))
		level.list_of_items_from_cell(&available_items,pX()+1,pY());

	if (level.entering_on_cell_possible(pX(),pY()-1))
		level.list_of_items_from_cell(&available_items,pX(),pY()-1);
	
	if (level.entering_on_cell_possible(pX(),pY()+1))
		level.list_of_items_from_cell(&available_items,pX(),pY()+1);


	if (level.entering_on_cell_possible(pX()-1,pY()-1))
		level.list_of_items_from_cell(&available_items,pX()-1,pY()-1);
	
	if (level.entering_on_cell_possible(pX()+1,pY()-1))
		level.list_of_items_from_cell(&available_items,pX()+1,pY()-1);
	
	if (level.entering_on_cell_possible(pX()-1,pY()+1))
		level.list_of_items_from_cell(&available_items,pX()-1,pY()+1);
	
	if (level.entering_on_cell_possible(pX()+1,pY()+1))
		level.list_of_items_from_cell(&available_items,pX()+1,pY()+1);
	
	
	return true;
}

CItem * CHero::choose_item_from_list(ptr_list &available_items, string header)
{
	struct SScreen_copy scr_copy;

	if (available_items.size()==0)
		return NULL;

    store_screen(&scr_copy);

	char *letters="abcdefghijklmnopqrstuvwxyz0123456789";
	
	CItem *returned=NULL;

	//
	int scroll=0;
	const int on_screen=36;
	int items_left = available_items.size();
	ptr_list::iterator m, first_m;
	CItem *item;

	int index;
	int key;

	header = string(" ") + header + " ";
	
	m = available_items.begin();
	while (items_left>0)
	{
		myclear();
		set_color(2);
		print_text(0,0, "--------------------------------------------------------------------------------");
		print_text(0,49,"--------------------------------------------------------------------------------");
		set_color(10);
		print_text(40-header.size()/2,0,header);
		
		first_m = m;

		for (index=0; index < min(items_left,on_screen); index++)
		{
			item = (CItem *) *m;
			set_color(14);
			print_character(1,index+4,letters[index]);
			set_color(7);
			print_character(2,index+4,')');

			set_color(7);			

			string to_show = item->show_name();

			if (item==this->weapon)
				to_show+=" (your weapon)";
			else if (item==this->armor)
				to_show+=" (your armor)";

			print_text(6,index+4,to_show);
			item->draw_at(4,index+4);
			m++;
		}
		if (items_left>on_screen)
		{
			set_color(7);						
			print_text(6,index+6,"[ MORE >> ]");
		}
		//
		while (1)
		{
			key = keyboard.getkey();
			m = first_m;
			for (int a=0;a<min(items_left,on_screen);a++)
			{
				if (key==letters[a])
				{
					returned = (CItem *) *m;
					restore_screen(&scr_copy);	
					return returned;
				}
				m++;
			}

			if (key == keyboard.escape || key == keyboard.readmore || key == keyboard.readmore2)
			{
				break;
			}
		}
		items_left-=index;
		scroll+=index;
	}
		
    restore_screen(&scr_copy);	
	return returned;
}

void CHero::add_known_item(string real)
{
	known_items.insert(real);
}

void CHero::add_known_category_of_items(string category_to_add)
{
	list_of_names_for_categories l;
	list_of_names_for_categories::iterator m,_m;	
	string n;
	
	// no such category
	if (definitions.category_number.find(category_to_add)==definitions.category_number.end())
	{
		fprintf(stderr,"\nERROR: Try to learn player about unknown category: %s\n\n",category_to_add.c_str());
		keyboard.wait_for_key();
		exit(32);			
	}

	int kat = definitions.category_number[category_to_add];

	if (definitions.objects_in_category.find(kat)!=definitions.objects_in_category.end())
	{
		l=definitions.objects_in_category[kat];
		for (m=l.begin(),_m=l.end();m!=_m;m++)
		{
			n=*m;
			add_known_item(n);
		}
	}	
}

bool CHero::is_item_known(string real)
{
	if (known_items.find(real)!=known_items.end())
		return true;
	return false;
}

void CHero::create_list_of_monsters_following_player()
{
	// player is followed by:
	// 1. friendly monsters in FOV of player
	// 2. enemies that see player (50%)
	
	ptr_list::iterator sm,_sm;
	CMonster *monster;
	CRobot *rob;
	CEnemy *host;
	for (sm = level.monsters.begin(),_sm = level.monsters.end();sm!=_sm;sm++)
	{
		monster = (CMonster *) *sm;
		if (monster->IsMadDoctor()) // MadDoctor doesn't go
			continue;
		if (monster->IsWorm()) // Worms don't go
			continue;

		host = monster->IsHostile();
		if (host==NULL)
			continue;
		rob = monster->IsRobot();
		
		if (host!=this && !host->IsStunned() && (host->speed.GetValue()>5 || (rob!=NULL && rob->get_robot_speed()>5)))
		{
				if (monster->seen_now)
				{
					if (host->is_friendly(level.player) ||
							(host->enemy==this && !host->have_critically_low_hp() &&
							   (coin_toss() ||
								  ( abs(host->pX()-pX())<2 && abs(host->pY()-pY())<2 )
							   )
							)
					   )
					{
						if (host->set_direction_to_cell_by_shortest_path(this->pX(),this->pY(),false))						
						{
							int time;
							if (rob!=NULL)
								time = TIME_MOVE/(rob->get_robot_speed()+1)+1;
							else
								time=TIME_MOVE/(speed.GetValue()+1)+1;
							host->wait( distance(pX(),pY(),host->pX(),host->pY()) * time );
							monsters_following_player_to_other_level.push_back(host);						
						}
					}
				}				
		}
	}	
}

int CHero::reprogram(CProgrammator *item, CProcessor *cpu)
{
	if (item==NULL)
		return false;

	if (cpu==NULL)
	{
		ptr_list available_items;
		ptr_list available_CPUs;
		ptr_list::iterator m,_m;
		build_robot_find_available_parts(available_items);	
		CItem *temp;

		for (m=available_items.begin(),_m=available_items.end();m!=_m;m++)
		{
			temp = (CItem *) *m;
			CProcessor *cpu = temp->IsRobotCPU();
			if (cpu!=NULL && cpu->group_affiliation==GROUP_ENEMIES)
				available_CPUs.push_back(temp);
		}
		temp = choose_item_from_list(available_CPUs,"Select a CPU to program (items around)");
		cpu = temp->IsRobotCPU();
	}
	
	if (cpu==NULL)
		return false;

	if (cpu->group_affiliation == GROUP_HERO)
	{
		screen.console.add(item->show_name() + " is already friendly.\n",3);		
		return false;
	}
	
	if (cpu->quality > skill[SKILL_PROGRAMMING])
	{
		string str = " You are not skilled enough to program this CPU! ";
		print_text(40-str.size()/2,0,str);
		myrefresh();
		keyboard.wait_for_key();
		level.player->skill_used(SKILL_PROGRAMMING);		
		return false;
	}

	cpu->group_affiliation = GROUP_HERO;
	screen.console.add(name + " has programmed the " + item->show_name() + " to be friendly.\n",3,false);

	for (int a=0;a<5;a++) // 5 times
		level.player->skill_used(SKILL_PROGRAMMING);		
	return true;
}

bool CHero::set_direction_to_closest_unexplored()
{
	// find the closest unexplored by floodfill.
	if (travel_dest_x!=-1 && travel_dest_y!=-1 && !(pX()==travel_dest_x && pY()==travel_dest_y))
	{
		if (!level.map.seen(travel_dest_x,travel_dest_y) ||
			(
			level.map.seen(travel_dest_x,travel_dest_y) &&  (!level.map.blockMove(travel_dest_x,travel_dest_y) || (level.map.blockMove(travel_dest_x,travel_dest_y)) 
			&& level.map.isOpenable(travel_dest_x,travel_dest_y))
			)
			)
		{
			if (set_direction_to_cell_by_shortest_path(travel_dest_x,travel_dest_y,true))
				return true;
		}
	}

	CPosition pos,new_pos;
	list<CPosition> positions;
	list<CPosition>::iterator m;
	int tested[MAPWIDTH][MAPHEIGHT];
	int x,y;
	for (y=0;y<MAPHEIGHT;++y)
		for (x=0;x<MAPWIDTH;++x)
			tested[x][y]=0;

	pos.x=pX();
	pos.y=pY();
	positions.push_back(pos);
	tested[pos.x][pos.y]=1;

	m=positions.begin();
	while(m!=positions.end())
	{
		pos=*m;
		if (!level.map.seen_once(pos.x,pos.y))
		{
			for (int a=-1;a<=1;a++)
				for (int b=-1;b<=1;b++)
				{
					new_pos.x=pos.x+a;
					new_pos.y=pos.y+b;
					if (level.map.seen_once(new_pos.x,new_pos.y) && (!level.map.blockMove(new_pos.x,new_pos.y) || level.map.isOpenable(new_pos.x,new_pos.y)))
					{
						if (set_direction_to_cell_by_shortest_path(new_pos.x,new_pos.y,true))
						{
							travel_dest_x=new_pos.x;
							travel_dest_y=new_pos.y;
							return true;
						}
					}
				}
		}
		
		for (int a=-1;a<=1;a++)
			for (int b=-1;b<=1;b++)
			{
				new_pos.x=x=pos.x+a;
				new_pos.y=y=pos.y+b;
				
				if (level.map.onMap(x,y) && !tested[x][y])
				{
					positions.push_back(new_pos);
					tested[x][y]=1;

				}
			}
		++m;
	}
	stop_repeating();
	return false;
}

bool CHero::choose_travel_destination()
{
	int key;
	int x,y;
	int last_x,last_y;
	struct SScreen_copy scr_copy;
	int changex, changey;

	last_x=-1; last_y=-1;

	screen.console.zero();
	screen.console.clean();
	screen.console.show();

	store_screen(&scr_copy); // remember screen

	changex=0;
	changey=0;
	key=0;
	x=pX();
	y=pY();

	while(1)
	{
		x+=changex;
		y+=changey;
		if (key==keyboard.escape)
		{
			restore_screen(&scr_copy);
			screen.console.clean();
			screen.console.zero();
			screen.console.show();
			return false;
		}
		else if (key==keyboard.readmore || key==keyboard.readmore2 || key==10 || key==459) // || enter
		{
			if (level.map.seen_once(x,y))
			{
				travel_dest_x=x;
				travel_dest_y=y;
				restore_screen(&scr_copy);
				screen.console.clean();
				screen.console.zero();
				screen.console.show();
				return true;
			}
		}
		else if (key==keyboard.down || key==keyboard.up) // stairs
		{
				// find stairs
			ptr_list::iterator m,_m;
			
			// find current stairs
			bool stairs_found=false;
			for (m=level.map.stairs_of_map.begin(),_m=level.map.stairs_of_map.end();m!=_m;++m)
			{
				STAIRS *stairs = (STAIRS *) (*m);
				if (stairs->x==x && stairs->y==y)
				{
					++m;
					if (m==_m)
						break;

					stairs = (STAIRS *) (*m);
					if (level.map.seen_once(stairs->x,stairs->y)) // next stairs
					{
						x=stairs->x;
						y=stairs->y;
						stairs_found=true;
						break;
					}
				}
			}
			if (!stairs_found) // find first seen
			{
				for (m=level.map.stairs_of_map.begin(),_m=level.map.stairs_of_map.end();m!=_m;++m)
				{
					STAIRS *stairs = (STAIRS *) (*m);
					if (level.map.seen_once(stairs->x,stairs->y))
					{
						x=stairs->x;
						y=stairs->y;
						break;
					}
				}
			}
			
		}

		restore_screen(&scr_copy);

		// limit of movement
		if (x<0 || x>79 || y<0 || y>39)
		{
			x=last_x; y=last_y;
		}
		set_color(12);
		print_character(x,y,'@');
		screen.console.add_and_zero("Choose destination or use '<' and '>' to locate stairs.",7);
		screen.console.show();
		myrefresh();

		key = keyboard.get_direction(changex, changey);

	} // endof while
}