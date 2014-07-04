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

extern CGame game;
extern CKeyboard keyboard;
extern CLevel level;
extern CMyScreen screen;
extern DEFINITIONS definitions;
extern class COptions options;

void CPlaces :: print_on_terminal(int x, int y, string text)
{
	for (size_t a=0;a<text.size();a++)
	{
		char character = text[a];
		if (character!=' ')
			set_color(10);
		else
		{
			set_color(2);
			character='.';
		}
		print_character(x+4+a,y+4,character);
	}
}

void CPlaces :: draw_terminal()
{
	screen.clear_all();
	int x,y;
	int sizex=57,sizey=21;
	for (x=2;x<=sizex;x++)
	{
		for (y=2;y<=sizey;y++)
		{
			if (x==2 || x==sizex || y==2 || y==sizey)
				set_color(2);
			else
				set_color(2);
			
			print_character(x,y,'.');
		}
	}
	
	set_color(8);
	for (x=3;x<=sizex+1;x++)
		print_character(x,sizey+1,'.');
	for (y=3;y<=sizey+1;y++)
		print_character(sizex+1,y,'.');	

}
void CPlaces :: print_laboratory()
{
	draw_terminal();
	
	set_color(10);
				
	int px,py;
	px = 0;
	py = 0;
	string text;
	print_on_terminal(px,py++,"Simple Holo Terminal ver.0.7a");	
	py++;
	switch (random(5))
	{
	case 0: text="Good day, Professor.";
		break;
	case 1: text="Welcome back, Professor.";
		break;
	case 2: text="I haven't seen you for a long time, Professor.";
		break;
	case 3: text="Hello again, Professor.";
		break;
	case 4: text="I'm always ready for your command, Professor.";
		break;
	}
	print_on_terminal(px,py++,text);
	switch (random(5))
	{
	case 0: text="What is your command?";
		break;
	case 1: text="Which program should I run?";
		break;
	case 2: text="Available projects:";
		break;
	case 3: text="Please, select one:";
		break;
	case 4: text="What do you want me to open?";
		break;
	}
	print_on_terminal(px,py++,text);
	py++;
	print_on_terminal(px,py++,">1.Bio Rebuilding 7");
	print_on_terminal(px,py++,">2.DNA Inst Modification");
	print_on_terminal(px,py++,">3.DI 227BS - DNA Integration");
	print_on_terminal(px,py++,">4.Personal log");

	myrefresh();
}
	
bool CPlaces::GeneticLaboratory()
{
	taken_DNA = "";
	print_laboratory();
	while(1)
	{
		int key = keyboard.wait_for_key();
		if (key == keyboard.escape || key == keyboard.readmore || key == keyboard.readmore2)
		{
			break;
		}		
		else if (key == '1')
		{
			if (GenLabBioRebuild()==true)
				return true;
			print_laboratory();
		}
		else if (key == '2')
		{
			if (GenLabDNAModif()==true)
				return true;
			print_laboratory();
		}
		else if (key == '3')
		{
			if (GenLabIntegration()==true)
				return true;
			print_laboratory();
		}
		else if (key == '4')
		{
			GenLabPersonalLog();
			print_laboratory();
		}
	}
	return false;
}

void CPlaces::print_program_1()
{
	int px,py;
	px=0,py=0;

	draw_terminal();
	print_on_terminal(px,py++,"-= Bio Rebuilding Program =- ver. 7.12.24");
	py++;
	print_on_terminal(px,py++,"Last modified: 2081-02-05");
	py++;
	print_on_terminal(px,py++,"Self test... CRC OK.");
	py++;
	print_on_terminal(px,py++,"Testing hardware:");
	py++;
	print_on_terminal(px,py++,"Testing bio generator... OK.");
	print_on_terminal(px,py++,"Testing DNA taker... OK.");
	print_on_terminal(px,py++,"Hardware test completed.");
	py++;
	py++;
	print_on_terminal(px,py++,"User options:");
	print_on_terminal(px,py++,">1.Rebuild creature using its DNA.");
	
	myrefresh();
}

