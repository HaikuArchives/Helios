// ============================================================================
// 	MSHLanguageMgr	-	Zeta locale compatible Language Manager
//
//	(c) 2004 Written by Mark Hogben (leonifan@dsl.pipex.com)
//
//	The purpose of this manager is to allow a single app to be easily
//	translatable on both Zeta and R5.
//
//	If building for Zeta, then simply use be.locale.
//	If building for R5, then simply use this manager and the same
//	locale files will be used as if you had used Zeta's locale.
//
//	Note: You won't get Zeta's nice on-the-fly OS-wide locale changing,
//	but at least you'll get as close to it as you can without losing
//	translations.
//
// ============================================================================

#ifndef _MSH_Language_Mgr_
#define _MSH_Language_Mgr_

#include <List.h>

#include "MSHLanguageFile.h"

class MSHLanguageMgr
{
public:
	MSHLanguageMgr();
	// Constructor. Note this will leave the manager in
	// an empty state. You must call LoadLanguageFiles()
	// to actually install the language files.
	// This 2-phase construction allows for success
	// detection, without using exceptions.

	~MSHLanguageMgr();
	// Destructor. No, really.

	bool LoadLanguageFiles(const char* pathToFileStub);
	// Loads all translation files, given the passed-in
	// file stub path.
	// 'pathToFileStub' - path relative to the app,
	//										where all language files are found.
	//										This path will also contain
	//										the prefix for all these files.
	//										e.g. A prefix of "Lang" could be
	//										used for Lang.enUS, and Lang.deDE.
	// All files with names beginning with the given
	// prefix "stub" will be loaded in the given directory.
	// Returns 'true' only if at least one file was
	// successfully loaded.

	int32 CountLanguages() const;
	// Returns the number of language files the manager contains.

	void SelectLanguage(const int32 index);
	// Sets the current language to the file at position 'index'.

	void SelectLanguageByName(const BString& languageTag);
	// Sets the current language to that with the passed-in
	// matching language tag e.g. "enUS"

	BString GetLanguageName(const int32 index);
	// Returns the name of the language file at position 'index',
	// minus the prefix stub e.g. MyApp.enUS will return 'enUS'

	BString _T(const char* stringTag);
	// Translate function.
	// Matches the passed-in stringTag with the currently selected
	// translation file, and returns the associated translation string.
	// Note: If it does not find the string, it will return the original
	// but with with asterisks (*) added at the start and end, to
	// indicate an untranslated string.
private:
	MSHLanguageMgr(const MSHLanguageMgr& mgr);
	// Copy constructor - intentionally not implemented and private.

	MSHLanguageMgr& operator=(const MSHLanguageMgr& mgr);
	// Assignment operator - intentionally not implemented and private.

	void DeleteAllLanguageFilesAndList();
	// Helper function - deletes all the language files pointed to
	// by the internal list, fTransFiles, zaps the list
	// (and makes the pointer NULL), and also sets the current language
	// to none (-1) and the file name stub to an empty string.

	BList* fTransFiles;
	// Translation file list. Each entry is a pointer to a MSHLanguageFile
	// object, representing a language file (unsurprisingly).

	BString fFileNameStub;
	// When LoadLanguageFiles() is called, the prefix stub for all language
	// files is stored here.

	int32 fCurrentLanguage;
	// Index to the currently selected language.
	// Will be -1 if no valid language is selected.
};

#endif
// _MSH_Language_Mgr_
