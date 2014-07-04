#ifndef MONSTER_H
#define MONSTER_H

//#pragma warning (disable: 4786)
//#pragma warning (disable: 4788)
// disable warning (compare bool with int)
//#pragma warning (disable: 4805)
//#pragma warning (disable: 4503)

#include <list>
#include <set>
using namespace std;

#include "tiles.h"
#include "actions.h"
#include "items.h"
#include "game.h"
#include "attrib.h"

typedef list<void *> ptr_list;
typedef set <string> set_of_known_names;

// w global.cpp
void bad_virtual_call(string method);

// value dla samoleczenia - po osiagnieciu tej wartosci HP wzrasta o 1
#define REACHED_TREATMENT 1000

#define GROUP_HERO		0
#define GROUP_ENEMIES		1
#define GROUP_NEUTRAL		2
#define GROUP_CRAZY	3

class CAnimal;
class CIntelligent;
class CRobot;
class CHero;
class CEnemy;
class CSearchLight;
class CMadDoctor;
class CWorm;

#define SPECIAL_PROPERTY_NONE 0
#define SPECIAL_PROPERTY_SPLITTER 1

#define MUTATION_POSITIVE 1
#define MUTATION_NEGATIVE 0
#define MUTATION_NEUTRAL 2

class CMonster : public CTile {
	
protected:
  void cell_in_current_direction(int &x, int &y);
  void direction_to_near_cell(int x, int y); // tylko pole sasiadujace z potworem

  int self_treatment; // incrases every turn by metabolism value, when 1000, HP++
  virtual void self_treatment_every_turn();
  virtual void status_management_every_turn();

public:	
  virtual bool ChangePosition(int x, int y);

  bool is_dead; // is dead
  int weight;  

  int last_pX, last_pY; // position in last turn
  
  int category;
  int special_properties;

  CAttribute fov_radius;
  
  int size;

  CAttribute hit_points;
  CAttribute energy_points;
  CAttribute strength;
  CAttribute dexterity;
  CAttribute endurance;
  CAttribute intelligence;
  CAttribute speed;
  CAttribute metabolism;  

  string sound_dying;
  string sound_pain;

  int group_affiliation; // 0 - player   1 - enemies   2 - neutral  3 - crazy - attack everything (SENTRY GUN)
  int direction; // direction of movement

  WEAPON_UNARMED unarmed;
  NO_ARMOR no_armor;

  CItem *weapon;
  CBaseArmor *armor;

  t_time next_action_time; // w ktore turze nastapi ruch tego potwora
  bool seen_now; 
  t_time seen_last_time_in_turn;

  unsigned long experience_for_kill;  

  CMonster * enemy; // selected enemy 

  ptr_list seen_enemies;
  ptr_list seen_friends;

  int skill[NUMBER_OF_SKILLS];   // value for skills
  int resist[NUMBER_OF_RESISTS]; // value for resists
  int state[NUMBER_OF_STATES];   // value for states

  bool IsBlinded() { return state[STATE_BLIND]>0; };
  bool IsStunned()  { return state[STATE_STUN]>0; };
  bool IsConfused(){ return state[STATE_CONFUSE]>0; };
  bool IsBurning() { return state[STATE_TEMPERATURE]>0; };
  bool IsFreezed() { return state[STATE_TEMPERATURE]<0; };
  bool IsRadiated()    { return state[STATE_RADIOACTIVE]>0; };
  bool IsSlowed()  { return state[STATE_SPEED]<0; };
  bool IsHasted()  { return state[STATE_SPEED]>0; };
  bool IsPoisoned(){ return state[STATE_CHEM_POISON]>0; };

  CAnimal * IsAnimal();
  CIntelligent * IsIntelligent();
  CRobot *IsRobot();
  CHero * IsPlayer();
  CEnemy * IsHostile();
  CSearchLight * IsSearchlight();
  CMadDoctor * IsMadDoctor();
  CWorm * IsWorm();
  