void CPlaces::print_program_2()
{
	int px,py;
	px=0,py=0;

	draw_terminal();
	print_on_terminal(px,py++,"Error: No configuration files.");
	print_on_terminal(px,py++,"Upload new configuration files to run.");
	myrefresh();
}

void CPlaces::print_program_3()
{
	int px,py;
	px=0,py=0;
	
	draw_terminal();
	print_on_terminal(px,py++,"Experiment DI 227BS");
	print_on_terminal(px,py++,"DNA Integration");
	py++;
	if (taken_DNA=="")
		print_on_terminal(px,py++,"No DNA sample for experiment.");
	else
		print_on_terminal(px,py++,"DNA taken from " + taken_DNA);
	py++;
	print_on_terminal(px,py++,">1.Choose a sample to extract DNA.");
	if (taken_DNA=="")
		print_on_terminal(px,py++," 2.Integrate living creature with taken DNA.");
	else
		print_on_terminal(px,py++,">2.Integrate living creature with taken DNA.");

	myrefresh();
}

void CPlaces::print_program_4(int page)
{
	int px,py;
	px=0,py=0;

	draw_terminal();

	switch(page) {
	case 1:
		print_on_terminal(px,py++,"[ED 06-12-2189]");
		print_on_terminal(px,py++,"Success, finally! The GenMachine works fine,");
		print_on_terminal(px,py++,"and the DNA integration went as expected.");
		print_on_terminal(px,py++,"Now I wait for the mega-freighter to arrive.");
		print_on_terminal(px,py++,"What was it called? The \"Atlas\" was it?");
		print_on_terminal(px,py++,"It doesn't matter, so long as it gets here soon.");
		print_on_terminal(px,py++,"I hope it gets here soon.");
		print_on_terminal(px,17,">2. Next page");
		break;
	case 2:
		print_on_terminal(px,py++,"[ED 08-12-2189] (1/2)");
		print_on_terminal(px,py++,"I wonder how can GenTech can demand results, when");
		print_on_terminal(px,py++,"all I have to work with are the same old samples!");
		print_on_terminal(px,py++,"I hope the freighter arrives soon.");
		 
		print_on_terminal(px,py++,"It is smuggling in a cache of alien life forms");
		print_on_terminal(px,py++,"for me to experiment on, under the guise of picking");
		print_on_terminal(px,py++,"up raw ore for processing.");
		print_on_terminal(px,py++,"Then I will have new DNA to experiment with!");
		print_on_terminal(px,py++,"Then this project will finally begin to move forward!");
		print_on_terminal(px,16,">1. Prev page");
		print_on_terminal(px,17,">2. Next page");
		break;
	case 3:
		print_on_terminal(px,py++,"[ED 06-12-2189] (2/2)");		
		print_on_terminal(px,py++,"I grow restless in waiting. Maybe I should ask one");
		print_on_terminal(px,py++,"of these stupid miners for help in the lab? No, they");
		print_on_terminal(px,py++,"won't ever understand how important my work is.");
		print_on_terminal(px,py++,"They're all narrow minded idiots! And these strange");
		print_on_terminal(px,py++,"deaths recently. They blame my creations for the");
		print_on_terminal(px,py++,"killings, and no matter how much I tell them that my");
		print_on_terminal(px,py++,"creatures are locked away and cannot harm them,");
		print_on_terminal(px,py++,"I cannot seem to convince them otherwise.");
		print_on_terminal(px,py++,"They are too foolish to see that there can be only");
		print_on_terminal(px,py++,"one possible explanation for these murders. ");
		print_on_terminal(px,py++,"There must by a mad man amongst them, a serial killer");
		print_on_terminal(px,py++,"with a lust for blood. Like a Leatherface, yes.");
		print_on_terminal(px,16,">1. Prev page");
		break;
	default:;
	}	
	myrefresh();
}


