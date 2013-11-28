// ============================================================================
// 	MSHLanguageFile	-	Zeta locale compatible Language File object
//
//	(c) 2004 Written by Mark Hogben (leonifan@dsl.pipex.com)
//
//	Represents a language file on disk, for use by the MSHLanguageMgr.
//
// ============================================================================

#ifndef _MSH_Language_File_
#define _MSH_Language_File_

#include <File.h>
#include <List.h>
#include <String.h>

class MSHLanguageFile {
public:
													MSHLanguageFile(const char* pathToLangFile);
														// Constructor. Pass in:
														//	pathToLangFile	- Complete path to the language file.

													~MSHLanguageFile();
														// Destructor. Like my commenting yet?
														// Destroys lists, reclaims memory, etc.

	bool										GetStringForTag(const char* stringTag, BString& stringFound);
														// Obtains a string translation, given a string tag to
														// look up in the translation file.
														// Pass in:
														//	stringTag		- [in] The tag stub to look up.
														//	stringFound	- [out] This will contain the found string on exit.
														// Returns 'true' only if the requested string is successfully found.

	const BString&					GetTranslationFileName() const;
														// Obtain the leaf name of the translation file on disk (no path).

private:
													MSHLanguageFile(const MSHLanguageFile& mgr);
														// Copy constructor - intentionally not implemented and private.

	MSHLanguageFile&				operator=(const MSHLanguageFile& mgr);
	MSHLanguageFile&				operator=(const MSHLanguageFile& mgr) const;
														// Assignment operator - intentionally not implemented and private.
														// Note: both const and non-const versions are present, or else
														// compiler might create default versions of one of them.

	void										ParseLanguageFileAndAddStrings(BFile& translationFile);
														// CALLED WITHIN CONSTRUCTOR - Helper function.
														// Moves through provided (and open) file, adding strings
														// to the tag and string lists as it goes.
														// Individual translations must be separated by newlines.
														// Line format is:
														//	# 	- anywhere in line discards the rest of that line.
														//	"		- First and second quotes on a line denote a tag.
														//				Third and fourth quotes on a line denote the string for a tag.

	BList*									fStringTagList;
														// List of string tags stored in the translation file.

	BList*									fStringTranslations;
														// List of translated strings for the equivalent (index-matched) tags.

	BString									fTranslationFileName;
														// When created, the translation file object stores its file name here.

	BString									fTranslationFileFullPath;
														// Full path and name to translation file.
};

#endif
// _MSH_Language_File_
