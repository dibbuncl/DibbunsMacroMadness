//
//  DMMGameState.cp
//  DibbunsMacroMadness
//
//  Created by Dibbun on 7/29/26.
//

#ifndef DMMGameState_hpp
#define DMMGameState_hpp 1

#include "ClanLord.h"
#include "TextCmdList_cl.h"
#include "SendText_cl.h"
#include <mach/mach_time.h>
#include "MacroDefs_cl.h"

class GameState
{
public:
    GameState();
    ~GameState();
};
GameState gGameState;

void ListShares( SafeString * oList, bool bOutbound )
{
    __builtin_trap();
}

void ShowInfoText( const char * text )
{
    if( NULL == text )
        return;
    
    if( char(0xa5) == text[0] )
    {
        // debug spew
        if( strcasestr( text+1, "error\r"))
            printf( "🔴\t\t\t%s\n", text + 1);
        else if( strcasestr( text+1, "warning"))
            printf( "⚠️\t\t\t%s\n", text + 1);
        else
            printf( "\t\t\t%s\n", text + 1);
    }
    else
        LOG_MESSAGE("%s\n", text);
}

static inline double GetCurrentEventTime()
{
    static double gConversion = 0.0;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        mach_timebase_info_data_t timebase;
        int err = mach_timebase_info( &timebase);
        if( 0 == err )
            gConversion = (double) timebase.numer / (1e9 * timebase.denom);
    });
    
    
    // return gConversion * mach_absolute_time();
    
    // reduce rounding error from initial uint64_t to double conversion
    // 1. Split into two conversions so exact.
    uint64_t time = mach_absolute_time();
    double lo = uint32_t(time);
    double hi = time >> 32;
    
    // 2. do the high half of the scaling exactly, and add to the rounded low half conversion, then round the result.
    return __builtin_fma( gConversion, hi * 0x1.0p32, lo * gConversion);
}

static inline ulong EventTimeToTicks( double eventTime )
{   // a tick is 1/60th of a second
    eventTime *= 60.0;
    assert(eventTime < 0x1.0p32);
    return ulong(eventTime);
}

ulong GetFrameCounter()
{
    //    return TickCount();
    return EventTimeToTicks( GetCurrentEventTime() );
}

bool MovePlayerToward( int quadrant, int moveSpeed, bool stopIfBalance )
{
    __builtin_trap();
}

void GetSelectedItemName( char * dst, size_t maxlen )
{
    __builtin_trap();
}

DescTable *    LocateMobileByPoint( const DTSPoint * where, int inKind )
{
    __builtin_trap();
}

void GetInvItemNameBySlot( int slot, char * dst, size_t maxlen )
{
    __builtin_trap();
}

void Beep()
{
    __builtin_trap();
}

#define MAKE_KEY_ENTRY( _name, _strName )   case _name: snprintf( buf, 16, _strName); break

