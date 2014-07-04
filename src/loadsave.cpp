#include "mem_check.h"

#define LOAD_DEBUG

#include "loadsave.h"
#include "items.h"
#include "monster.h"
#include "attrib.h"
#include "level.h"
#include "game.h"
#include "options.h"
#include "parser.h"
#include "keyboard.h"
#include <assert.h>

extern CGame game;
extern COptions options;
extern CKeyboard keyboard;
extern DEFINITIONS definitions;
extern CMyScreen screen;

#pragma warning (disable: 4786)
#pragma warning (disable: 4788)
#pragma warning (disable: 4805)

#include <string>
#include <list>
#include <map>
using namespace std;

#include <stdlib.h>
#include "monster.h"
#include "items.h"
#include "global.h"
#include "game.h"
#include <math.h>

extern CLevel level;

// static

FILE *CToSave::FilePointer; 
FILE *CToSave::loadlog; 
unsigned long CToSave::LastUniqueNumber = FIRST_UNIQUE_NUMBER;
to_fix CToSave::ListToFix;
addresses CToSave::AddressesOfObjects;
list_of_saved CToSave::saved;

CToSave *CToSave::CreateObject()
{
#ifdef LOAD_DEBUG
	fprintf(loadlog,"* TOSAVE::CreateObject\n");
#endif

	string s;
	unsigned long UN;
	CToSave *new_one;

	UN = 0;

	unsigned long FilePos;

	FilePos = ftell(FilePointer);

	LoadString(s);
	LoadLong(UN);

	// place w pliku jeszcze raz na poczatek, aby load CLASS_NAME w LoadObject bylo mozliwe.
	fseek(FilePointer,FilePos,SEEK_SET);   
	
	new_one = NULL;

   if (s == "CORPSE")
   {
                new_one=new CCorpse;
				level.all_items.push_back(new_one);
   }
   else if (s == "AMMO")
   {
                new_one=new CAmmo;
				level.all_items.push_back(new_one);
   }
   else if (s == "TRASH")
   {
				new_one=new CTrash;
				level.all_items.push_back(new_one);
   }
   else if (s == "REPAIR_KIT")
   {
	   new_one=new CRepairKit;
	   level.all_items.push_back(new_one);
   }
   else if (s == "RANGED_WEAPON")
   {
       new_one=new CRangedWeapon;
	   level.all_items.push_back(new_one);
   }
   else if (s == "HAND_WEAPON")
   {
       new_one = new CHandWeapon;
	   level.all_items.push_back(new_one);
   }
   else if (s == "GRENADE")
   {
       new_one=new CGrenade;
	   level.all_items.push_back(new_one);
   }
   else if (s == "CONTROLLER")
   {
       new_one=new CONTROLLER;
	   level.all_items.push_back(new_one);
   }
   else if (s == "PILL")
   {
	   new_one=new CPill;
	   level.all_items.push_back(new_one);
   }
   else if (s == "PROCESSOR")
   {
	   new_one=new CProcessor;
	   level.all_items.push_back(new_one);
   }
   else if (s == "ROBOT_LEG")
   {
	   new_one=new CRobotLeg;
	   level.all_items.push_back(new_one);
   }
   else if (s == "ROBOT_SHELL")
   {
	   new_one=new CRobotShell;
	   level.all_items.push_back(new_one);
   }
   else if (s == "NO_ARMOR")
   {
       new_one=new NO_ARMOR;
	   level.all_items.push_back(new_one);
   }
   else if (s == "ARMOR")
   {
       new_one=new CArmor;
	   level.all_items.push_back(new_one);
   }
   else if (s == "PROGRAMMATOR")
   {
	   new_one=new CProgrammator;
	   level.all_items.push_back(new_one);
   }
   else if (s == "BATTERY")
   {
	   new_one=new CBattery;
	   level.all_items.push_back(new_one);
   }   
   else if (s == "ANIMAL")
   {
       new_one=new CAnimal;
   }
   else if (s == "WORM")
   {
	   new_one=new CWorm;
   }
   else if (s == "MADDOCTOR")
   {
	   new_one=new CMadDoctor;
   }
   else if (s == "SEARCHLIGHT")
   {
	   new_one=new CSearchLight;
   }
   else if (s == "INTELLIGENT")
   {
       new_one=new CIntelligent;
   }
   else if (s == "ROBOT")
   {
	   new_one=new CRobot;
   }
   else if (s == "GAS")
   {
       new_one=new GAS;
   }
   else if (s == "FIRE")
   {
       new_one=new FIRE;
   }
   else if (s == "STAIRS")
   {
       new_one=new STAIRS;
   }
   else if (s == "NULL_OBJECT")
   {
	   new_one=NULL;
	   LoadString(s); // there won't be load object, so move file ptr by string
	   return new_one;
   }
   else
   {
	   fprintf(stderr,"ERROR READ: Not defined object for reading: %s\n",s.c_str());
	   keyboard.wait_for_key(); 
	   exit(199);	   
   }

   if (new_one==NULL)
   {
	   fprintf(stderr,"ERROR READ: Creating object failed %s\n",s.c_str());
	   keyboard.wait_for_key(); 
	   exit(200);
   }

   return new_one;
}

bool CToSave::LoadMessageBuffer(string file_name)
{
#ifdef LOAD_DEBUG
	fprintf(loadlog,"TOSAVE::LoadMessageBuffer\n");
#endif

	FILE *fp, *old_fp;
	
	old_fp = FilePointer;
	
	fp = fopen(string(string("save/") + game.name_of_player + ".msg").c_str(),"rb");
	
	if (fp==NULL)
	{
		fprintf(stderr,"ERROR READ: Can not open file save/%s.msg",level.player->name.c_str());		
		keyboard.wait_for_key(); 
		exit(83);
	}
	InitFilePointer(fp);
	
	int linii;
	string line;

	screen.console.message_buffer.clear();
	
	LoadInt(linii);
	for (int a=0;a<linii;a++)
	{
		LoadString(line);
		screen.console.message_buffer.push_back(line);
	}
	
	fclose(fp);
	InitFilePointer(old_fp);	
	return true;
}	

bool CToSave::SaveMessageBuffer(string file_name)
{
#ifdef LOAD_DEBUG
	fprintf(loadlog,"TOSAVE::LoadMessageBuffer\n");
#endif
	list<string>::iterator m,_m;
	m=screen.console.message_buffer.begin();
	_m=screen.console.message_buffer.end();

	FILE *fp, *old_fp;
	
	old_fp = FilePointer;

	fp = fopen(string(string("save/") + game.name_of_player + ".msg").c_str(),"wb+");
	if (fp==NULL)
	{
		fprintf(stderr,"ERROR SAVE: Error creating file save/%s.msg",level.player->name.c_str());		
		keyboard.wait_for_key(); 
		exit(83);
	}
	InitFilePointer(fp);
	
	SaveInt(screen.console.message_buffer.size());
	for (;m!=_m;m++)
	{
		string line = *m;
		SaveString(line);
	}

	fclose(fp);
	InitFilePointer(old_fp);	
	return true;
}	


