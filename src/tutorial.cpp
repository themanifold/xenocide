#include "tutorial.h"
#include "global.h"
#include "level.h"

extern CMyScreen screen;
extern CLevel level;

void CTutorial::show_next()
{
	if (index<tutorial_texts.size())
	{
		screen.console.zero();
		screen.console.add(tutorial_texts[index],15);
		index++;
	}
	else
		level.player->death();
}

CTutorial::CTutorial()
{
	index=0;
	tutorial_texts.push_back("Welcome to Xenocide: The roguelike.\nThis tutorial will introduce the game and teach you how to play.\nWhat you see now is the main screen. Most of the time you will see it. Your character is represented by yellow '@' sign on the map. Walk a bit with the numpad keys (1-9), with num-lock on. Press Space or Enter to continue...\n\n");
	tutorial_texts.push_back("At the bottom of the screen you see basic information about your character. Some stats (like strength or speed) as well as your weapon and armor (bare hands and skin for now). HP to the left are your 'hit points', that describe your health level. Press the 'M' (shift+'m') to see the message history and '?' (shift + /) for list of available commands. Press Space or Enter...\n\n");
	tutorial_texts.push_back("During your game you will find a lot of useful items, that help you to survive in hostile environment. There is an Axe in your backpack. Press 'i' key to look into your backpack, then choose the letter by the item. Context menu with all available options will show and choose 'w' to wield this weapon.\n\n\n");
	tutorial_texts.push_back("To look around press 'l' key. The cursor with change and you will be able to examine environment without moving. This is especially useful when you meet a new monster. Look at the item to your right: there is a Trooper Armor. You can also print seen objects by pressing 'L' (shift+'l)'\n\n\n");
	tutorial_texts.push_back("Get the Trooper Armor by pressing 'g' key, then put it on by the context menu in your inventory ('i').\n\n\n\n");
	tutorial_texts.push_back("You found the closed door. To open it simply move into them, or press 'o' (open/operate) key staying near by. Remember, you can press '?' to get the list of all available commands.\n\n\n\n");
	tutorial_texts.push_back("Through the window in these door you see a scary monster! Look at it with the 'l' key. To attack the monster just walk into it. Open the door now and kill it with your axe!\n\n\n\n");
	tutorial_texts.push_back("That was easy... :) In real game monsters are much harder to beat... In this room you will find ranged weapon and ammo. Pick them up ('g') and wield in inventory.\n\n\n\n");
	tutorial_texts.push_back("To fire weapon press the 'f' key. This fires a missile into selected enemy. To choose the next target press 'n' key. To reload weapon press 'r' key or reload it by context menu in inventory.\n\n\n\n");
	tutorial_texts.push_back("Some weapons, like LR-30 Assault Rifle, have different fire modes. To change them press '[' and ']' keys. Other modes fire more ammo in one shoot. It is very useful against tough monsters, but consumes more ammo.\n\n\n");
	tutorial_texts.push_back("Sometimes the way is blocked by the terrain. Here are some boxes that can be destroyed to pass. Attack them with your axe with 'A' (shift+'a') key, or destroy them firing 'f' your rifle. It is useful to Ready (in inventory) different type of weapon than you wield. You can instantly exchange ready and wield weapons with the 'e' key.\n\n");
	tutorial_texts.push_back("Well well! Grenades lay here! :) Get them and look at your inventory. You can activate the grenade in the contex menu, then throw it by the 't' key or by option in the context menu. Throw it far away not to get hurt.\n\n\n");
	tutorial_texts.push_back("If your health is low, rest 'R' (shift+'r'), wait a bit '5', or eat the healing pill. There are different types of pills in the game. These will heal you if you are wounded. Use them by the context menu in the inventory. Resting is very important, rest whenever it is safe.\n\n");
	tutorial_texts.push_back("In Xenocide you can build your own robots. To build an robot take the robot parts and choose the proper option in context menu of a robot shell. You can also turn existing robots off by pressing 'o' key. Turn this robot off, then build a new one with your CPU and Axe in action slot.\n\n");
	tutorial_texts.push_back("There are some enemies behind these doors. Let your robot kill them.\n\n\n\n\n\n");
	tutorial_texts.push_back("As traveling can be boring, you can use the explore command 'E' (shift + 'e') and travel command 'X' (shift + 'x'), to explore new places or travel to known. It is a good way of moving, because these actions stop at interesting events.\n\n\n");
	tutorial_texts.push_back("Some items in Xenocide require power, some items gain new features when powered. Pick up these items, wear armor and weapon. You can activate them by context menu or by pressing \";\" (semicolon) and \"'\" (apostrophe) to handle weapon and armor on you. Look how activated items change your stats.\n\n\n");
	tutorial_texts.push_back("All the things you do will teach your character a new skills. You can only learn when you have unallocated experience points. These points you get by killing monsters. Experience is also required to advance into a new level, which gives you some other bonuses. To look at your character stats press '@' (shift + '2').\n\n\n");
	tutorial_texts.push_back("NOT IMPLEMENTED YET! In your adventure through alien planet you will find many alien devices. These devices are powerful, but the knowledge (Alien Science skill) is required to use them. To learn this skill use more devices, examine them and read proper memory cards.\n\n\n");
	tutorial_texts.push_back("Staying on stairs or elevator press '<' or '>' to travel to the next level.\n\nThis is the end of tutorial. Hope you learned the basics. Refer the manual, faq and readme.txt for further help. The tutorial will now end... Good luck!\n\n\n");
}