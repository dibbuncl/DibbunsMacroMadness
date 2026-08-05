//
//  DMMTextField.cp
//  DibbunsMacroMadness
//
//  Created by Dibbun on 7/29/26.
//


#include "DMMTextField.hpp"

/*
**    DTSTextFieldPriv
*/
DTSDefineImplementFirmNoCopy(DTSTextFieldPriv)


void DTSTextField::Hide()
{
    __builtin_trap();
}

void DTSTextField::Show()
{
    __builtin_trap();
}

void DTSTextField::Clear()
{
    __builtin_trap();
}

void DTSTextField::GetSelect( int * start, int * stop ) const{ return priv.p->GetSelect(start, stop); }
int  DTSTextField::GetTextLength() const{ return priv.p->GetTextLength(); }
void DTSTextField::GetText( char * text, size_t bufferSize ) const { return priv.p->GetText(text, bufferSize); };
void DTSTextField::SetText( const char * newText ){ return priv.p->SetText(newText); }
void DTSTextField::SelectText(int start, int stop){ return priv.p->SelectText(start, stop);}
void DTSTextField::Insert( const char * newText ){return priv.p->InsertText(newText);}


bool DTSTextField::KeyStroke( int key, uint modifiers ){ __builtin_trap(); return false; }
void DTSTextField::Idle(){ __builtin_trap(); }

void DTSTextField::DoDraw(){ __builtin_trap(); }
bool DTSTextField::Click( const DTSPoint& loc, ulong time, uint modifiers ){ __builtin_trap(); return false; }
void DTSTextField::Move( DTSCoord left, DTSCoord top ){ __builtin_trap(); }
void DTSTextField::Size( DTSCoord width, DTSCoord height ){ __builtin_trap(); }


#pragma mark -

DTSTextFieldPriv::DTSTextFieldPriv() :
    textLastStart(-1),
    textLastStop(-1),
    textUndoStart(-1),
    textUndoStop(-1),
    textUndoBuff(NULL),
    textStorage(NULL),
    textAnchor(kTEAnchorNone),
    textActive(false),
    textSelStart(kTXNStartOffset),
    textSelStop(kTXNStartOffset)
{
    
}

DTSTextFieldPriv::~DTSTextFieldPriv()
{
    delete textUndoBuff; textUndoBuff = NULL;
    delete textStorage; textStorage = NULL;
}

// interface
void DTSTextFieldPriv::GetSelect( int * start, int * stop ) const
{
    if(start)
        start[0] = textSelStart;
    if(stop)
        stop[0] = textSelStop;
}

void DTSTextFieldPriv::SelectText(int start, int stop)
{
    textSelStart = start;
    textSelStop = stop;
}

int DTSTextFieldPriv::GetTextLength() const
{
    if(NULL == textStorage)
        return 0;
    
    return (int) min(textStorage->GetTextLength(), size_t(INT_MIN));
}

void DTSTextFieldPriv::GetText( char * text, size_t bufferSize ) const
{
    if(NULL == text || 0 == bufferSize)
        return;

    size_t len = GetTextLength() + 1;
    if( len == 1 )
        text[0] = '\0';
    else
        memcpy(text, textStorage, min(len, bufferSize));
    
    if( bufferSize < len)
        text[bufferSize - 1] = '\0';
}

void DTSTextFieldPriv::SetText( const char * newText )
{
    PushStorage( new TextStorage(newText) );
}

void DTSTextFieldPriv::InsertText( const char * newText )
{
    if( NULL == newText || newText[0] == '\0')
        return;
    
    if( NULL == textStorage)
        return SetText(newText);
    
    size_t selectLength = textSelStop - min(textSelStart, textSelStop);
    size_t oldTextLength = textStorage->GetTextLength();
    size_t unmodifiedTextLength = oldTextLength - min(oldTextLength, selectLength);
    if( 0 == unmodifiedTextLength)
        return SetText(newText);
    
    size_t newTextLength = strlen(newText) + unmodifiedTextLength;
    
    TextStorage * newStorage = new TextStorage(newTextLength);
    if( NULL == newStorage )
        return;
    char * string = const_cast<char*>(newStorage->GetText());
    char * p = string;
    const char * oldString = textStorage->GetText();
    size_t lastRunSize = oldTextLength - min(oldTextLength, size_t(textSelStop));
    assert( newTextLength == max(textSelStart,0) + newTextLength + lastRunSize);

    if( textSelStart > 0)
    {
        memcpy( p, oldString, textSelStart);
        p += textSelStart;
    }
    memcpy( p, newText, newTextLength);
    p += newTextLength;

    if( lastRunSize > 0)
        memcpy(p, oldString + textSelStop, lastRunSize);

    PushStorage(newStorage);
}


#pragma mark -

#include "SendText_cl.h"

void SendTextField::Init( DTSView * parent, const DTSRect * box, const char * font, int sz ){ __builtin_trap(); }
void SendTextField::DoDraw(){ __builtin_trap(); }
OSStatus SendTextField::HandleEvent( CarbonEvent& ){ __builtin_trap(); }

#pragma mark -

#include "TextCmdList_cl.h"

bool
CTextCmdList::SendCmd( bool inCache, const char * inCmd, bool doSend )
{
    const char * sendVal = doSend ? "true" : "false";
    LOG_MESSAGE("Command: \"%s\" (send = %s)", inCmd, sendVal);
    return true;
}