bool CToSave::LoadOptions(string file_name)
{
#ifdef LOAD_DEBUG
	fprintf(loadlog,"TOSAVE::LoadOptions\n");
#endif
	
	string line;
	CFILE file;
	if (!(file.open(file_name)))
		return false;

	line=file.get_line();
	while (line!="")
	{
		bool wybrane = false;
		int option=-1;
		
		if (get_boolean(line,file.line_number)==true)
			wybrane = true;
		if (line.find("Constant letters in inventory:")!=-1)
			option = OPTION_LETTERS;
		else if (line.find("Show experience needed for new level:")!=-1)
			option = OPTION_EXPERIENCE;
		else if (line.find("Auto aim nearest enemy:")!=-1)
			option = OPTION_AUTO_AIM;
		else if (line.find("Auto aim if no enemy selected:")!=-1)
			option = OPTION_AIM_IF_NO_ENEMY;
		else if (line.find("Don't let friendly monsters to swap place with you:")!=-1)
			option = OPTION_DONT_LET_SWAP;
		else if (line.find("Show cursor on player:")!=-1)
			option = OPTION_CURSOR;
		else if (line.find("Always get the whole package of items:")!=-1)
			option = OPTION_GETALL;
		else if (line.find("")!=-1)
			option = OPTION_DONT_LET_SWAP;
		
		if (option!=-1)
			options.number[option] = wybrane;	

		line=file.get_line();
	}

	file.close();
	return true;
}	

bool CToSave::SaveOptions(string file_name)
{
#ifdef LOAD_DEBUG
	fprintf(loadlog,"TOSAVE::SaveOptions\n");
#endif

	CFILE file;
	if (!file.create(file_name))
		file.error_create();
	
	string line;
	string text;
	file.save_line("# OPTIONS\n");
	
	for (int zz=0;zz<NUMBER_OF_OPTIONS;zz++)
	{
		switch(zz)
		{
			
           case OPTION_LETTERS:
			   text="Constant letters in inventory: ";
			   break;
           case OPTION_EXPERIENCE:
			   text="Show experience needed for new level: ";
			   break;
           case OPTION_AUTO_AIM:
			   text="Auto aim nearest enemy: ";
			   break;
           case OPTION_AIM_IF_NO_ENEMY:
			   text="Auto aim if no enemy selected: ";
			   break;
           case OPTION_DONT_LET_SWAP:
			   text="Don't let friendly monsters to swap place with you: ";
			   break;								   
           case OPTION_CURSOR:
			   text="Show cursor on player: ";
			   break;								   
           case OPTION_GETALL:
			   text="Always get the whole package of items: ";
			   break;								   
		   default:
			   text="Unknown option!!! : ";
		}

	    if (options.number[zz]==true)
		   text+="TRUE";
	    else
		   text+="FALSE";

		file.save_line(text);		
	}		
	file.close();
	return true;
}	


CHero *CToSave::LoadPlayer(string name)
{
#ifdef LOAD_DEBUG
	fprintf(loadlog,"TOSAVE::LoadPlayer\n");
#endif

	FILE *fp, *old_fp;

	CHero *new_one;
	string s;
	unsigned long UN;

	old_fp = FilePointer;

	fp = fopen(string(string("save/") + name + ".sav").c_str(),"rb+");
	if (fp==NULL)
	{
		fprintf(stderr,"ERROR READ: Error opening file %s\n",string(string("save/") + name + ".sav").c_str());		
		keyboard.wait_for_key(); 
		exit(84);
	}
	CToSave::InitFilePointer(fp);

	LoadString(s);
	LoadLong(UN);

	if (s != "HERO")
	{
	   fprintf(stderr,"ERROR READ: HERO string expected, not %s\n",s.c_str());
	   keyboard.wait_for_key(); 
	   exit(215);
	}


	new_one = new CHero;
	if (new_one == NULL)
	{
	   fprintf(stderr,"ERROR READ: Error creating HERO\n");
	   keyboard.wait_for_key(); 
	   exit(216);
	}


	fseek(fp,0,SEEK_SET);
	new_one->LoadObject();

	fclose(fp);
	CToSave::InitFilePointer(old_fp);
	CToSave::LoadOptions(string("save/") + name + ".opt");
	CToSave::LoadMessageBuffer(string("save/") + name + ".msg");			

	return new_one;
}

CToSave::CToSave()
{
	UniqueNumber = ++LastUniqueNumber;
        if (UniqueNumber==0) // UniqueNumber overflow - it should NEVER happen
	{
           fprintf(stderr,"Wow, the universe has been destroyed! Contact the author!\n\n");
           keyboard.wait_for_key(); 
		   exit(3);
	}
	ClassName = "UNDEFINED";
	IsAlreadySaved = false;
}

void CToSave::SaveList(ptr_list &value)
{
   int size;
   ptr_list::iterator m,_m;
   CToSave *temp;

	SaveString("LIST");
	size = value.size(); // size listy
	SaveInt(size); 

   for(m=value.begin(),_m=value.end(); m!=_m; m++)
   {
       temp=(CToSave *)*m;
 	   temp->SaveObject();
   }
}

void CToSave::LoadList(ptr_list &value)
{
#ifdef LOAD_DEBUG
	fprintf(loadlog,"* TOSAVE::LoadList\n");
#endif

	string t;
	int size;
	LoadString(t);

	if (t != "LIST")
	{
	   fprintf(stderr,"ERROR READ: string LIST expected, not %s\n",t.c_str());
	   keyboard.wait_for_key(); 
	   exit(215);
	}

	LoadInt(size);
	CToSave *new_one;

	for (int index = 0; index<size;index++)
	{
		new_one = CreateObject();
#ifdef LOAD_DEBUG
		fprintf(loadlog,"-> List element: %d,%d\n",index,size);
#endif
		new_one->LoadObject();
		value.push_back(new_one);
	}
}

void CToSave::InitFilePointer(FILE *fp) // static
{
	FilePointer = fp;
}

void CToSave::SaveUniqueNumber(unsigned long value)
{
	fwrite(&value,sizeof(unsigned long),1,FilePointer);
}

void CToSave::LoadUniqueNumber(CToSave **ptr)
{
	para p;
	unsigned long value;
	fread(&value,sizeof(unsigned long),1,FilePointer);
	if (value>LastUniqueNumber) // this shouldn't happen
		LastUniqueNumber = value+1;

	p.first = ptr;
	p.second = value;
	ListToFix.push_back(p);

#ifdef LOAD_DEBUG
	fprintf(loadlog,"LoadUN: %d\n",(int) value);
	if (value==786445)
	{
		int z=0;
	}
#endif
}


void CToSave::SaveBool(bool value)
{
	fwrite(&value,sizeof(bool),1,FilePointer);
}

void CToSave::LoadBool(bool &value)
{
	fread(&value,sizeof(bool),1,FilePointer);
#ifdef LOAD_DEBUG
	fprintf(loadlog,"LoadBool: %d\n",(int) value);
#endif
}


void CToSave::SaveChar(char value)
{
	fwrite(&value,sizeof(char),1,FilePointer);
}

void CToSave::LoadChar(char &value)
{
	fread(&value,sizeof(char),1,FilePointer);
#ifdef LOAD_DEBUG
	if (value != 7)
		fprintf(loadlog,"LoadChar: %c (%d, 0x%x)\n",value,value,value);
	else
		fprintf(loadlog,"LoadChar: <7> (%d, 0x%x)\n",value,value);
#endif
}


void CToSave::SaveInt(int value)
{
   fwrite(&value,sizeof(int),1,FilePointer);
}

void CToSave::LoadInt(int &value)
{
	fread(&value,sizeof(int),1,FilePointer);
#ifdef LOAD_DEBUG
	fprintf(loadlog,"LoadInt: %d\n",value);
#endif
}

void CToSave::SaveLong(unsigned long value)
{
	fwrite(&value,sizeof(long),1,FilePointer);
}

void CToSave::LoadLong(unsigned long &value)
{
	fread(&value,sizeof(long),1,FilePointer);
#ifdef LOAD_DEBUG
	fprintf(loadlog,"LoadLong: %d\n",(int) value);
#endif
}


void CToSave::SaveString(string value)
{
	int string_size;
	string_size=value.size();
	fwrite(&string_size,sizeof(int),1,FilePointer);
	fwrite(value.c_str(),string_size,1,FilePointer);	
}