  virtual void ShowDescription();

  CMonster();
  ~CMonster();

  virtual void set_attributes_on_self();
  virtual int move();
  void wait( t_time );
  virtual void attack(CMonster *a_enemy);
  bool is_unarmed(); // gdy ma jakas weapon
  bool in_armor(); // gdy nosi jakis armor
  
  virtual bool have_low_hp() { return (hit_points.GetValue() <= hit_points.max/2); };
  virtual bool have_critically_low_hp() { return (hit_points.GetValue() <= hit_points.max/4); };
  
  virtual bool is_intelligent() { return false; };
  bool is_enemy(CMonster *m);
  bool is_friendly(CMonster *m);
  virtual int cause_damage(int damage); // return (0 - no damage, 1 - damage done or -1 death)
  virtual int use_energy(int energy);
  virtual int add_energy(int energy);
  bool heal(int damage);
  virtual int hit_by_item(int hit_power, CItem *item);
  virtual int hit_by_explosion(int hit_power);
  virtual int hit_changes_status(int kind, int power);
  virtual bool attack_in_direction();
  virtual bool look_around();
  virtual int calculate_weight() { return weight; };
  virtual int mutate(int type);
  
  virtual void monster_sees_enemy(CMonster *enemy) {};
  virtual bool evade_missile(CMonster *thrower);
  virtual int get_h2h_attack_value();
  virtual int get_h2h_defense_value();
  
  virtual void enemy_died();
  virtual bool death();
  virtual EAction get_action() { bad_virtual_call("Monster: get_action"); return ACTION_NOTHING; };
  virtual t_time do_action( EAction ) { bad_virtual_call("Monster: do_action");  return 0; };

  virtual void select_closest_enemy() { bad_virtual_call("Monster: select_closest_enemy");};

  virtual CMonster * duplicate()  { bad_virtual_call("Monster: duplicate"); return NULL; };

  virtual void every_turn();
  
   /// INHERITED FROM ToSave
	virtual unsigned long SaveObject();
	virtual bool LoadObject();
};

enum BEHAVIOUR {
	BEHAVIOUR_CALM,
	BEHAVIOUR_H2H_COMBAT,
	BEHAVIOUR_RANGED_COMBAT,
	BEHAVIOUR_RUN_AWAY,
	BEHAVIOUR_SEARCH_FOR_ENEMY,
	BEHAVIOUR_TURN_TO_FIGHT
};

class CEnemy : public CMonster {
public:
  EAction chosen_action;	// do decyzji inteligentnego...
  int actual_behaviour;
  bool turned_to_fight;
  
  int camping; // Licznik postoju w jednym miejscu dla potwora. Gdy > 5 to idzie w losowa strone.
  int last_x_of_enemy; // Ostatnie place, gdzie player byl widziany
  int last_y_of_enemy; // Ostatnie place, gdzie player byl widziany
  int last_direction_of_enemy;
  bool is_at_enemy_position;
  t_time enemy_last_seen_in_turn; 

  CEnemy();
  virtual void enemy_died();
  int set_direction_to_cell_by_shortest_path(int cell_x, int cell_y, bool opening);
  virtual int how_danger_is_cell(int x,int y) { return 0;}; // aby potrafil omijac (w direction na pole) takie pola  
  virtual void monster_sees_enemy(CMonster *enemy);
  virtual bool go_around_cell(int x,int y,bool opening);

   /// INHERITED FROM ToSave
	virtual unsigned long SaveObject();
	virtual bool LoadObject();
};

#define AI_NOT_LIMITED					0x00000000
#define AI_DONT_USE_GRENADES			0x00000001
#define AI_DONT_USE_HEALING				0x00000002
#define AI_DONT_USE_BOOSTERS			0x00000004
#define AI_DONT_USE_RANGED_WEAPONS		0x00000008
#define AI_DONT_MOVE					0x00000010
#define AI_DONT_COOPERATE_WITH_FRIENDS	0x00000020
#define AI_DONT_RUN_AWAY				0x00000040
#define AI_DONT_PICK_UP_ITEMS			0x00000080
#define AI_DONT_USE_OTHER_WEAPONS		0x00000100
#define AI_DONT_USE_OTHER_ARMORS		0x00000200

