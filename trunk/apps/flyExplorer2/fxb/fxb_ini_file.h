//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXB_INI_FILE_H__
#define __FXB_INI_FILE_H__
#pragma once

namespace fxb
{
class IniFile
{
public:
    enum { InvalidIndex = 0xffffffff };

    class Section;
    class Key;
    class Comment;

protected:
    typedef std::list<Section *> SectionList;
    typedef std::list<Key     *> KeyList;
    typedef std::list<Comment *> CommentList;
    typedef std::tr1::unordered_map<std::tstring, Key *> KeyMap;

public:
    class SectionIterator
    {
        friend class IniFile;

    public:
        SectionIterator(void);

    public:
        Section *        operator*  () const;
        SectionIterator& operator++ ();
        SectionIterator  operator++ (xpr_sint_t);
        bool             operator== (const SectionIterator &aIterator) const;
        bool             operator!= (const SectionIterator &aIterator) const;

    protected:
        IniFile::SectionList::const_iterator mIterator;
    };

    class KeyIterator
    {
        friend class IniFile;

    public:
        KeyIterator(void);

    public:
        Key *        operator*  () const;
        KeyIterator& operator++ ();
        KeyIterator  operator++ (xpr_sint_t);
        bool         operator== (const KeyIterator &aIterator) const;
        bool         operator!= (const KeyIterator &aIterator) const;

    protected:
        IniFile::Section                 *mSection;
        IniFile::KeyList::const_iterator  mIterator;
    };

public:
    IniFile(const xpr_tchar_t *aIniPath = XPR_NULL);
    virtual ~IniFile(void);

public:
    const xpr_tchar_t *getPath(void) const;
    void setPath(const xpr_tchar_t *aIniPath);

    xpr_bool_t readFile(void);
    xpr_bool_t writeFile(xpr::CharSet aCharSet) const;

    void clear(void);

public:
    // get value for key (property)
    const xpr_tchar_t *getValue(const Key *aKey, const xpr_tchar_t *aDefValue = XPR_NULL) const;
    const xpr_tchar_t *getValue(const Section *aSection, const xpr_tchar_t *aKey, const xpr_tchar_t *aDefValue = XPR_NULL) const;
    const xpr_tchar_t *getValue(const xpr_tchar_t *aSection, const xpr_tchar_t *aKey, const xpr_tchar_t *aDefValue = XPR_NULL) const;

    // set command
    void setComment(const xpr_tchar_t *aComment);

    // set value for key
    xpr_bool_t setValue(Key *Key, const xpr_tchar_t *aValue);
    xpr_bool_t setValue(Section *aSection, const xpr_tchar_t *aKey, const xpr_tchar_t *aValue);
    xpr_bool_t setValue(const xpr_tchar_t *aSection, const xpr_tchar_t *aKey, const xpr_tchar_t *aValue);

    // for section iteration
    SectionIterator beginSection(void) const;
    SectionIterator endSection(void) const;

    // for key iterator
    KeyIterator beginKey(Section *aSection) const;
    KeyIterator endKey(Section *aSection) const;

    // for section
    Section           *addSection(const xpr_tchar_t *aSection);
    xpr_size_t         getSectionCount(void) const;
    const xpr_tchar_t *getSectionName(const Section *aSection) const;
    Section           *findSection(const xpr_tchar_t *aSection) const;
    void               clearSection(const xpr_tchar_t *aSection);
    void               clearSection(void);

    // for key
    xpr_size_t         getKeyCount(const Section *aSection) const;
    xpr_size_t         getKeyCount(const xpr_tchar_t *aSection) const;
    const xpr_tchar_t *getKeyName(const Key *aKey) const;
    Key               *findKey(const Section *aSection, const xpr_tchar_t *aKey) const;
    Key               *findKey(const xpr_tchar_t *aSection, const xpr_tchar_t *aKey) const;
    void               clearKey(Section *aSection, const xpr_tchar_t *aKey);
    void               clearKey(const xpr_tchar_t *aSection, const xpr_tchar_t *aKey);

    // for comment
    xpr_size_t         getCommentCount(void);
    const xpr_tchar_t *getComment(const Comment *aComment) const;
    void               clearComment(void);

protected:
    std::tstring mPath;

    SectionList mSectionList;
    CommentList mCommentList;
};
} // namespace fxb

#endif // __FXB_INI_FILE_H__
