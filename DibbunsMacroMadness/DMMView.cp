//
//  DMMView.cp
//  DibbunsMacroMadness
//
//  Created by Dibbun on 7/29/26.
//


#include "View_dts.h"

class DTSViewPriv
{
public:
//    GrafPtr            viewPort;
//    GDHandle        viewDevice;
//    DTSView *        viewParent;
//    DTSView *        viewChild;
//    DTSView *        viewSibling;
//    DTSRect            viewBounds;
//    bool            viewVisible;
#ifndef AVOID_QUICKDRAW
//    DTSRegion        viewContent;
#endif
    
    // constructor/destructor
                        DTSViewPriv();
                        ~DTSViewPriv();
    
    // interface
//    void                InitViewPort( GrafPtr port, GDHandle device );
//    void                Detach();
};

/*
**    DTSViewPriv
*/
DTSDefineImplementFirmNoCopy(DTSViewPriv)

DTSViewPriv::DTSViewPriv()
{
    
}

DTSViewPriv::~DTSViewPriv()
{
    
}


void DTSView::Show(){__builtin_trap();}
void DTSView::Hide(){__builtin_trap();}
void DTSView::DoDraw(){__builtin_trap();}
void DTSView::Move( DTSCoord h, DTSCoord v ){__builtin_trap();}
void DTSView::Size( DTSCoord width, DTSCoord height ){__builtin_trap();}
bool DTSView::Click( const DTSPoint& loc, ulong when, uint modifiers ){__builtin_trap(); return false;}

