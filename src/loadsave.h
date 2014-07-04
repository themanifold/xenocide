#ifndef LOADSAVE_H
#define LOADSAVE_H

//#pragma warning (disable: 4786)
//#pragma warning (disable: 4788)
// disable warning (compare bool with int)
//#pragma warning (disable: 4805)
//#pragma warning (disable: 4503)

#include <stdio.h>
#include <list>
#include <map>
#include <string>
#include "position.h"
using namespace std;

class CToSave;

typedef list<void *> ptr_list;

typedef pair<CToSave **,unsigned long>  para;
typedef list < para > to_fix;
typedef list <CToSave *> list_of_saved;

typedef map <unsigned long, CToSave *> addresses;

#define NULL_UNIQUE_NUMBER		0
#define PLAYER_UNIQUE_NUMBER	5
#define LEVEL_UNIQUE_NUMBER		6
#define MAP_UNIQUE_NUMBER		7
#define FIRST_UNIQUE_NUMBER		100

class CHero;

class CToSave {
public:
		bool IsAlreadySaved; // error control - to avoid double save of object

		string ClassName;
		unsigned long UniqueNumber;

		void operator=(CToSave &) {}; // TOSAVE data are unique

		virtual unsigned long SaveObject();
		virtual bool LoadObject();
		virtual void UpdatePointers();

		static CHero *LoadPlayer(string name);
		static bool LoadOptions(string file_name);
		static bool SaveOptions(string file_name);

		static bool LoadMessageBuffer(string file_name);
		static bool SaveMessageBuffer(string file_name);		
		
		static void SaveNULLObject();
		static void SaveString(string value);
		static void SaveChar(char value);
		static void SaveInt(int value);
		static void SaveBool(bool value);		
		static void SaveLong(unsigned long value);
		static void SaveUniqueNumber(unsigned long value); 
		static void SaveList(ptr_list &value);
		static void SaveListOfPointers(ptr_list &value);

		static void LoadString(string &value);
		static void LoadChar(char &value);
		static void LoadInt(int &value);
		static void LoadBool(bool &value);		
		static void LoadLong(unsigned long &value);
		static void LoadUniqueNumber(CToSave **ptr); 
		static void LoadList(ptr_list &value);
		static void LoadListOfPointers(ptr_list &value);
		static void OpenLogFile();
		static void CloseLogFile();
		static void LogLevelChange(string to_log);

		static void FinishLoadingGame();

		static to_fix ListToFix;
		static addresses AddressesOfObjects;
		static CToSave *CreateObject();

		static list_of_saved saved;

		CToSave();
		virtual ~CToSave() {};
		//// shared

		static unsigned long LastUniqueNumber;
		static FILE *FilePointer;
		static FILE *loadlog;
		static void InitFilePointer(FILE *fp);

		void SaveLastUNToFile();
		void LoadLastUNFromFile();
};

#endif