inline void GetKeyName( int key, char buf[16] )
{
    buf[0] = (char) key;
    buf[1] = '\0';
    
    switch( key )
    {
        MAKE_KEY_ENTRY(kHomeKey, "<Home>");
        MAKE_KEY_ENTRY(kEnterKey, "<Enter>");
        MAKE_KEY_ENTRY(kEndKey, "<End>");
        MAKE_KEY_ENTRY(kHelpKey, "<Help>");
//        MAKE_KEY_ENTRY(kBackspaceKey, "<Backspace>"); //same as delete
        MAKE_KEY_ENTRY(kDeleteKey, "<Delete>");
        MAKE_KEY_ENTRY(kTabKey, "<Tab>");
        MAKE_KEY_ENTRY(kPageUpKey, "<PageUp>");
        MAKE_KEY_ENTRY(kPageDownKey, "<PageDown>");
        MAKE_KEY_ENTRY(kReturnKey, "<Return>");
        MAKE_KEY_ENTRY(kEscapeKey, "<Escape>");
        MAKE_KEY_ENTRY(kLeftArrowKey, "<LeftArrow>");
        MAKE_KEY_ENTRY(kRightArrowKey, "<RightArrow>");
        MAKE_KEY_ENTRY(kUpArrowKey, "<UpArrow>");
        MAKE_KEY_ENTRY(kDownArrowKey, "<DownArrow>");
        MAKE_KEY_ENTRY(kRightDeleteKey, "<RightDelete>");

        // synthetic character codes
        MAKE_KEY_ENTRY(kMenuModKey, "<Menu>");
        MAKE_KEY_ENTRY(kOptionModKey, "<Option>");
        MAKE_KEY_ENTRY(kShiftModKey, "<Shift>");
        MAKE_KEY_ENTRY(kControlModKey, "<Control>");
        MAKE_KEY_ENTRY(kCapsLockModKey, "<CapsLock>");
        MAKE_KEY_ENTRY(kF1Key, "<F1>");
        MAKE_KEY_ENTRY(kF2Key, "<F2>");
        MAKE_KEY_ENTRY(kF3Key, "<F3>");
        MAKE_KEY_ENTRY(kF4Key, "<F4>");
        MAKE_KEY_ENTRY(kF5Key, "<F5>");
        MAKE_KEY_ENTRY(kF6Key, "<F6>");
        MAKE_KEY_ENTRY(kF7Key, "<F7>");
        MAKE_KEY_ENTRY(kF8Key, "<F8>");
        MAKE_KEY_ENTRY(kF9Key, "<F9>");
        MAKE_KEY_ENTRY(kF10Key, "<F10>");
        MAKE_KEY_ENTRY(kF11Key, "<F11>");
        MAKE_KEY_ENTRY(kF12Key, "<F12>");
        MAKE_KEY_ENTRY(kF13Key, "<F13>");
        MAKE_KEY_ENTRY(kF14Key, "<F14>");
        MAKE_KEY_ENTRY(kF15Key, "<F15>");
        MAKE_KEY_ENTRY(kF16Key, "<F16>");
        default:
            break;
    };


}

bool IsKeyDown( int key )// true if the key is down
{
    //See View_dts.h:53 for keycodes
    char keyName[16];
    GetKeyName(key, keyName);
    
#if DEBUG
#   warning Ignoring keydown tests
#else
    LOG_ERROR("Ignoring keydown test for key: %s (0x%hx)", keyName, key );
#   warning Ignoring keydown tests
#endif
    return false;
}



int gAckFrame;                // latest state data frame
CTextCmdList * gCmdList;
PrefsData gPrefsData;                // preferences
char gSelectedPlayerName[ kMaxNameLen ];
SendTextField    gSendText;         // Read in CExecutingMacro::CExecutingMacro to set @text and @textSel variables
DescTable *    gThisPlayer;
bool        bDebugMessages;

#pragma mark -

#include "VersionNumber_cl.h"