bool CPlaces::GenLabBioRebuild()
{
	print_program_1();
	while(1)
	{
		int key = keyboard.wait_for_key();
		if (key == keyboard.escape || key == keyboard.readmore || key == keyboard.readmore2)
		{
			break;
		}		
		else if (key == '1')
		{
			level.player->hit_points.val = level.player->hit_points.max;

			for (int a=0;a<NUMBER_OF_STATES;a++)
				level.player->state[a]=0;

			screen.console.add("You lay down on table. Mechanic arm with sharp needle gave you long dream...",7);
			screen.console.add("You have nightmares... After unknown period of time you wake up. You feel very refreshed.",7);

			level.player->wait(random(200)+100);				

			level.is_player_on_level = false;
			level.monsters.remove(level.player);
			level.map.setBlockMove(level.player->pX(),level.player->pY());
			
			return true;
		}
	}		
	return false;
}

bool CPlaces::GenLabDNAModif()
{
	print_program_2();
	while(1)
	{
		int key = keyboard.wait_for_key();
		if (key == keyboard.escape || key == keyboard.readmore || key == keyboard.readmore2)
		{
			break;
		}		
	}		
	return false;
}


bool CPlaces::GenLabIntegration()
{
	int character;
	CItem *temp;
	CCorpse *cialo;
	CMonster *monster;
	string text;
	int zwracana;

	print_program_3();
	while(1)
	{
		int key = keyboard.wait_for_key();

		if (key == keyboard.escape || key == keyboard.readmore || key == keyboard.readmore2)
		{
			break;
		}		
		else if (key == '1')
		{
			level.player->show_backpack(NULL,INV_SHOW_HIGHLIGHT_CORPSES);
            set_color(10);
			text = " From which corpse DNA shall be extracted? ";
			level.player->adaptation_points = 6;
            print_text(40-text.size()/2,0,text);
			myrefresh();
            while (1)
            {
				character=keyboard.wait_for_key();
				if ((character>='a' && character<='z') || (character>='A' && character<='Z'))
				{
					temp=level.player->choose_item_from_backpack(character);
					if (temp!=NULL) // gdy cos wybrano
					{
						cialo = temp->IsCorpse();
						if (cialo!=NULL)
						{
							taken_DNA = cialo->of_what;
							cialo->death();
							print_program_3();
							break;							
						}
					}
				}
				else if (character==keyboard.escape || character==keyboard.readmore || character==keyboard.readmore2)
				{
					print_program_3();
					break;
				}				
			}
		}		
		else if (key == '2')
		{
			if (taken_DNA!="")
			{
				monster = definitions.find_monster(taken_DNA);

				draw_terminal();
				int px=0,py=0;
				print_on_terminal(px,py++,"Profesor!");
				print_on_terminal(px,py++,"The only living creature here is you!");
				print_on_terminal(px,py++,"Do you really want to integrate yourself?");
				py++;
				print_on_terminal(px,py++,"(y/n)");
				py++;
				print_on_terminal(px,py++,"(I hope you remember how dangerous it is?)");
				myrefresh();
				zwracana=keyboard.wait_for_key();
				if (zwracana!='y' && zwracana!='Y')
				{
					print_program_3();
					break;							
				}
				print_on_terminal(6,4,"Y");
				myrefresh();
				if (level.player->adaptation_points<1)
				{
					print_on_terminal(px,py++,"Your organism can't adaptate this DNA.");
					myrefresh();
					keyboard.wait_for_key();
					print_program_3();
					break;							
				}
				print_on_terminal(px,py++,"Let's start...");
				delay(150);
				myrefresh();
				
				if (IntegratePlayerWith(monster)==false)
					return false;

				level.player->wait(random(500)+500);				
				level.is_player_on_level = false;
				level.monsters.remove(level.player);
				level.map.setBlockMove(level.player->pX(),level.player->pY());				
				return true;
			}
		}		
	}
	return false;
}


#define LOG_PAGES 3

bool CPlaces::GenLabPersonalLog()
{
	int page = 1;
	print_program_4(page);
	while(1)
	{
		int key = keyboard.wait_for_key();
		if (key == '1' && page>1)
			page--;
		else if (key == '2' && page<LOG_PAGES)
			page++;		
		else if (key == keyboard.escape || key == keyboard.readmore || key == keyboard.readmore2)
		{
			break;
		}	
		print_program_4(page);		
	}		
	return false;
}

