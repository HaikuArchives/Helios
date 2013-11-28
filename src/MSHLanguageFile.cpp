// ============================================================================
// 	MSHLanguageFile	-	Zeta locale compatible Language File object
//
//	(c) 2004 Written by Mark Hogben (leonifan@dsl.pipex.com)
//
//	Represents a language file on disk, for use by the MSHLanguageMgr.
//
// ============================================================================

#include "MSHLanguageFile.h"

#include <Entry.h>
#include <stdio.h>

static const char kCommentChar = '#';

MSHLanguageFile::MSHLanguageFile(const char* pathToLangFile) :
	fStringTagList(NULL),
	fStringTranslations(NULL),
	fTranslationFileName(""),
	fTranslationFileFullPath(pathToLangFile)
{
	fStringTagList = new BList();
	fStringTranslations = new BList();
	
	if ((fStringTagList != NULL) && (fStringTranslations != NULL )) {
		// Load the file and parse the strings.
		// Then store them in the lists (tag and translation separately).
//		BFile translationFile(pathToLangFile, B_READ_ONLY);
//		ParseLanguageFileAndAddStrings(translationFile);
//		translationFile.Unset();

		BEntry transEntry(pathToLangFile);
		if (B_OK == transEntry.InitCheck()) {
			char buffer[B_FILE_NAME_LENGTH];
			if (B_OK == transEntry.GetName(buffer)) {
				fTranslationFileName = buffer;
			}
		}
		transEntry.Unset();
	}
}

MSHLanguageFile::~MSHLanguageFile()
{
	// Delete each individual string (both lists).
	const int32 numItems = fStringTagList->CountItems();
	for (int32 itemIndex = 0; itemIndex < numItems; itemIndex++) {
		BString* stringTag = reinterpret_cast<BString*>(fStringTagList->ItemAt(itemIndex));
		BString* stringTrans = reinterpret_cast<BString*>(fStringTranslations->ItemAt(itemIndex));
		delete stringTag;
		delete stringTrans;
	}

	delete fStringTagList;
	delete fStringTranslations;
}

bool
MSHLanguageFile::GetStringForTag(const char* stringTag, BString& stringFound)
{
	bool foundString = false;

	// Bad situation if the following test fails. No memory?
	// If yes, then don't allow strings to be obtained.
	// Not the best solution - but without exceptions, or 2-phase construction,
	// this is the easiest way to deal with it.
	if ((fStringTagList != NULL) && (fStringTranslations != NULL )) {
	
		if (fStringTagList->CountItems() == 0) {
			BFile translationFile(fTranslationFileFullPath.String(), B_READ_ONLY);
			ParseLanguageFileAndAddStrings(translationFile);
			translationFile.Unset();
		}

		const int32 numTags = fStringTagList->CountItems();
		const int32 numStrings = fStringTranslations->CountItems();
		
		// Sanity check!
		if (numTags == numStrings) {

			const BString tagToFind(stringTag);
			for (int32 stringIndex = 0; stringIndex < numTags; stringIndex++) {
				BString* currentTag = reinterpret_cast<BString*>(fStringTagList->ItemAt(stringIndex));
				if (NULL != currentTag) {
					if (tagToFind == *currentTag) {
						// Found the tagged entry. Now get the translation.
						BString* translationString = reinterpret_cast<BString*>(fStringTranslations->ItemAt(stringIndex));
						if (NULL != translationString) {
							stringFound = *translationString;
							foundString = true;
						} else {
							// Madness - should never happen. But then I've been a programmer long
							// enough to know it does. Especially on big projects. Zzzzzzz...
							break;
						}
					}
				} else {
					// This should never happen - a NULL entry. So give up.
					break;
				}
			}

		}

	}
	
	return foundString;
}

void
MSHLanguageFile::ParseLanguageFileAndAddStrings(BFile& translationFile)
{
	// Some sanity checking first.
	if (B_OK != translationFile.InitCheck()) {
		return;
	}

	off_t fileSize;
	if (B_OK != translationFile.GetSize(&fileSize)) {
		return;
	}

	// I decided not to buffer this - OS should do it, and allocating buffers
	// would have either made things messy or used lots of memory (depending how I did it).
	// Since this should only be a once-off action at the start of the app, who cares?

	// Find beginning of file. Really just here for an extra sanity check, or empty files.
	if (B_OK == translationFile.Seek(0, SEEK_SET)) {
		BString currentTag = "";
		BString currentTranslation = "";

		bool inComment = false;
		bool inQuotes = false;
		bool doneTag = false;
		bool doneTranslation = false;

		char readChar = '\0';
		
		for (off_t fileIndex = 0; fileIndex < fileSize; fileIndex++) {
			// Read one character at a time.
			translationFile.ReadAt(fileIndex, &readChar, 1);

			// Comments.
			if (readChar == kCommentChar) {
				// Discard all until the end of the line.
				inComment = true;
			}

			// End of the line - or end of file.
			if ((readChar == '\n') || (fileIndex == (fileSize-1))) {

				// Finish off translation for this line.
				if (doneTag) {
					BString* newTag = new BString(currentTag);
					fStringTagList->AddItem(reinterpret_cast<void *>(newTag));
					
					// Must always add an equivalent translation string (even if none was set)
					// to keep lists at equal lengths.
					BString* newTranslation = new BString(currentTranslation);
					fStringTranslations->AddItem(reinterpret_cast<void *>(newTranslation));
				}

				// Zap the strings and flags for the next line.
				currentTag = currentTranslation = "";
				inComment = false;
				inQuotes = false;
				doneTag = false;
				doneTranslation = false;

			} else {
			
				// Ignore everything on the line if we are in a comment.
				if (!inComment) {

					// Main parsing really happens here.

					// Quote detected?
					if (readChar == '\"') {

						if (!inQuotes) {
							inQuotes = true;
							continue;
								// Back round the file reading loop.
						} else {
							// Still within quotes until this point - so we have found a closing quote.
							inQuotes = false;

							if (!doneTag) {
								// Still processing the tag string - so end it now.
								doneTag = true;
							} else if (!doneTranslation) {
								// Still processing the translation string - so end it now.
								doneTranslation = true;
							} else {
								// Done both strings, just parsing quoted garbage until the end of the line... ;-)
							}
						}

					}	// Quote detected?

					// If it gets here without being in quotes, the character will be discarded.
					if (inQuotes) {

						if (!doneTag) {
							// Started the tag string.
							currentTag += readChar;
						} else if (!doneTranslation) {
							// Started the actual string.
							currentTranslation += readChar;
						} else {
							// Done both strings, just parsing garbage until the end of the line...
						}

					} // inQuotes == true
					
				}

			}
				
		}
	}
}

const BString&
MSHLanguageFile::GetTranslationFileName() const
{
	return fTranslationFileName;
}