static void InitPrefs()
{
    memset( (char*) &gPrefsData, 0, sizeof(gPrefsData));
    gPrefsData.pdVersion = kPrefsVersion;
    gPrefsData.pdGWPos = DTSRect(0, 0, 200, 200);
    gPrefsData.pdPlayersPos = DTSRect(200, 0, 300, 100);
    gPrefsData.pdInvenPos = DTSRect(200, 100, 300, 200);
    gPrefsData.pdTextPos = DTSRect(0,200,300,300);
    gPrefsData.pdMsgPos = DTSPoint(0, 0);
    gPrefsData.pdShowNames = 1;
    gPrefsData.pdGodHideMonsterNames = 1;
    gPrefsData.pdSound = 1;
    gPrefsData.pdTimeStamp = 1;
    gPrefsData.pdLargeWindow = 0;
    gPrefsData.pdBrightColors = 1;
    gPrefsData.pdMoveControls = kMoveClickHold;
    gPrefsData.pdSuppressClanning = 0;
    gPrefsData.pdSuppressFallen = 0;
    gPrefsData.pdMusicPlay = 1;
    gPrefsData.pdMusicVolume = 80;
    gPrefsData.pdSavePassword = 1;

    strcpy( gPrefsData.pdAccount, "<PlayerAccountName>");
    strcpy( gPrefsData.pdAcctPass, "<AcctPassword>");
    strcpy( gPrefsData.pdCharName, "<CharacterName>");
    strcpy( gPrefsData.pdCharPass, "<CharPassword>");
    
    gPrefsData.pdShowFrameTime = 1;
    gPrefsData.pdSaveMsgLog = 1;
    gPrefsData.pdSaveTextLog = 1;
    gPrefsData.pdShowDrawTime = 0;
    gPrefsData.pdShowImageCount = 0;
    gPrefsData.pdFastBlitters = 1;
    
    strcpy( gPrefsData.pdHostAddr, "<localhost>");
    strcpy( gPrefsData.pdProxyAddr, "");

 //   CLStyleRecord    pdStyles[ kMsgMaxStyles ];    // misc text styles                    Leave 0
 
    gPrefsData.pdMaxNightPct = 100;
    gPrefsData.pdBubbleBlitter = kBlitterTransparent;
    gPrefsData.pdFriendBubbleBlitter = kBlitterTransparent;
    
    
// v30
 //   DTSBoolean        pdSpeaking[ kMsgMaxStyles ];    // speak this msg aloud?        c    Leave 0
    
// v31
    gPrefsData.pdItemPrefsVers = kItemPrefsVersion;            // version of item-prefs            g
    
// v32
    gPrefsData.pdHBColorStyle = 0;                // value, type, ...                    c
    gPrefsData.pdHBPlacement = 0;                // center, ll, lr, ur, ...            c
    
// v33
    gPrefsData.pdThinkNotify = 1;                // notify for /thinkto's            c

// v34
    // no new fields, just reinterpreted the saved passwords
    // they are now SimpleEncrypted on disk

// v35
    gPrefsData.pdDisableCmdQ = 0;                // mask cmd-q menu key when online

// v36
    gPrefsData.pdNoSuppressFriends = 1;        // friend messages aren't suppressed

// v38
    gPrefsData.pdUseArbitrary = true;                // use an arbitrary client port        g
    gPrefsData.pdClientPort = 5000;                // client port                        g
    
// v39                                            // changed password encryption        g+c

// v40
    gPrefsData.pdTreatSharesAsFriends = false;        // added treat-shares-as-friends

// v41    no new fields, but enlarged pdStyles[] and pdSpeaking[]

// v42
    gPrefsData.pdSoundVolume = 100;                // [0-100] controls CLsounds, not music movies

// v43
    gPrefsData.pdSurveys = 0;                    // bitmap for hardware survey

// v44
    gPrefsData.pdNumFrames = 0;                // total number of frames played
    gPrefsData.pdLostFrames = 0;                // number of lost frames
    
// v45
    gPrefsData.pdClientVersion = kFullVersionNumber;            // client version number
    
// v46
    gPrefsData.pdCompiledClient = kFullVersionNumber;            // compiled client version number

// v47
    gPrefsData.pdOpenGLEnable = 0;                // master enable
    gPrefsData.pdOpenGLEnableEffects = 0;        // ogl-specific effects, like smooth night
    gPrefsData.pdOpenGLRenderer = 0;            // 0 = let client pick renderer
                                                // 1 = force software renderer
                                                // 2 = allow "non-compliant" hardware
// v48
    gPrefsData.pdNewLogEveryJoin = 1;            // start a new log file with every join
    gPrefsData.pdNoMovieTextLogs = 1;            // don't save log files when playing movies

// v49
    gPrefsData.pdBardVolume = 100;                // 0-100 separate master volume for bard songs

// v50
    gPrefsData.pdLanguageID = kRealLangDefault;            // initial gRealLangID, if MULTILINGUAL (else ignored)

}

GameState::GameState()
{
    InitPrefs();
}

GameState::~GameState()
{
    
}

#pragma mark -

void
SendTextField::UpdateMacroStatus( bool macroExecuting )
{
    
}


#endif /* DMMGameState_hpp */