bool CPlaces::IntegratePlayerWith(CMonster *monster)
{
	if (monster==NULL)
		return false;
		
	level.player->adaptation_points--;

	screen.console.add("You hear \"Experiment: DNA integration started.\".\n",11);		
	screen.console.add("You've been putted asleep...\n",7);		
	screen.console.add("... ... ... ...\n",7);		
	screen.console.add("After unknown period of time you wake up.\n",7);		
	
	int random_value;

	int new_value;
	// power	

	if (level.player->strength.val < monster->strength.val)
		screen.console.add("You feel stronger...",7);
	else if (level.player->strength.val > monster->strength.val)
		screen.console.add("You feel not as strong as before...",7);
	
	new_value = (level.player->strength.max + monster->strength.max)/2;
	level.player->strength.val += new_value - level.player->strength.max;
	level.player->strength.max = new_value;
	
	// zrecznosc
	if (level.player->dexterity.val < monster->dexterity.val)
		screen.console.add("You feel more flexible...",7);
	else if (level.player->dexterity.val > monster->dexterity.val)
		screen.console.add("Your moves are less coordinated...",7);

	new_value = (level.player->dexterity.max + monster->dexterity.max)/2;
	level.player->dexterity.val += new_value - level.player->dexterity.max;
	level.player->dexterity.max = new_value;
	
	// wytrzymalosc
	if (level.player->endurance.val < monster->endurance.val)
		screen.console.add("You feel tougher...",7);
	else if (level.player->endurance.val > monster->endurance.val)
		screen.console.add("Your feel weaker...",7);

	new_value = (level.player->endurance.max + monster->endurance.max)/2;
	level.player->endurance.val += new_value - level.player->endurance.max;
	level.player->endurance.max = new_value;
	
	// inteligencja
	if (level.player->intelligence.val < monster->intelligence.val)
		screen.console.add("Your thoughts are bright and clear...",7);
	else if (level.player->intelligence.val > monster->intelligence.val)
		screen.console.add("You can't concentrate as before...",7);

	new_value = (level.player->intelligence.max + monster->intelligence.max)/2;
	level.player->intelligence.val += new_value - level.player->intelligence.max;
	level.player->intelligence.max = new_value;
	
	// szybkoœæ
	if (level.player->speed.val < monster->speed.val)
		screen.console.add("Your moves are faster...",7);
	else if (level.player->intelligence.val > monster->intelligence.val)
		screen.console.add("Your moves are slower...",7);
	
	new_value = (level.player->speed.max + monster->speed.max)/2;
	level.player->speed.val += new_value - level.player->speed.max;
	level.player->speed.max = new_value;
	
	// hit points

	if (level.player->hit_points.max < monster->hit_points.max)
		screen.console.add("You feel more health...",7);
	else if (level.player->hit_points.max > monster->hit_points.max)
		screen.console.add("You feel less health.",7);
	
	new_value = (level.player->hit_points.max + monster->hit_points.max)/2;
	level.player->hit_points.val += new_value - level.player->hit_points.max;
	level.player->hit_points.max = new_value;

	if (level.player->hit_points.val<=0)
		level.player->hit_points.val=1;
	
	// metabolizm
	random_value = level.player->metabolism.val - monster->metabolism.val;
	
	if (random_value>0) // player szybszy
	{
		if (random_value>4)
			random_value=4;
		
		random_value = random(random_value-1)+1;
		level.player->metabolism.val-=random_value;
		level.player->metabolism.val-=random_value;
	}
	else if (random_value<0) // player wolniejszy
	{
		if (random_value<4)
			random_value=-4;
		
		random_value = random(random_value-1)+1;
		level.player->metabolism.val+=random_value;
		level.player->metabolism.val+=random_value;
	}
	// skora
	bool change = false;
	if (level.player->no_armor.name != monster->no_armor.name)
	{
		if (level.player->no_armor.ARM < monster->no_armor.ARM)
		{
			level.player->no_armor.ARM++;
			change = true;
		}
		else if (level.player->no_armor.ARM > monster->no_armor.ARM)
		{
			level.player->no_armor.ARM--;
			change = true;
		}
		else // ARM jest takie samo, nabiera properties
		{
			for (PROPERTIES a=1;a!=TYPE_LAST_UNUSED;a+=a)
			{
				if (monster->no_armor.properties&a) // gdy ma ceche
				{
					if (!level.player->no_armor.properties&a) // gdy cechy nie ma
						if (random(3)==0)
						{
							level.player->no_armor.properties^=a; // ustawiamy bit
							change = true;						
							break;
						}
				}
				else if (!monster->no_armor.properties&a) // gdy nie ma cechy
				{
					if (level.player->no_armor.properties&a) // gdy ma ceche
						if (random(3)==0)
						{
							level.player->no_armor.properties^=a; // zerujemy w ten sposob dany bit
							change = true;						
							break;
						}
				}
			}			
		}
		if (monster->no_armor.ARM == level.player->no_armor.ARM &&
			monster->no_armor.properties == level.player->no_armor.properties)
		{
			screen.console.add(string("Your ") + level.player->no_armor.name + " transformed into " + monster->no_armor.name + "!.",7);			
			level.player->no_armor.name = monster->no_armor.name;
			level.player->no_armor.color = monster->no_armor.color;
		}
		else if (change)
			screen.console.add(string("Your ") + level.player->no_armor.name + " is transforming into " + monster->no_armor.name + ".",7);

	}
	// gole rece
	change = false;
	if (level.player->unarmed.name != monster->unarmed.name)
	{
		if (level.player->unarmed.DEF < monster->unarmed.DEF)
		{
			level.player->unarmed.DEF++;
			change = true;
		}
		else if (level.player->unarmed.DEF > monster->unarmed.DEF)
		{
			level.player->unarmed.DEF--;
			change = true;
		}

		if (level.player->unarmed.HIT < monster->unarmed.HIT)
		{
			level.player->unarmed.HIT++;
			change = true;
		}
		else if (level.player->unarmed.HIT > monster->unarmed.HIT)
		{
			level.player->unarmed.HIT--;
			change = true;
		}

		if (level.player->unarmed.DMG < monster->unarmed.DMG)
		{
			level.player->unarmed.DMG++;
			change = true;
		}
		else if (level.player->unarmed.DMG > monster->unarmed.DMG)
		{
			level.player->unarmed.DMG--;
			change = true;
		}
		
		for (PROPERTIES a=1;a<=TYPE_LAST_UNUSED;a+=a)
		{
			if (monster->unarmed.properties&a) // gdy ma ceche
			{
				if ((level.player->unarmed.properties&a)==0) // gdy cechy nie ma
				{
					change = true;						
					if (random(5)==0)
					{
						level.player->unarmed.properties^=a; // ustawiamy bit
						break;
					}
				}
			}
			else if (!monster->unarmed.properties&a) // gdy nie ma cechy
			{
				if ((level.player->unarmed.properties&a)==0) // gdy cechy nie ma
				{
					change = true;						
					if (random(5)==0)
					{
						level.player->unarmed.properties^=a; // zerujemy w ten sposob dany bit
						break;
					}
				}
			}
		}
		if (change)
			screen.console.add(string("Your ") + level.player->unarmed.name + " is transforming...",7);		
		else
		{
			screen.console.add(string("Your ") + level.player->unarmed.name + " transformed into " + monster->unarmed.name + "!",7);		
			level.player->unarmed.name = monster->unarmed.name;
			level.player->unarmed.color = monster->unarmed.color;
		}
		
	}

	// resisty

	for (int b=0;b<NUMBER_OF_RESISTS;b++)
	{
		if (level.player->resist[b]<monster->resist[b])
			level.player->resist[b] += random(min(3,monster->resist[b] - level.player->resist[b]));
		else if (level.player->resist[b]>monster->resist[b])
			level.player->resist[b] -= random(min(3,level.player->resist[b] - monster->resist[b]));
	}
	
	level.player->set_weapon(&level.player->unarmed);
	level.player->set_armor(&level.player->no_armor);
	
	screen.console.add("\n You hear: \"DNA integration with this sample is completed.\".",11);			
	return true;
}

