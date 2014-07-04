#ifndef ATTRIB_H
#define ATTRIB_H

/*
Ranged Weapons
Physical Skills
Technical Skills
Scientific Skills

- handguns (Ranged Weapons)
- rifles (Ranged Weapons)
- heavy weapons (Ranged Weapons)
- energy weapons (Ranged Weapons)

- dodging (Physical skills)
- throwing (Physical skills)
- melee weapons (Physical skills)
- unarmed (Physical skills)
- stealth (Physical skills)
- armor (Physical skills)

Technical Skills
- demolition
- mechanics
- first aid (!)
- repair

- programming (Scientific Skills/Technical Skills)

Scientific
- biology (Scientific Skills)
- chemistry (Scientific Skills)
- alien science (Scientific Skills)
- critical hits (Weapon+Physical skills+Biology)

*/

enum ESkills {
	SKILL_SMALL_GUNS,
	SKILL_BIG_GUNS,
	SKILL_ENERGY_WEAPONS,
	SKILL_UNARMED,
	SKILL_MELEE_WEAPONS,
	SKILL_THROWING,
	SKILL_MEDICINE,
	SKILL_WEAPON_KNOWLEDGE,
	SKILL_ALIEN_SCIENCE,
	SKILL_DODGING,
	SKILL_MECHANIC,
	SKILL_PROGRAMMING,
	NUMBER_OF_SKILLS
};

enum EResists {
		RESIST_RADIOACTIVE,
		RESIST_CHEM_POISON,
		RESIST_STUN,
		RESIST_PARALYZE,
		RESIST_FIRE,
		RESIST_ELECTRICITY,
		RESIST_BLIND,
		NUMBER_OF_RESISTS
};


#define STATE_RADIOACTIVE   0
#define STATE_CHEM_POISON   1
#define STATE_STUN          2
#define STATE_SPEED			3
#define STATE_TEMPERATURE   4
#define STATE_ELECTRICITY   5
#define STATE_BLIND         6
#define STATE_CONFUSE       7
#define STATE_STRENGTH		8

#define STATE_INVISIBLE		8
#define NUMBER_OF_STATES 9


enum EAttribType {
	ATTRIB_TYPE_HP,
	ATTRIB_TYPE_EP,
	ATTRIB_TYPE_STR,
    ATTRIB_TYPE_DEX,
    ATTRIB_TYPE_END,
    ATTRIB_TYPE_INT,
    ATTRIB_TYPE_SPD,
    ATTRIB_TYPE_MET,
    ATTRIB_TYPE_FOV
};

class CAttribute : CToSave {
public:
       int val;
       int max;
       int mod;
	   CMonster *owner;
	   int type;
	   int GetValue();
	   void ChangeOwner(CMonster *_owner) { owner = _owner; };
	   
       CAttribute(CMonster *_owner, int _type) : owner(_owner), type(_type)
	   { val=0; max=0; mod=0; ClassName="ATTRIBUTE"; };

   /// INHERITED FROM ToSave
	virtual unsigned long SaveObject();
	virtual bool LoadObject();
};

#endif