void CToSave::LoadString(string &value)
{
	char t[1000];
	unsigned int string_size;
	fread(&string_size,sizeof(int),1,FilePointer);
   if (string_size>=1000)
   {
#ifdef LOAD_DEBUG
	  fprintf(loadlog,"ERROR READ: size string >1000\n");
      fprintf(stderr,"ERROR READ: size string >1000\n");
#endif
      keyboard.wait_for_key(); 
	  exit(222);
   }   
	fread(t,sizeof(char),string_size,FilePointer);
	t[string_size]='\0';
	value = string(t);
#ifdef LOAD_DEBUG
	string to_print=value;
	size_t pos = to_print.find(7);
	if (pos!=string::npos)
		to_print[pos]='+';
	fprintf(loadlog,"LoadString: %s (size: %d)\n",to_print.c_str(),string_size);
#endif
}

unsigned long CToSave::SaveObject()
{
	if (IsAlreadySaved == true)
	{
	   fprintf(stderr,"ERROR SAVE:  TOSAVE already saved\n");
	   keyboard.wait_for_key(); 
	   exit(230);
	}

	SaveString(ClassName);
	SaveLong(UniqueNumber);

	IsAlreadySaved = true;
	saved.push_back(this); // dodajemy object do zapisanych

	return UniqueNumber;
}

void CToSave::SaveNULLObject()
{
	SaveString("NULL_OBJECT");
}

bool CToSave::LoadObject()
{

	LoadString(ClassName);
	LoadLong(UniqueNumber);

#ifdef LOAD_DEBUG
	fprintf(loadlog,"* TOSAVE::LoadObject - %s\n",ClassName.c_str());
#endif

	if (AddressesOfObjects.find(UniqueNumber)!=AddressesOfObjects.end())
	{
		CToSave *adr = AddressesOfObjects[UniqueNumber];
		   fprintf(stderr,"ERROR: Map has assigned address for %d\n",UniqueNumber);
		   keyboard.wait_for_key(); 
		   exit(234);
	}
	AddressesOfObjects[UniqueNumber] = this;

	return true;
}

void CToSave::UpdatePointers()
{
	to_fix::iterator m,_m;
	addresses::iterator l,_l;

	para p;
	pair <unsigned long, CToSave *> para_UN;

	CToSave *adres;
	CToSave **mod;

	unsigned long UN;

	for (m=ListToFix.begin(),_m = ListToFix.end();m!=_m;m++)
	{
		p = *m;
		UN = p.second;
		if (UN!=0 && AddressesOfObjects.find(UN)==AddressesOfObjects.end())
		{
		   fprintf(stderr,"ERROR UPDATE POINT: Not found for %d (%x)\n",UN,UN);
		   keyboard.wait_for_key(); 
		   exit(231);
		}

		adres = AddressesOfObjects [UN];
		mod = p.first;
		*mod = adres;
	}

	return;
}

unsigned long CTile::SaveObject()
{
	if (IsAlreadySaved == true)
	{
	   fprintf(stderr,"ERROR SAVE:  TOSAVE already saved\n");
	   keyboard.wait_for_key(); 
	   exit(230);
	}

	CToSave::SaveObject();

	SaveInt(positionX);
	SaveInt(positionY);
	SaveString(name);
	SaveChar(color);
	SaveChar(tile);

	return UniqueNumber;
}

bool CTile::LoadObject()
{
#ifdef LOAD_DEBUG
	fprintf(loadlog,"* TILE::LoadObject\n");
#endif

	CToSave::LoadObject();

	LoadInt(positionX);
	LoadInt(positionY);
	LoadString(name);
	LoadChar(color);
	LoadChar(tile);
	return true;
}

unsigned long STAIRS::SaveObject()
{
	if (IsAlreadySaved == true)
	{
	   fprintf(stderr,"ERROR SAVE:  TOSAVE already saved\n");
	   keyboard.wait_for_key(); 
	   exit(230);
	}

	CToSave::SaveObject();

	SaveBool(lead_up);
	SaveInt(number);
	SaveInt(x);
	SaveInt(y);
	SaveString(to_where);
	SaveString(name);

	return UniqueNumber;
}

bool STAIRS::LoadObject()
{
#ifdef LOAD_DEBUG
	fprintf(loadlog,"* STAIRS::LoadObject\n");
#endif

	CToSave::LoadObject();

	LoadBool(lead_up);
	LoadInt(number);
	LoadInt(x);
	LoadInt(y);
	LoadString(to_where);
	LoadString(name);
	return true;
}



unsigned long CItem::SaveObject()
{
	if (IsAlreadySaved == true)
	{
	   fprintf(stderr,"ERROR SAVE:  TOSAVE already saved\n");
	   keyboard.wait_for_key(); 
	   exit(230);
	}

	CTile::SaveObject();

	SaveInt(category);
	
	if (owner!=NULL)
		SaveUniqueNumber(owner->UniqueNumber);
	else
		SaveUniqueNumber(NULL_UNIQUE_NUMBER);

	if (in_robot_shell!=NULL)
		SaveUniqueNumber(in_robot_shell->UniqueNumber);
	else
		SaveUniqueNumber(NULL_UNIQUE_NUMBER);	

	SaveInt(DMG);
	SaveInt(HIT);
	SaveInt(DEF);
	SaveBool(on_ground);
	SaveInt(energy_activated);	
	SaveLong(properties);
	SaveInt(hit_points);
	SaveInt(max_hit_points);
	SaveInt(required_strength);
	SaveInt(weight);
	SaveInt(price);
	SaveInt(skill_to_use);
	SaveString(activities);
	SaveInt(inventory_letter);
	SaveBool(is_dead);
	SaveInt(purpose);	

	return UniqueNumber;
}

bool CItem::LoadObject()
{
#ifdef LOAD_DEBUG
	fprintf(loadlog,"* ITEM::LoadObject\n");
#endif
	CTile::LoadObject();

	LoadInt(category);
	
	LoadUniqueNumber( (CToSave **) &owner);
	LoadUniqueNumber( (CToSave **) &in_robot_shell);
	
	LoadInt(DMG);
	LoadInt(HIT);
	LoadInt(DEF);
	LoadBool(on_ground);
	LoadInt(energy_activated);
	LoadLong(properties);
	LoadInt(hit_points);
	LoadInt(max_hit_points);
	LoadInt(required_strength);
	LoadInt(weight);
	LoadInt(price);
	LoadInt(skill_to_use);
	LoadString(activities);
	LoadInt(inventory_letter);
	LoadBool(is_dead);
	LoadInt(purpose);
	

	return true;
}

unsigned long CCorpse::SaveObject()
{
	if (IsAlreadySaved == true)
	{
	   fprintf(stderr,"ERROR SAVE:  TOSAVE already saved\n");
	   keyboard.wait_for_key(); 
	   exit(230);
	}

	CItem::SaveObject();

	SaveString(of_what);
	SaveInt(rotting_state);

	return UniqueNumber;
}

bool CCorpse::LoadObject()
{
#ifdef LOAD_DEBUG
	fprintf(loadlog,"* CORPSE::LoadObject\n");
#endif
	CItem::LoadObject();

	LoadString(of_what);
	LoadInt(rotting_state);
	return true;
}

unsigned long CTrash::SaveObject()
{
	if (IsAlreadySaved == true)
	{
		fprintf(stderr,"ERROR SAVE:  TOSAVE already saved\n");
		keyboard.wait_for_key(); 
		exit(230);
	}
	
	CItem::SaveObject();
	
	return UniqueNumber;
}

bool CTrash::LoadObject()
{
#ifdef LOAD_DEBUG
	fprintf(loadlog,"* TRASH::LoadObject\n");
#endif
	CItem::LoadObject();
	
	return true;
}