typedef unsigned long AI_CONTROL;

void calculate_hit_place(int who_x, int who_y, int &target_x, int &target_y, double angle_miss);

class CIntelligent : public CEnemy {
private:
	int misses_in_shooting; // to choose lower distance
	int run_away_to_x, run_away_to_y;
public:
  AI_CONTROL AI_restrictions;
  t_time turns_of_searching_for_enemy;
  t_time turns_of_calm;
  
  ptr_list backpack;

  ptr_list seen_items;

  CItem *ready_weapon; // ready item  
  
  int max_items_in_backpack;
  int items_in_backpack;

  int get_backpack_weight(); // zwraca wage
  int get_monster_capacity();

  virtual bool look_around();
  virtual bool defend_self_from_danger_on_map();
  virtual bool is_intelligent() { return true; };
  virtual int how_danger_is_cell(int x,int y); // aby potrafil omijac takie pola
  virtual int cause_damage(int damage);
  virtual int use_item(CItem *); // np. pill
  virtual int set_weapon(CItem *);
  virtual int set_armor(CItem *);
  virtual int remove_armor();
  virtual int take_out_from_backpack(CItem *);
  virtual int drop_item(CItem *, bool see_possible);
  virtual int pick_up_item(CItem *, bool see_possible);
  virtual int shoot_into(int x, int y); // strzela z broni uzywanej w pole
  virtual int reload_weapon();
  virtual int can_pick_up(CItem *item);
  virtual bool open_in_direction();
  virtual bool close_in_direction();
  virtual bool isStrained();
  virtual bool isBurdened();
  virtual int calculate_weight() { return weight + get_backpack_weight(); };
  
  // with shooting  
  virtual bool evade_missile(CMonster *thrower);
  virtual bool throw_item_to (int x,int y,CItem *item);
  // AI
  virtual bool throw_item_to();
  virtual bool load_weapon();
  virtual bool unload_weapon();
  virtual bool fire_at_enemy();
  virtual bool use_healing_item();
  virtual bool use_remove_poison_item();
  virtual bool throw_active_grenade();
  virtual bool tell_others_about_enemy();
  virtual bool use_booster_item();
  virtual bool change_weapon_to_better_one();
  virtual bool change_armor_to_better_one();
  virtual bool activate_grenade_when_enemy_is_far();
  virtual int activate_weapon_and_armor_when_energy_available();
  virtual int carried_number_of_missiles_for_weapon(CRangedWeapon *ranged_weapon);
  virtual bool choose_place_to_throw_grenade(int &x,int &y);
  virtual int go_to_direction_of_enemy();
  virtual bool find_best_weapons(CItem *&best_for_h2h, int &best_evaluation_h2h, CItem *&best_for_ranged, int &best_evaluation_ranged);
  virtual int pick_valuable_items();
  virtual CItem * go_to_valuable_item();
  virtual bool choose_run_away_direction();
  virtual bool use_defensive_techniques_running_away();
  virtual int  turn_robot_on(CItem *to_turn_on);

  virtual bool choose_ready_weapon(CItem *item);
  virtual bool set_ready_weapon();

  virtual void start_to_run_away();
				
  CIntelligent();
  virtual bool death();
  
  virtual EAction get_action();
  virtual t_time do_action( EAction );
  
  virtual CMonster * duplicate();

   /// INHERITED FROM ToSave
	virtual unsigned long SaveObject();
	virtual bool LoadObject();
};

#define SKILL_MULTIPLIER 35

// way of showing the inventory

