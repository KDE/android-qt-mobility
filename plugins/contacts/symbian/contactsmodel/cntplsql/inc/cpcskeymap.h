/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: Class to hold the character to key mappings and provide
*   conversion of strings to their numerical mappings. 
*
*/

#ifndef __CPCSKEYMAP_H__
#define __CPCSKEYMAP_H__

// INCLUDES
#include <QLocale>
#include <QList>
#include <hbinputdef.h>	// HbKeyboardType
#include <hbinputlanguage.h>
#include <e32base.h>

// FORWARD DECLARATIONS
class QString;
class QChar;
class HbKeymap;


// CLASS DECLARATION
NONSHARABLE_CLASS(CPcsKeyMap) : public CBase
	{
    public:
        /**
         * Destructor
         */
		virtual ~CPcsKeyMap();

    public: // New functions
		/**
		 * Maps the given string using the key map.
		 * aSource String to be converted
		 * returns Conversion result, ownership is transferred
		 */
		HBufC* GetMappedStringL(const TDesC& aSource) const;

		/**
		 * Maps the given string using the key map.
		 * aSource String to be converted
		 * returns Conversion result
		 *
		 * Overloaded version using QString.
		 */
		QString GetMappedString(QString aSource) const;

		/**
		 * Computes the upper and lower limits, that can be put into SQL SELECT
		 * statement, from the string.
		 * aString Only contains valid characters (for 12-key keymap this means
		 *  0..9, 'a', 'b', 'f'). This is fulfilled if aString is output from
		 *  GetMappedStringL or GetMappedString, or a sub-string of the output.
		 * aLowerLimit Lower limit
		 * aUpperLimit Upper limit
		 * returns KErrNone if successful
		 */
		TInt GetNumericLimits(QString aString,
							  QString& aLowerLimit,
							  QString& aUpperLimit) const;

		/*
		 * Return the separator character.
		 */
		QChar Separator() const;

	public: // Pure virtual functions
		virtual const QChar ArrayIndexToMappedChar(TInt aArrayIndex) const = 0;

	private: // Pure virtual functions
		virtual TInt ComputeValue(QString aString,
								  TBool aUpperLimit,
								  QString& aValue) const = 0;

	protected: // Virtual functions
		virtual QList<HbInputLanguage> SelectLanguages();

	private: // Virtual functions
		virtual void SetHardcodedCharacters();

	private: // Virtual functions
		/**
         * Returns ETrue if characters that are mapped to certain specific keys,
		 * should be skipped.
         */
		virtual TBool DetermineSpecialCharBehaviour(QString aSource) const;

		virtual TBool ShouldSkipChar(QChar aChar, TBool aSkipHashStar) const;

	protected: // Constructors
		/**
         * Constructor
         */
		CPcsKeyMap(TInt aAmountOfKeys, QChar aPadChar, TInt aMaxKeysStoredInDb);

		/**
		 * Second phase constructor
		 */
		void ConstructL(HbKeyboardType aKeyboardType);

	private: // New functions
		void InitKeyMappings();

		/**
		 * Construct mappings between keys and characters for all languages.
		 */
		void ConstructLanguageMappings(HbKeyboardType aKeyboardType);

        /**
         * Returns ETrue if this language is supported
         */
        TBool IsLanguageSupported(QLocale::Language aLanguage) const;

        /**
         * Returns the key into which the given character is mapped.
         */
		const QChar MappedKeyForChar(const QChar aChar) const;
		
		/**
         * Read the keymap's keys and append them to the internal keymap
         * (iKeyMapping).
         * Returns they amount of keys appended to the iKeyMapping.
         */
		TInt ReadKeymapCharacters(HbKeyboardType aKeyboardType,
                                  const HbKeymap& aKeymap);

	protected:
		// One QString for each key of the keyboard.
		// Each contains all the characters that can originate from that key,
        // considering the languages available on the device.
		//
		// 12-key keymap has keys 1-9,0,*,#.
		//   iKeyMapping[0] has mappings for 1-key, iKeyMapping[1] for 2-key, ...
		//   iKeyMapping[8] for 9-key, iKeyMapping[9] for 0-key,
		//   iKeyMapping[10] for *-key, iKeyMapping[11] for #-key.
		//
		// Qwerty keymap has keys q, w, e, r, ...
		//   iKeyMapping[0] has mappings for q-key, iKeyMapping[1] for w-key, ...
        QList<QString> iKeyMapping;

		// Characters that have been hardcoded to certain keys, regardless of
		// the actual keymaps.
		QString iHardcodedChars;

	private: // Data
		// How many keys (not characters) the keymap has
		const TInt iAmountOfKeys;

		// Unmapped (unknown) characters are mapped to this
		const QChar iPadChar;

		// Largest amount of keypresses that can be stored in predictive search
		// table, using this keyboard.
		const TInt iMaxKeysStoredInDb;

		// For unit testing
		friend class UT_CPcsKeyMap;
	};

#endif // __CPCSKEYMAP_H__

// End of file