unsigned long CRepairKit::SaveObject()
{
	if (IsAlreadySaved == true)
	{
		fprintf(stderr,"ERROR SAVE:  TOSAVE already saved\n");
		keyboard.wait_for_key(); 
		exit(230);
	}
	
	CItem::SaveObject();

	SaveInt(regeneration_progress);
	SaveInt(regeneration_time);
	
	return UniqueNumber;
}

bool CRepairKit::LoadObject()
{
#ifdef LOAD_DEBUG
	fprintf(loadlog,"* REPAIR_KIT::LoadObject\n");
#endif
	CItem::LoadObject();
	
	LoadInt(regeneration_progress);
	LoadInt(regeneration_time);

	return true;
}

//////////////////////////////////////////////////////////////////////////
unsigned long CProgrammator::SaveObject()
{
	if (IsAlreadySaved == true)
	{
		fprintf(stderr,"ERROR SAVE:  TOSAVE already saved\n");
		keyboard.wait_for_key(); 
		exit(230);
	}
	
	CItem::SaveObject();
	return UniqueNumber;
}

bool CProgrammator::LoadObject()
{
#ifdef LOAD_DEBUG
	fprintf(loadlog,"* PROGRAMMATOR::LoadObject\n");
#endif
	CItem::LoadObject();
	return true;
}
//////////////////////////////////////////////////////////////////////////
unsigned long CBattery::SaveObject()
{
	if (IsAlreadySaved == true)
	{
		fprintf(stderr,"ERROR SAVE:  TOSAVE already saved\n");
		keyboard.wait_for_key(); 
		exit(230);
	}
	
	CItem::SaveObject();
	SaveInt(max_capacity);
	SaveInt(capacity); 
	SaveInt(regeneration_speed); 
	
	return UniqueNumber;
}

bool CBattery::LoadObject()
{
#ifdef LOAD_DEBUG
	fprintf(loadlog,"* BATTERY::LoadObject\n");
#endif
	CItem::LoadObject();
	LoadInt(max_capacity);
	LoadInt(capacity); 
	LoadInt(regeneration_speed); 
	return true;
}


unsigned long CPill::SaveObject()
{
	if (IsAlreadySaved == true)
	{
		fprintf(stderr,"ERROR SAVE:  TOSAVE already saved\n");
		keyboard.wait_for_key(); 
		exit(230);
	}
	
	CCountable::SaveObject();
	
	SaveInt(PWR);
	
	return UniqueNumber;
}

bool CPill::LoadObject()
{
#ifdef LOAD_DEBUG
	fprintf(loadlog,"* PILL::LoadObject\n");
#endif
	CCountable::LoadObject();
	
	LoadInt(PWR);
	return true;
}

unsigned long CProcessor::SaveObject()
{
	if (IsAlreadySaved == true)
	{
		fprintf(stderr,"ERROR SAVE:  TOSAVE already saved\n");
		keyboard.wait_for_key(); 
		exit(230);
	}
	
	CItem::SaveObject();

	SaveInt(frequency);	
	SaveInt(quality);	
	SaveInt(group_affiliation);	
	SaveString(program);	

	if (where_placed!=NULL)
		SaveUniqueNumber(where_placed->UniqueNumber);
	else
		SaveUniqueNumber(NULL_UNIQUE_NUMBER);
		
	return UniqueNumber;
}

bool CProcessor::LoadObject()
{
#ifdef LOAD_DEBUG
	fprintf(loadlog,"* PROCESSOR::LoadObject\n");
#endif
	CItem::LoadObject();

	LoadInt(frequency);	
	LoadInt(quality);
	LoadInt(group_affiliation);	
	LoadString(program);	

	LoadUniqueNumber( (CToSave **) &where_placed);	
	
	return true;
}

unsigned long CRobotLeg::SaveObject()
{
	if (IsAlreadySaved == true)
	{
		fprintf(stderr,"ERROR SAVE:  TOSAVE already saved\n");
		keyboard.wait_for_key(); 
		exit(230);
	}
	
	CItem::SaveObject();
	
	SaveInt(move_power);	
	
	return UniqueNumber;
}

bool CRobotLeg::LoadObject()
{
#ifdef LOAD_DEBUG
	fprintf(loadlog,"* ROBOT_LEG::LoadObject\n");
#endif
	CItem::LoadObject();
	
	LoadInt(move_power);	
	
	return true;
}



unsigned long CONTROLLER::SaveObject()
{
	if (IsAlreadySaved == true)
	{
	   fprintf(stderr,"ERROR SAVE:  TOSAVE already saved\n");
	   keyboard.wait_for_key(); 
	   exit(230);
	}

	CItem::SaveObject();

	return UniqueNumber;
}

bool CONTROLLER::LoadObject()
{
#ifdef LOAD_DEBUG
	fprintf(loadlog,"* CONTROLLER::LoadObject\n");
#endif
	CItem::LoadObject();

	return true;
}

unsigned long CCountable::SaveObject()
{
	if (IsAlreadySaved == true)
	{
		fprintf(stderr,"ERROR SAVE:  TOSAVE already saved\n");
		keyboard.wait_for_key(); 
		exit(230);
	}
	CItem::SaveObject();
	
	SaveInt(quantity);	
	return UniqueNumber;
}

bool CCountable::LoadObject()
{
#ifdef LOAD_DEBUG
	fprintf(loadlog,"* COUNTABLE::LoadObject\n");
#endif
	CItem::LoadObject();
	LoadInt(quantity);
	return true;
}


unsigned long CAmmo::SaveObject()
{
	if (IsAlreadySaved == true)
	{
	   fprintf(stderr,"ERROR SAVE:  TOSAVE already saved\n");
	   keyboard.wait_for_key(); 
	   exit(230);
	}

	CCountable::SaveObject();

	SaveInt(PWR);
	SaveInt(ACC);
	SaveInt(ammo_type);

	if (in_weapon!=NULL)
		SaveUniqueNumber(in_weapon->UniqueNumber);
	else
		SaveUniqueNumber(NULL_UNIQUE_NUMBER);

	return UniqueNumber;
}

bool CAmmo::LoadObject()
{
#ifdef LOAD_DEBUG
	fprintf(loadlog,"* AMMO::LoadObject\n");
#endif

	CCountable::LoadObject();
	
	LoadInt(PWR);
	LoadInt(ACC);
	LoadInt(ammo_type);

	LoadUniqueNumber( (CToSave **) &in_weapon);
	return true;
}

unsigned long CRangedWeapon::SaveObject()
{
	if (IsAlreadySaved == true)
	{
	   fprintf(stderr,"ERROR SAVE:  TOSAVE already saved\n");
	   keyboard.wait_for_key(); 
	   exit(230);
	}

	CItem::SaveObject();

	SaveInt(ammo_type);
	SaveInt(PWR);
	SaveInt(ACC);
	SaveInt(energy_PWR);
	SaveInt(energy_ACC);	
	SaveInt(magazine_capacity);
	SaveInt(fire_type);
	SaveInt(available_fire_types);
	ammo.SaveObject(); // ??? Czy moze jako wskaznik?

	return UniqueNumber;
}

bool CRangedWeapon::LoadObject()
{
#ifdef LOAD_DEBUG
	fprintf(loadlog,"* RANGED_WEAPON::LoadObject\n");
#endif

	CItem::LoadObject();

	LoadInt(ammo_type);
	LoadInt(PWR);
	LoadInt(ACC);
	LoadInt(energy_PWR);
	LoadInt(energy_ACC);
	LoadInt(magazine_capacity);
	LoadInt(fire_type);
	LoadInt(available_fire_types);
	ammo.LoadObject(); // ??? Czy moze jako wskaznik?
	return true;
}