enum EInvShowType {
	INV_SHOW_SIMPLE,
	INV_SHOW_MATCH_AMMO,
	INV_SHOW_HIGHLIGHT_CORPSES,
	INV_SHOW_MATCH_ITEM_TO_REPAIR,
	INV_SHOW_REPAIR_KIT,
	INV_SHOW_HIGHLIGHT_REAPIR_KITS,
	INV_SHOW_IMPROVE_ARMOR
};

enum ERepeatAction {
	REPEAT_ACTION_NONE,
	REPEAT_ACTION_REST,
	REPEAT_ACTION_INVENTORY,
	REPEAT_ACTION_TRAVEL,
	REPEAT_ACTION_EXPLORE
};

class CHero : public CIntelligent {
private:
  CItem *choose_item_to_pick_up();

  int turns_of_rest;
  int exp_level;

  unsigned long experience;
  unsigned long level_at_experience;
  unsigned long free_skill_pointes;
  
  int experience_in_skill[NUMBER_OF_SKILLS];   // experience w tym % w skillu
  void train_skill(int skill, int value);

  bool set_direction_to_closest_unexplored(); // returns distance
  void got_level_up();  
  int show_actions_of_item(CItem *);
  int get_action_for_item(CItem *);
  int shoot_into(int x, int y); // strzela z broni uzywanej w pole
  CMonster * choose_target(int *x, int *y, string text);
  virtual void select_closest_enemy();
  bool choose_travel_destination();
  bool print_visible_monsters_and_items();
  void show_attributes();
  int show_inventory();
  bool use_stairs(bool direction);
  bool reprogram_robot(CRobotShell *shell);
  
  void show_player_info();
  void show_info();
  
  void look_with_cursor();
  bool set_move_direction(void); // ustala zmienna direction gracza na chosen_one

  ERepeatAction action_to_repeat;
  bool inventory_repeat_break;
  
public:
  ptr_list monsters_following_player_to_other_level;

  set_of_known_names known_items;
  void add_known_item(string real);
  void add_known_category_of_items(string category_to_add);
  bool is_item_known(string real);

  string ID_of_level; // on which player is
  string ID_of_last_level; // from which player came
  int stairs_number_used; // with changing level
  unsigned long adaptation_points;

  int travel_dest_x, travel_dest_y; // not saved
  
  CItem *last_activated;
  
  CHero();
  EAction get_action();
  t_time do_action( EAction );

  void create_list_of_monsters_following_player();

  virtual int pick_up_item(CItem *, bool see_possible);

  void show_laying_items();
  void show_backpack(CItem *passed_item, EInvShowType print_type); 
  void show_robot_build_screen(CRobotShell *shell, int show_letters); // if -1 then check is robot on;
  int build_robot_change_field_number(CRobotShell *shell, int field_number);
  bool build_robot_fix(CRobotShell *shell);
  bool build_robot_find_available_parts(ptr_list &available_items);
  
  CItem *choose_item_from_backpack(int character);
  CItem *choose_item_from_list(ptr_list &available_items, string header);
  void give_experience(unsigned long value);
  void skill_used(int skill);
  void stop_repeating();
  void repeat_action(ERepeatAction to_repeat);
  int is_repeating_action() { return action_to_repeat; };
  virtual bool look_around();  
  virtual int build_robot(CItem *item);
  virtual int reprogram (CProgrammator *item, CProcessor *cpu=NULL);
  virtual int set_weapon(CItem *);  
  virtual int drop_item(CItem *, bool visible_dropping);  
  virtual bool throw_item_to (int x,int y,CItem *item); 
  
  virtual void display(); // nowa method
  virtual bool death();

   /// INHERITED FROM ToSave
	virtual unsigned long SaveObject();
	virtual bool LoadObject();
};

class CAnimal : public CEnemy {
public:
  CAnimal();
  EAction get_action();
  t_time do_action( EAction );
  virtual bool look_around();
  virtual bool evade_missile(CMonster *thrower);
  virtual CMonster * duplicate();

