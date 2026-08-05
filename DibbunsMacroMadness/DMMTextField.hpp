//
//  DMMTextFieldPriv.hpp
//  DibbunsMacroMadness
//
//  Created by Dibbun on 7/29/26.
//

#ifndef DMMTextField_hpp
#define DMMTextField_hpp 1

#include "TextField_dts.h"
#include <limits.h>

constexpr int kTXNStartOffset = 0;
constexpr int kTXNEndOffset = INT_MAX;

class DTSTextFieldPriv
{
private:
    struct TextStorage
    {
    private:
        struct TextStorage *    next;
        char *                  text;
        size_t                  textLen;

    public:
        TextStorage( const char * buf) : TextStorage( buf ? strlen(buf) : 0UL){ memcpy(text, buf, buf ? strlen(buf) + sizeof('\0') : 0UL );}
        TextStorage( size_t len ) : next(NULL), text(new char[len + sizeof('\0')]), textLen(len){ memset(text, 0, len + sizeof('\0'));}
        ~TextStorage(){ assert(NULL == next); next = NULL; textLen = 0; delete [] text; text = NULL;}

        inline const char * GetText() const { return text;}
        inline void SetText( const char * newText)
        {
            delete [] text; text = NULL; textLen = 0;
            if( NULL == newText )
                return;
            
            textLen = strlen(newText);
            text = new char[textLen + sizeof('\0')];
            if( text )
                memcpy( text, newText, textLen + sizeof('\0'));
        }
        inline size_t GetTextLength() const { return textLen; }
        
        TextStorage * WARN_UNUSED SwapNext( TextStorage * newNext){ TextStorage * old = next; next = newNext; return old;}
        TextStorage * GetNext() const { return next; }
    };


    int            textLastStart;
    int            textLastStop;
    int            textUndoStart;
    int            textUndoStop;
    const char *   textUndoBuff;
    TextStorage *  textStorage;
    int            textAnchor;
    bool           textActive;
    int            textSelStart;    // inclusive
    int            textSelStop;     // exclusive --  selection is [start, stop)


public:


    
    // constructor/destructor
    DTSTextFieldPriv();
    ~DTSTextFieldPriv();
    
    // interface
    void GetSelect( int * start, int * stop ) const;
    int  GetTextLength() const;
    void GetText( char * text, size_t bufferSize ) const;
    void SetText( const char * newText );
    void SelectText(int start, int stop);
    void InsertText( const char * newText );

private:
    // declared but not defined
    DTSTextFieldPriv( const DTSTextFieldPriv& );
    DTSTextFieldPriv&    operator=( const DTSTextFieldPriv& );
    
    inline void PushStorage( TextStorage * newStorage )
    {
        if( NULL == newStorage) return;
        assert(newStorage != textStorage);
        TextStorage * UNUSED resultIsNullAndIsIgnored = newStorage->SwapNext(textStorage);
        assert(NULL == resultIsNullAndIsIgnored);
        textStorage = newStorage;
    }
    TextStorage * WARN_UNUSED PopStorage()
    {
        TextStorage * result = textStorage;
        if( result)
            textStorage = textStorage->SwapNext(NULL);
        return result;
    }
};

#endif // !DMMTextField_hpp