unsigned long CBaseArmor::SaveObject()
{
	if (IsAlreadySaved == true)
	{
	   fprintf(stderr,"ERROR SAVE:  TOSAVE already saved\n");
	   keyboard.wait_for_key(); 
	   exit(230);
	}

	CItem::SaveObject();

	SaveInt(ARM);
	SaveInt(MOD_STR);
	SaveInt(MOD_DEX);
	SaveInt(MOD_SPD);

	SaveInt(energy_ARM);
	SaveInt(energy_MOD_STR);
	SaveInt(energy_MOD_DEX);
	SaveInt(energy_MOD_SPD);
	SaveLong(energy_properties);
	SaveLong(energy_real_properties);	
	
	return UniqueNumber;
}

bool CBaseArmor::LoadObject()
{
#ifdef LOAD_DEBUG
	fprintf(loadlog,"* BASE_ARMOR::LoadObject\n");
#endif

	CItem::LoadObject();

	LoadInt(ARM);
	LoadInt(MOD_STR);
	LoadInt(MOD_DEX);
	LoadInt(MOD_SPD);

	LoadInt(energy_ARM);
	LoadInt(energy_MOD_STR);
	LoadInt(energy_MOD_DEX);
	LoadInt(energy_MOD_SPD);
	LoadLong(energy_properties);
	LoadLong(energy_real_properties);

	return true;
}

unsigned long NO_ARMOR::SaveObject()
{
	if (IsAlreadySaved == true)
	{
	   fprintf(stderr,"ERROR SAVE:  TOSAVE already saved\n");
	   keyboard.wait_for_key(); 
	   exit(230);
	}

	CBaseArmor::SaveObject();

	return UniqueNumber;
}

bool NO_ARMOR::LoadObject()
{
#ifdef LOAD_DEBUG
	fprintf(loadlog,"* NO_ARMOR::LoadObject\n");
#endif

	CBaseArmor::LoadObject();
	return true;
}

unsigned long CArmor::SaveObject()
{
	if (IsAlreadySaved == true)
	{
	   fprintf(stderr,"ERROR SAVE:  TOSAVE already saved\n");
	   keyboard.wait_for_key(); 
	   exit(230);
	}

	CBaseArmor::SaveObject();	
	return UniqueNumber;
}

bool CArmor::LoadObject()
{
#ifdef LOAD_DEBUG
	fprintf(loadlog,"* ARMOR::LoadObject\n");
#endif

	CBaseArmor::LoadObject();	
	return true;
}

unsigned long CHandWeapon::SaveObject()
{
	if (IsAlreadySaved == true)
	{
	   fprintf(stderr,"ERROR SAVE:  TOSAVE already saved\n");
	   keyboard.wait_for_key(); 
	   exit(230);
	}

	CItem::SaveObject();

	SaveInt(energy_DMG);
	SaveInt(energy_HIT);
	SaveInt(energy_DEF);
	SaveLong(energy_properties);	
	SaveLong(energy_real_properties);
	

	return UniqueNumber;
}

bool CHandWeapon::LoadObject()
{
#ifdef LOAD_DEBUG
	fprintf(loadlog,"* HAND_WEAPON::LoadObject\n");
#endif

	CItem::LoadObject();

	LoadInt(energy_DMG);
	LoadInt(energy_HIT);
	LoadInt(energy_DEF);
	LoadLong(energy_properties);
	LoadLong(energy_real_properties);
	
	return true;
}

unsigned long CGrenade::SaveObject()
{
	if (IsAlreadySaved == true)
	{
	   fprintf(stderr,"ERROR SAVE:  TOSAVE already saved\n");
	   keyboard.wait_for_key(); 
	   exit(230);
	}

	CItem::SaveObject();

	SaveInt(counter);
	SaveBool(active);
	SaveBool(works_now);
	SaveInt(PWR);
	SaveInt(RNG);
	SaveInt(DLY);

	return UniqueNumber;
}

bool CGrenade::LoadObject()
{
#ifdef LOAD_DEBUG
	fprintf(loadlog,"* GRENADE::LoadObject\n");
#endif

	CItem::LoadObject();

	LoadInt(counter);
	LoadBool(active);
	LoadBool(works_now);
	LoadInt(PWR);
	LoadInt(RNG);
	LoadInt(DLY);
	return true;
}


unsigned long GAS::SaveObject()
{
	if (IsAlreadySaved == true)
	{
	   fprintf(stderr,"ERROR SAVE:  TOSAVE already saved\n");
	   keyboard.wait_for_key(); 
	   exit(230);
	}

	CTile::SaveObject();

	SaveInt(density);
	SaveLong(properties);

	return UniqueNumber;
}

bool GAS::LoadObject()
{
#ifdef LOAD_DEBUG
	fprintf(loadlog,"* GAS::LoadObject\n");
#endif

	CTile::LoadObject();

	LoadInt(density);
	LoadLong(properties);
	return true;
}

unsigned long FIRE::SaveObject()
{
	if (IsAlreadySaved == true)
	{
	   fprintf(stderr,"ERROR SAVE:  TOSAVE already saved\n");
	   keyboard.wait_for_key(); 
	   exit(230);
	}

	GAS::SaveObject();

	return UniqueNumber;
}

bool FIRE::LoadObject()
{
#ifdef LOAD_DEBUG
	fprintf(loadlog,"* FIRE::LoadObject\n");
#endif

	GAS::LoadObject();
	return true;
}

unsigned long CAttribute::SaveObject()
{
	if (IsAlreadySaved == true)
	{
	   fprintf(stderr,"ERROR SAVE:  TOSAVE already saved\n");
	   keyboard.wait_for_key(); 
	   exit(230);
	}

	SaveInt(val);
	SaveInt(max);
	SaveInt(mod);

	SaveInt(type);
	if (owner!=NULL)
		SaveUniqueNumber(owner->UniqueNumber);
	else
		SaveUniqueNumber(NULL_UNIQUE_NUMBER);
	

	return UniqueNumber;
}

bool CAttribute::LoadObject()
{
#ifdef LOAD_DEBUG
	fprintf(loadlog,"* ATTRIBUTE::LoadObject\n");
#endif
	
	LoadInt(val);
	LoadInt(max);
	LoadInt(mod);
	
	LoadInt(type);	
	LoadUniqueNumber( (CToSave **) &owner);
	
	return true;
}


unsigned long CMonster::SaveObject()
{
	int index;

	if (IsAlreadySaved == true)
	{
	   fprintf(stderr,"ERROR SAVE:  TOSAVE already saved\n");
	   keyboard.wait_for_key(); 
	   exit(230);
	}

	CTile::SaveObject();

	SaveInt(last_pX);
	SaveInt(last_pY);
	SaveInt(special_properties);
	
	SaveInt(category);
	
	SaveInt(self_treatment);
	SaveBool(is_dead);
	
	fov_radius.SaveObject();

	SaveInt(size);
	SaveInt(weight);

	hit_points.SaveObject();
	energy_points.SaveObject();
	strength.SaveObject();
	dexterity.SaveObject();
	endurance.SaveObject();
	intelligence.SaveObject();
	speed.SaveObject();
	metabolism.SaveObject();

	SaveInt(group_affiliation);
	SaveInt(direction);

	unarmed.SaveObject();
	no_armor.SaveObject();

	if (weapon!=NULL)
		SaveUniqueNumber(weapon->UniqueNumber);
	else
		SaveUniqueNumber(NULL_UNIQUE_NUMBER);

	if (armor!=NULL)
		SaveUniqueNumber(armor->UniqueNumber);
	else
		SaveUniqueNumber(NULL_UNIQUE_NUMBER);

	SaveLong(next_action_time);

	SaveLong(seen_last_time_in_turn);
	SaveLong(experience_for_kill);

// player nie zapisuje of enemy, aby przy zmianie poziomu nie bylo bledu
	if (enemy!=NULL && this!=level.player) 
		SaveUniqueNumber(enemy->UniqueNumber);
	else
		SaveUniqueNumber(NULL_UNIQUE_NUMBER);

	for (index = 0; index < NUMBER_OF_SKILLS ; index++)
		SaveInt(skill[index]);

	for (index = 0; index < NUMBER_OF_RESISTS ; index++)
		SaveInt(resist[index]);

	for (index = 0; index < NUMBER_OF_STATES ; index++)
		SaveInt(state[index]);

	return UniqueNumber;
}