  virtual int how_danger_is_cell(int x,int y); // aby potrafil omijac takie pola
  virtual int cause_damage(int damage);
  
   /// INHERITED FROM ToSave
	virtual unsigned long SaveObject();
	virtual bool LoadObject();
};

class CWorm : public CEnemy {
private:
	void reverse_worm();
public:
	CWorm *prev; // segments
	CWorm *next;
	int length_left;
	string segment_name;
	string tail_name;

	CWorm();
	EAction get_action();
	t_time do_action( EAction );
	virtual int move();
	virtual bool look_around();
	virtual CMonster * duplicate();

	/// INHERITED FROM ToSave
	virtual unsigned long SaveObject();
	virtual bool LoadObject();
};

class CRobot : public CEnemy {
private:
	int last_x_of_creator; 
	int last_y_of_creator; 
	bool sees_creator; 

	char get_CPU_instruction();
	bool can_weapon_shoot();
	bool load_ammo_to_weapon();
	bool fire_grenades();
					
public:
	CRobotShell *shell;

	unsigned long Creator_ID; 	

	CRobot();
	EAction get_action();
	t_time do_action( EAction );	

	virtual bool death();	
	void turn_robot_off();
	bool tries_to_turn_off(CMonster *temp);

	int get_robot_speed();
	virtual int calculate_weight() { return shell->calculate_weight(); };	
	
	virtual void display(); // robot to specjalny type potworka, zmienia wyglad zaleznie od pancerza
	virtual void ShowDescription();
	
	virtual bool look_around();
	virtual bool evade_missile(CMonster *thrower) { return false; };
	virtual CMonster * duplicate();

	virtual int get_h2h_attack_value();
	virtual int get_h2h_defense_value();
	
	virtual void attack(CMonster *enemy);	
	virtual bool attack_with_ranged_weapon();
	virtual bool shoot_into(int x, int y);

	virtual int cause_damage(int damage); // zwraca (0 - bez obrazen, 1 - byly lub -1 gdy death)
	bool heal(int damage);

	virtual int hit_by_item(int hit_power, CItem *item);
	virtual int hit_by_explosion(int hit_power);
	virtual int hit_changes_status(int kind, int power);
	virtual bool attack_in_direction();

	virtual bool have_low_hp() { return (shell->hit_points <= shell->max_hit_points/2); };
	virtual bool have_critically_low_hp() { return (shell->hit_points <= shell->max_hit_points/4); };	
	
	virtual int how_danger_is_cell(int x,int y) { return 0; }; // aby potrafil omijac takie pola

	virtual void self_treatment_every_turn();
	virtual void status_management_every_turn();	
	
	/// INHERITED FROM ToSave
	virtual unsigned long SaveObject();
	virtual bool LoadObject();
};


class CSearchLight : public CEnemy {
public:
	int destination_x,destination_y;
	CSearchLight();
	virtual bool death();	
	
	EAction get_action();
	t_time do_action( EAction );
	virtual bool look_around();
	virtual bool evade_missile(CMonster *thrower) { return false; };
	virtual CMonster * duplicate();
		
	/// INHERITED FROM ToSave
	virtual unsigned long SaveObject();
	virtual bool LoadObject();
};


#define MADDOCTOR_TEXTS 25

class CMadDoctor : public CIntelligent {
private:
	string histext[MADDOCTOR_TEXTS];
	int yelling_chance;
	
public:
	CHandWeapon tail;
	
	CMadDoctor();
	EAction get_action();
	t_time do_action( EAction );
	virtual bool look_around();
	virtual CMonster * duplicate();
	
	virtual int how_danger_is_cell(int x,int y); // aby potrafil omijac takie pola
	virtual bool death();
	
	// special akcje
	int throw_enemy_at_the_wall();
	int spit_poison();
	int fire_needle();
	int yell();

	/// INHERITED FROM ToSave
	virtual unsigned long SaveObject();
	virtual bool LoadObject();
};



#endif


