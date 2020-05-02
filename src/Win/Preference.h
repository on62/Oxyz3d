#ifndef H__PREF__H
#define H__PREF__H

#include "Utils/PPSingletonCrtp.h"

#include "Shape/DebugVars.h"


namespace M3d {
  //*********************************
  class Preference : public virtual PPSingletonCrtp<Preference>{
      
  public:

    //Save
    const std::string sSaveDefault = "default.oxyz";
    std::string cLastSave = sSaveDefault;
      
    //Dbg
    int cDbgEvt = 0;
    int cDbgAct = 0;
    int cDbgLua = 0;
      
    int & cDbgDrw;
    int & cDbgBaz;
    int & cDbgSel;

    //Select
    bool cSelectPassOverLighting = true ; // inlight entity when passing on
      
    friend class PPSingletonCrtp;
      
    Preference();
  };
  //*********************************

} //endspace

#define MyPref M3d::Preference::Instance()

#define DBG_EVT( A )    if( MyPref.cDbgEvt > 0 ) std::cout << "DbgSel0> " << A << std::endl;
#define DBG_EVT1( A )   if( MyPref.cDbgEvt > 1 ) std::cout << "DbgSel1> " << A << std::endl;
#define DBG_EVT2( A )   if( MyPref.cDbgEvt > 2 ) std::cout << "DbgSel0> " << A << std::endl;
#define DBG_EVT_NL( A ) if( MyPref.cDbgEvt > 0 ) std::cout << "DbgSel> " << A ;

  
#define DBG_ACT( A )    if( MyPref.cDbgAct > 0 ) std::cout << "DbgAct0> " << A << std::endl;
#define DBG_ACT1( A )   if( MyPref.cDbgAct > 1 ) std::cout << "DbgAct1> "<< A << std::endl;
#define DBG_ACT2( A )   if( MyPref.cDbgAct > 2 ) std::cout << "DbgAct2> "<< A << std::endl;
  
#define DBG_LUA( A )    if( MyPref.cDbgLua > 0 ) std::cout << "DbgLua1> "<< A << std::endl;
#define DBG_LUA1( A )   if( MyPref.cDbgLua > 1 ) std::cout << "DbgLua2> "<< A << std::endl;
#define DBG_LUA2( A )   if( MyPref.cDbgLua > 2 ) std::cout << "DbgLua3> "<< A << std::endl;
  
#endif