bool CMonster::LoadObject()
{
#ifdef LOAD_DEBUG
	fprintf(loadlog,"* MONSTER::LoadObject\n");
#endif

	int index;

	CTile::LoadObject();

	LoadInt(last_pX);
	LoadInt(last_pY);
	LoadInt(special_properties);
	
	LoadInt(category);
	
	LoadInt(self_treatment);
	LoadBool(is_dead);
	
	fov_radius.LoadObject();

	LoadInt(size);
	LoadInt(weight);

	hit_points.LoadObject();
	energy_points.LoadObject();
	strength.LoadObject();
	dexterity.LoadObject();
	endurance.LoadObject();
	intelligence.LoadObject();
	speed.LoadObject();
	metabolism.LoadObject();

	LoadInt(group_affiliation);
	LoadInt(direction);

	unarmed.LoadObject();
	no_armor.LoadObject();

	LoadUniqueNumber( (CToSave **) &weapon);

	LoadUniqueNumber( (CToSave **) &armor);

	LoadLong(next_action_time);

	seen_now=false;
	
	LoadLong(seen_last_time_in_turn);
	LoadLong(experience_for_kill);

	LoadUniqueNumber( (CToSave **) &enemy);

	for (index = 0; index < NUMBER_OF_SKILLS ; index++)
		LoadInt(skill[index]);

	for (index = 0; index < NUMBER_OF_RESISTS ; index++)
		LoadInt(resist[index]);

	for (index = 0; index < NUMBER_OF_STATES ; index++)
		LoadInt(state[index]);
	return true;
}

unsigned long CEnemy::SaveObject()
{
	if (IsAlreadySaved == true)
	{
	   fprintf(stderr,"ERROR SAVE:  TOSAVE already saved\n");
	   keyboard.wait_for_key(); 
	   exit(230);
	}

	CMonster::SaveObject();

	SaveInt(camping);
	SaveLong(enemy_last_seen_in_turn);		
	SaveInt(last_x_of_enemy);
	SaveInt(last_y_of_enemy);
	SaveInt(last_direction_of_enemy);
	SaveBool(is_at_enemy_position);
	SaveInt(last_direction_of_enemy);
	SaveInt(actual_behaviour);	
	SaveBool(turned_to_fight);
	return UniqueNumber;
}

bool CEnemy::LoadObject()
{
#ifdef LOAD_DEBUG
	fprintf(loadlog,"* ENEMY::LoadObject\n");
#endif

	CMonster::LoadObject();

	LoadInt(camping);
	LoadLong(enemy_last_seen_in_turn);		
	LoadInt(last_x_of_enemy);
	LoadInt(last_y_of_enemy);
	LoadInt(last_direction_of_enemy);
	LoadBool(is_at_enemy_position);
	LoadInt(last_direction_of_enemy);
	LoadInt(actual_behaviour);
	LoadBool(turned_to_fight);
	return true;
}


unsigned long CAnimal::SaveObject()
{
	if (IsAlreadySaved == true)
	{
	   fprintf(stderr,"ERROR SAVE:  TOSAVE already saved\n");
	   keyboard.wait_for_key(); 
	   exit(230);
	}

	CEnemy::SaveObject();

	return UniqueNumber;
}

bool CAnimal::LoadObject()
{
#ifdef LOAD_DEBUG
	fprintf(loadlog,"* ANIMAL::LoadObject\n");
#endif

	CEnemy::LoadObject();
	return true;
}

unsigned long CWorm::SaveObject()
{
	if (IsAlreadySaved == true)
	{
		fprintf(stderr,"ERROR SAVE:  TOSAVE already saved\n");
		keyboard.wait_for_key(); 
		exit(230);
	}

	CEnemy::SaveObject();
	if (prev!=NULL)
		SaveUniqueNumber(prev->UniqueNumber);
	else
		SaveUniqueNumber(NULL_UNIQUE_NUMBER);

	if (next!=NULL)
		SaveUniqueNumber(next->UniqueNumber);
	else
		SaveUniqueNumber(NULL_UNIQUE_NUMBER);

	SaveInt(length_left);
	SaveString(segment_name);
	SaveString(tail_name);

	return UniqueNumber;
}

bool CWorm::LoadObject()
{
#ifdef LOAD_DEBUG
	fprintf(loadlog,"* WORM::LoadObject\n");
#endif

	CEnemy::LoadObject();

	LoadUniqueNumber( (CToSave **) &prev);
	LoadUniqueNumber( (CToSave **) &next);
	LoadInt(length_left);
	LoadString(segment_name);
	LoadString(tail_name);
	return true;
}


unsigned long CMadDoctor::SaveObject()
{
	if (IsAlreadySaved == true)
	{
		fprintf(stderr,"ERROR SAVE:  TOSAVE already saved\n");
		keyboard.wait_for_key(); 
		exit(230);
	}
	
	CIntelligent::SaveObject();
	SaveInt(yelling_chance);
	tail.SaveObject();	
	
	return UniqueNumber;
}

bool CMadDoctor::LoadObject()
{
#ifdef LOAD_DEBUG
	fprintf(loadlog,"* MADDOCTOR::LoadObject\n");
#endif
	
	CIntelligent::LoadObject();
	LoadInt(yelling_chance);
	tail.LoadObject();	
	
	return true;
}


unsigned long CSearchLight::SaveObject()
{
	if (IsAlreadySaved == true)
	{
		fprintf(stderr,"ERROR SAVE:  TOSAVE already saved\n");
		keyboard.wait_for_key(); 
		exit(230);
	}

	CEnemy::SaveObject();
	SaveInt(destination_x);
	SaveInt(destination_y);
	
	return UniqueNumber;
}

bool CSearchLight::LoadObject()
{
#ifdef LOAD_DEBUG
	fprintf(loadlog,"* SEARCHLIGHT::LoadObject\n");
#endif
	
	CEnemy::LoadObject();
	LoadInt(destination_x);
	LoadInt(destination_y);
	return true;
}



unsigned long CIntelligent::SaveObject()
{
	if (IsAlreadySaved == true)
	{
	   fprintf(stderr,"ERROR SAVE:  TOSAVE already saved\n");
	   keyboard.wait_for_key(); 
	   exit(230);
	}

	CEnemy::SaveObject();

	SaveLong(AI_restrictions);	
	SaveLong(turns_of_searching_for_enemy);	
	SaveLong(turns_of_calm);	
	SaveInt(misses_in_shooting);
	SaveInt(run_away_to_x);
	SaveInt(run_away_to_y);
	SaveList(backpack);
	SaveInt(max_items_in_backpack);
	SaveInt(items_in_backpack);

	if (ready_weapon!=NULL) 
		SaveUniqueNumber(ready_weapon->UniqueNumber);
	else
		SaveUniqueNumber(NULL_UNIQUE_NUMBER);
	
	return UniqueNumber;
}

bool CIntelligent::LoadObject()
{
#ifdef LOAD_DEBUG
	fprintf(loadlog,"* INTELLIGENT::LoadObject\n");
#endif

	CEnemy::LoadObject();

	LoadLong(AI_restrictions);	
	LoadLong(turns_of_searching_for_enemy);	
	LoadLong(turns_of_calm);	
	LoadInt(misses_in_shooting);
	LoadInt(run_away_to_x);
	LoadInt(run_away_to_y);
	LoadList(backpack);
	LoadInt(max_items_in_backpack);
	LoadInt(items_in_backpack);

	LoadUniqueNumber( (CToSave **) &ready_weapon);
	
	return true;
}

unsigned long CRobotShell::SaveObject()
{
	if (IsAlreadySaved == true)
	{
		fprintf(stderr,"ERROR SAVE:  TOSAVE already saved\n");
		keyboard.wait_for_key(); 
		exit(230);
	}
	
	CItem::SaveObject();

	SaveInt(max_number_move_slots);
	SaveInt(max_number_action_slots);
	SaveInt(ARM);

	SaveList(move_slots);
	SaveList(action_slots);
	
	if (cpu!=NULL) 
		cpu->SaveObject();
	else
		SaveNULLObject();

	SaveString(last_robot_name);
	
	return UniqueNumber;
}

bool CRobotShell::LoadObject()
{
#ifdef LOAD_DEBUG
	fprintf(loadlog,"* ROBOT_SHELL::LoadObject\n");
#endif
	
	CItem::LoadObject();
	
	LoadInt(max_number_move_slots);
	LoadInt(max_number_action_slots);
	LoadInt(ARM);

	LoadList(move_slots);
	LoadList(action_slots);

	cpu = (CProcessor *) CreateObject();
	if (cpu!=NULL)
		cpu->LoadObject();

	LoadString(last_robot_name);
	
	return true;
}
//////////////////////////////////////////////////////////////////////////
unsigned long CRobot::SaveObject()
{
	if (IsAlreadySaved == true)
	{
		fprintf(stderr,"ERROR SAVE:  TOSAVE already saved\n");
		keyboard.wait_for_key(); 
		exit(230);
	}
	
	CEnemy::SaveObject();

	SaveInt(last_x_of_creator);
	SaveInt(last_y_of_creator);
	SaveBool(sees_creator);
	SaveLong(Creator_ID);
	

	assert(shell!=NULL); // tak byc nie powinno. Robot nie istnieje bez powloki!

	shell->SaveObject();
	
	return UniqueNumber;
}

bool CRobot::LoadObject()
{
#ifdef LOAD_DEBUG
	fprintf(loadlog,"* ROBOT::LoadObject\n");
#endif
	
	CEnemy::LoadObject();

	LoadInt(last_x_of_creator);
	LoadInt(last_y_of_creator);
	LoadBool(sees_creator);
	LoadLong(Creator_ID);
	

	shell = (CRobotShell *) CreateObject();
	if (shell!=NULL)
		shell->LoadObject();
	
	return true;
}


//////////////////////////////////////////////////////////////////////////

unsigned long CHero::SaveObject()
{
	if (IsAlreadySaved == true)
	{
	   fprintf(stderr,"ERROR SAVE:  TOSAVE already saved\n");
	   keyboard.wait_for_key(); 
	   exit(230);
	}

	// Inicjalizacja
	FILE *fp, *old_fp;

	old_fp = FilePointer;

	fp = fopen(string(string("save/") + level.player->name + ".sav").c_str(),"wb+");
	if (fp==NULL)
	{
		fprintf(stderr,"ERROR SAVE: Error creating file save/%s.sav",level.player->name.c_str());		
		keyboard.wait_for_key(); 
		exit(83);
	}
	CToSave::InitFilePointer(fp);

	// Zapisanie
	CIntelligent::SaveObject();

	SaveInt(exp_level);
	SaveLong(experience);
	SaveLong(level_at_experience);
	SaveLong(free_skill_pointes);
	SaveLong(adaptation_points);
	
	for (int index = 0; index < NUMBER_OF_SKILLS ; index++)
		SaveInt(experience_in_skill[index]);

	SaveString(level.ID); // jest na tym poziomie
	SaveString(ID_of_last_level);
	SaveInt(stairs_number_used);

	// save potworkow, ktore ida za graczem na new_one poziom
	SaveList(monsters_following_player_to_other_level);	
  
	// save pol¹czeñ poziomów
	// schody w dol
	 map_of_level_links::iterator m,_m;
	 list <string>::iterator j,_j;
	 pair < string , list < string > > para;

	 SaveInt(level.stairs_down.size());
	 for (m=level.stairs_down.begin(),_m=level.stairs_down.end();m!=_m;m++)
	 {
		 para = *m;
		 SaveString(para.first);
		 SaveInt(para.second.size()); // size listy
		 for (j = para.second.begin(),_j = para.second.end();j!=_j;j++)
		 {
			SaveString(*j); // name
		 }
	 }
	 // schody w gore
	 SaveInt(level.stairs_up.size());
	 for (m=level.stairs_up.begin(),_m=level.stairs_up.end();m!=_m;m++)
	 {
		 para = *m;
		 SaveString(para.first);
		 SaveInt(para.second.size()); // size listy
		 for (j = para.second.begin(),_j = para.second.end();j!=_j;j++)
		 {
			SaveString(*j); // name
		 }
	 }

	 // save znanych graczowi nazw

	 set_of_known_names::iterator nn;

	 // save liczby znanych
	 SaveInt(known_items.size());

	 // save kolejnych

	 for (nn=known_items.begin();nn!=known_items.end();nn++)
	 {
		 SaveString((*nn));
	 }

   // Zapisanie map wszystkich nieznanych items
	 // save sizei map
	 SaveInt(definitions.constantly_unknown_items.size());
	 // save wszystkich elementow
	 map_of_unknown_items::iterator mnp,_mnp;
	 mnp=definitions.constantly_unknown_items.begin();
	 _mnp=definitions.constantly_unknown_items.end();
	 
	 for (;mnp!=_mnp;mnp++)
	 {
		 SaveString((*mnp).first);
		 SaveString((*mnp).second.name);
		 SaveInt((*mnp).second.color);
	 }
	 


	//////////////////////////////////////////////////////////////////////////	
	fclose(fp);

	CToSave::InitFilePointer(old_fp);
	CToSave::SaveOptions(string("save/") + name + ".opt");		
	CToSave::SaveMessageBuffer(string("save/") + name + ".msg");		
	return UniqueNumber;
}

bool CHero::LoadObject()
{
#ifdef LOAD_DEBUG
	fprintf(loadlog,"* HERO::LoadObject\n");
#endif
	int index;

	CIntelligent::LoadObject();

	LoadInt(exp_level);
	LoadLong(experience);
	LoadLong(level_at_experience);
	LoadLong(free_skill_pointes);
	LoadLong(adaptation_points);	

	for (index = 0; index < NUMBER_OF_SKILLS ; index++)
		LoadInt(experience_in_skill[index]);

	LoadString(ID_of_level); // jest na tym poziomie
	LoadString(ID_of_last_level);
	LoadInt(stairs_number_used);

	// load potworkow, ktore ida za graczem na new_one poziom
	LoadList(monsters_following_player_to_other_level);		

	// load polaczen poziomow

	 int number_a, number_b, count;
	 string temp_a,temp_b;

	 if (level.stairs_down.size()!=0 || level.stairs_up.size()!=0)
	 {
	   fprintf(stderr,"ERROR READ:  Sa juz definitions stairs!\n");
	   keyboard.wait_for_key(); 
	   exit(192);
	 }

	// schody w dol

	 LoadInt(number_a);
	 for (index=0;index<number_a;index++)
	 {
		 LoadString(temp_a);
		 LoadInt(number_b); // size listy
		 for (count=0;count<number_b;count++)
		 {
			 LoadString(temp_b);
			 level.stairs_down[temp_a].push_back(temp_b);
		 }
	 }

	// schody w gore

	 LoadInt(number_a);
	 for (index=0;index<number_a;index++)
	 {
		 LoadString(temp_a);
		 LoadInt(number_b); // size listy
		 for (count=0;count<number_b;count++)
		 {
			 LoadString(temp_b);
			 level.stairs_up[temp_a].push_back(temp_b);
		 }
	 }

	 // load zestawu znanych graczowi nazw
	 
	 known_items.clear();
	 
	 // load liczby nieznanych
	 int number_nazw;
	 LoadInt(number_nazw);
	 string wczytany;
	 
	 // load kolejnych
	 
	 for (index=0;index<number_nazw;index++)
	 {
		 LoadString(wczytany);
		 add_known_item(wczytany);
	 }

 // Wczytanie map wszystkich nieznanych items
	 definitions.constantly_unknown_items.clear();
	 // load sizei map
	 int size_mapy;
	 unknown_item np;
	 LoadInt(size_mapy);
	 // Wczytanie wszystkich elementow

	 for (index=0;index<size_mapy;index++)
	 {
		 LoadString(wczytany);
		 LoadString(np.name);
		 LoadInt(np.color);	
		 definitions.constantly_unknown_items[wczytany]=np;
	 }	 	 

	UpdatePointers();

	return true;
}

unsigned long CLevel::SaveObject()
{
	if (IsAlreadySaved == true)
	{
	   fprintf(stderr,"ERROR SAVE:  TOSAVE already saved\n");
	   keyboard.wait_for_key(); 
	   exit(230);
	}
    clear_lists_to_delete();

	// Inicjalizacja
	FILE *fp;
	fp = fopen(string(string("save/") + player->name + "." + ID).c_str(),"wb+");
	if (fp==NULL)
	{
		fprintf(stderr,"ERROR SAVE: Error creating file save/%s",(level.player->name + "." + ID).c_str());				
		keyboard.wait_for_key(); 
		exit(82);
	}
	CToSave::InitFilePointer(fp);

	// save

	CToSave::SaveObject();

	SaveString(ID);
	SaveInt(fov_range);
	SaveLong(turn);

	ptr_list::iterator sm,_sm;
	CMonster *monster;
	// usuniecie ich z listy monsters na poziomie
	sm=player->monsters_following_player_to_other_level.begin();
	_sm=player->monsters_following_player_to_other_level.end();
	for (;sm!=_sm;sm++)
	{
		monster=(CMonster *) *sm;
		monster->ChangePosition(-1,-1);
		monsters.remove(monster);
	}
	
	SaveList(items_on_map);
	monsters.remove(player);
	SaveList(monsters);
	SaveList(gases_on_map);

	//////////////////////////////////////////////////////////////////////////	
	// Save map

	map.SaveObject();

	fclose(fp);

	// Save the player for case of game's crash

	level.player->SaveObject();

	// Set all saved items that are not saved, to make save possible again
	list_of_saved::iterator m,_m;

	for (m=saved.begin(),_m=saved.end();m!=_m;m++)
		((CToSave *) *m)->IsAlreadySaved = false;

	saved.clear();

	return UniqueNumber;
}

bool CLevel::LoadObject()
{
#ifdef LOAD_DEBUG
	fprintf(loadlog,"* LEVEL::LoadObject\n");
#endif
	// Initialize
	
	FILE *fp;
	fp = fopen(string(string("save/") + game.name_of_player + "." + level.ID).c_str(),"rb+");
	if (fp==NULL) // No such level!
	{
#ifdef LOAD_DEBUG
		fprintf(loadlog,"ERROR READ: Can not open file level save/%s",(level.player->name + "." + level.ID).c_str());						
#endif
		return false;
	}

	CToSave::InitFilePointer(fp);

	// save

	CToSave::LoadObject();

	LoadString(ID);	

	LoadInt(fov_range);	
	LoadLong(turn);

	LoadList(items_on_map);
	LoadList(monsters);
	LoadList(gases_on_map);

	// load map
	map.LoadObject();

	fclose(fp);

	CToSave::InitFilePointer(NULL);

	UpdatePointers();

	ListToFix.clear();
	AddressesOfObjects.clear();
   
	return true;
}

unsigned long CMap::SaveObject()
{
	if (IsAlreadySaved == true)
	{
	   fprintf(stderr,"ERROR SAVE:  TOSAVE already saved\n");
	   keyboard.wait_for_key(); 
	   exit(230);
	}

	CToSave::SaveObject();

	// save map
	SaveList(stairs_of_map);

	int x,y;

	for (x=0;x<MAPWIDTH;x++)
		for (y=0;y<MAPHEIGHT;y++)
			cells[x][y].SaveObject();

	return UniqueNumber;
}

bool CMap::LoadObject()
{
#ifdef LOAD_DEBUG
	fprintf(loadlog,"* MAP::LoadObject\n");
#endif

	CToSave::LoadObject();

// load map
	LoadList(stairs_of_map);

	int x,y;

	for (x=0;x<MAPWIDTH;x++)
		for (y=0;y<MAPHEIGHT;y++)
			cells[x][y].LoadObject();
	return true;
}

unsigned long CELL::SaveObject()
{
	SaveBool(seen);
	SaveChar(last_tile);
	SaveChar(seen_at_least_once);
	SaveString(name);
	SaveChar(tile);
	SaveInt(color);
	SaveBool(block_los);
	SaveBool(real_block_los);
	SaveBool(block_move);
	SaveBool(real_block_move);
	SaveBool(is_stairs);
	SaveBool(is_monster);
	SaveInt(number_of_items);
	SaveInt(number_of_gases);
	SaveInt(shield);
	SaveInt(hit_points);
	SaveInt(max_hit_points);
	SaveInt(about_open);
	SaveString(destroyed_to);
	return 0;
}


bool CELL::LoadObject()
{
	LoadBool(seen);
	LoadChar(last_tile);
	LoadChar(seen_at_least_once);
	LoadString(name);
	LoadChar(tile);
	LoadInt(color);
	LoadBool(block_los);
	LoadBool(real_block_los);
	LoadBool(block_move);
	LoadBool(real_block_move);
	LoadBool(is_stairs);
	LoadBool(is_monster);
	LoadInt(number_of_items);
	LoadInt(number_of_gases);
	LoadInt(shield);
	LoadInt(hit_points);
	LoadInt(max_hit_points);
	LoadInt(about_open);
	LoadString(destroyed_to);
	return true;
}

void CToSave::SaveLastUNToFile()
{
	FILE *fp, *old_fp;

	old_fp = FilePointer;

	fp = fopen(string(string("save/") + game.name_of_player + ".un").c_str(),"wb+");
	if (fp==NULL)
	{
		fprintf(stderr,"ERROR SAVE UniqueNumber");								
		keyboard.wait_for_key(); 
		exit(81);
	}
	InitFilePointer(fp);
	SaveLong(LastUniqueNumber);
	InitFilePointer(old_fp);

	fclose(fp);
}

void CToSave::LoadLastUNFromFile()
{
	FILE *fp, *old_fp;

	old_fp = FilePointer;

	fp = fopen(string(string("save/") + game.name_of_player + ".un").c_str(),"rb+");
	if (fp==NULL)
	{
		fprintf(stderr,"ERROR READ UniqueNumber");								
		keyboard.wait_for_key(); 
		exit(80);
	}
	InitFilePointer(fp);
	LoadLong(LastUniqueNumber);
	InitFilePointer(old_fp);

	fclose(fp);
}

void CToSave::OpenLogFile()
{
#ifdef LOAD_DEBUG
	loadlog = fopen("logfile.txt","wt+");
#endif 
}

void CToSave::CloseLogFile()
{
#ifdef LOAD_DEBUG
	fclose(loadlog);
	loadlog=NULL;
#endif
}

void CToSave::LogLevelChange(string to_log)
{
#ifdef LOAD_DEBUG
	fprintf(loadlog,"\n*************************************************\n");
	fprintf(loadlog,"Changing level to: %s\n", to_log.c_str());
#endif
}
