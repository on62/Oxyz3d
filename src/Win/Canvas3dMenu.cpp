#include "Canvas3d.h"

//#include <config.h>
#include <FL/Fl.H>


#include <FL/gl.h>

#include <FL/names.h>
#include <FL/Fl_Menu_Button.H>


#include "Service.h"


#include <iostream>
#include <memory>


#include "Shape/ObjectLine.h"
#include "Shape/ObjectPoly.h"


#include "Shape/GLUtility.h"

#include "Shape/PrimitivFactory.h"
#include "Shape/Light.h"


#include "Shape/ViewProps.h"
#include "Shape/Selection.h"

#include "Application.h"
#include "MyFlWidget.h"

#include "Dialogs.h"

using namespace std;


namespace M3d {

	
#define StrMenu_CreateCube      "Cube"
#define StrMenu_CreateTetra     "Tetraede"
#define StrMenu_CreatePyramid   "Pyramide"
#define StrMenu_CreateOcto      "Octoedre"
#define StrMenu_CreateDodec     "Dodecaedre"
#define StrMenu_CreateIcosahe   "Icosaedre"
	
#define StrMenu_CreateShape          "New Shape "
#define StrMenu_CreateShapeLine      StrMenu_CreateShape  "Line"
#define StrMenu_CreateShapePolyline  StrMenu_CreateShape  "Polyline"
#define StrMenu_CreateShapeFacet     StrMenu_CreateShape "Facet"

#define StrMenu_Move     "Move"
#define StrMenu_MoveX    "Move X"
#define StrMenu_MoveY    "Move Y"
#define StrMenu_MoveZ    "Move Z"
#define StrMenu_MoveNormal    "Move normal"

#define StrMenu_Rot      "Rotate"
#define StrMenu_RotX     "Rotate X"
#define StrMenu_RotY     "Rotate Y"
#define StrMenu_RotZ     "Rotate Z"

	
#define StrMenu_Dup        "Dup"	
#define StrMenu_DupMoveX    StrMenu_Dup  " move X"
#define StrMenu_DupMoveY    StrMenu_Dup  " move Y"
#define StrMenu_DupMoveZ    StrMenu_Dup  " move Z"
#define StrMenu_DupNormal   StrMenu_Dup  " move normal"

#define StrMenu_DupRotX     StrMenu_Dup  " rotate X"
#define StrMenu_DupRotY     StrMenu_Dup  " rotate Y"
#define StrMenu_DupRotZ     StrMenu_Dup  " rotate Z"

#define StrMenu_Extrude     "Extrude"
#define StrMenu_ExtrudeX    StrMenu_Extrude  "  move X"
#define StrMenu_ExtrudeY    StrMenu_Extrude  "  move Y"
#define StrMenu_ExtrudeZ    StrMenu_Extrude  "  move Z"
#define StrMenu_ExtrudeNorm StrMenu_Extrude  "  move normal"

#define  StrMenu_Cut "Cut"
#define  StrMenu_Cut2 "  2"
#define  StrMenu_Cut3 "  3"
#define  StrMenu_Cut4 "  4"
#define  StrMenu_Cut5 "  5"
#define  StrMenu_Cut6 "  6"
#define  StrMenu_Cut7 "  7"
#define  StrMenu_Cut8 "  8"
#define  StrMenu_Cut9 "  9"
#define  StrMenu_Cut10 " 10"
#define  StrMenu_CutN "-----  N"
  
  //-------------------------------------------
  void  Canvas3d::makeMenu(Fl_Menu_Button& pMenu)
  {
    if( PP3d::Selection::Instance().getNbSelected() > 0 )
      {										
	makeMenuSelect(  *cPopup);
      }
    else
      {
	makeMenuPrimitiv( *cPopup );
      }
  }
  //-------------------------------------------
  void  Canvas3d::makeMenuSelect(Fl_Menu_Button& pMenu)
  {

    pMenu.add( StrMenu_Move  "/" StrMenu_MoveX, "", MyMenuCallbackSelect, this);
    pMenu.add( StrMenu_Move  "/" StrMenu_MoveY, "", MyMenuCallbackSelect, this);
    pMenu.add( StrMenu_Move  "/" StrMenu_MoveZ, "", MyMenuCallbackSelect, this);
    pMenu.add( StrMenu_Move  "/" StrMenu_MoveNormal, "", MyMenuCallbackSelect, this);
	
    pMenu.add( StrMenu_Rot  "/" StrMenu_RotX, "",  MyMenuCallbackSelect, this);
    pMenu.add( StrMenu_Rot  "/" StrMenu_RotY, "",  MyMenuCallbackSelect, this);
    pMenu.add( StrMenu_Rot  "/" StrMenu_RotZ, "",  MyMenuCallbackSelect, this);
		
    pMenu.add( StrMenu_Dup "/" StrMenu_DupMoveX, "", MyMenuCallbackSelect, this);
    pMenu.add( StrMenu_Dup "/" StrMenu_DupMoveY, "", MyMenuCallbackSelect, this);
    pMenu.add( StrMenu_Dup "/" StrMenu_DupMoveZ, "", MyMenuCallbackSelect, this);
    pMenu.add( StrMenu_Dup "/" StrMenu_DupRotX, "",  MyMenuCallbackSelect, this);
    pMenu.add( StrMenu_Dup "/" StrMenu_DupRotY, "",  MyMenuCallbackSelect, this);
    pMenu.add( StrMenu_Dup "/" StrMenu_DupRotZ, "",  MyMenuCallbackSelect, this);

    

    if(  PP3d::Selection::Instance().getSelectType() == PP3d::SelectType::Line )
      {
	pMenu.add( StrMenu_Cut "/"  StrMenu_Cut2, "", MyMenuCallbackCutLine, this);
  	pMenu.add( StrMenu_Cut "/"  StrMenu_Cut3, "", MyMenuCallbackCutLine, this);
   	pMenu.add( StrMenu_Cut "/"  StrMenu_Cut4, "", MyMenuCallbackCutLine, this);
   	pMenu.add( StrMenu_Cut "/"  StrMenu_Cut5, "", MyMenuCallbackCutLine, this);
   	pMenu.add( StrMenu_Cut "/"  StrMenu_Cut6, "", MyMenuCallbackCutLine, this);
   	pMenu.add( StrMenu_Cut "/"  StrMenu_Cut7, "", MyMenuCallbackCutLine, this);
   	pMenu.add( StrMenu_Cut "/"  StrMenu_Cut8, "", MyMenuCallbackCutLine, this);
   	pMenu.add( StrMenu_Cut "/"  StrMenu_Cut9, "", MyMenuCallbackCutLine, this);
   	pMenu.add( StrMenu_Cut "/"  StrMenu_Cut10, "", MyMenuCallbackCutLine, this);
   	pMenu.add( StrMenu_Cut "/"  StrMenu_CutN, "", MyMenuCallbackCutLine, this);
      }

    
    switch( PP3d::Selection::Instance().getSelectType() )
      {
      case PP3d::SelectType::Point :	
      case PP3d::SelectType::Line :
      case PP3d::SelectType::Facet :	
	{
	  pMenu.add( StrMenu_Extrude "/" StrMenu_ExtrudeX, "", MyMenuCallbackExtrude, this);
	  pMenu.add( StrMenu_Extrude "/" StrMenu_ExtrudeY, "", MyMenuCallbackExtrude, this);
	  pMenu.add( StrMenu_Extrude "/" StrMenu_ExtrudeZ, "", MyMenuCallbackExtrude, this);
	  pMenu.add( StrMenu_Extrude "/" StrMenu_ExtrudeNorm, "", MyMenuCallbackExtrude, this);
	}
	break;
      case PP3d::SelectType::Poly :
	pMenu.add( StrMenu_Dup "/" StrMenu_DupNormal, "", MyMenuCallbackSelect, this);
	break;
      case PP3d::SelectType::Object :
      case PP3d::SelectType::All :
      default:;
      }

  }
  //-------------------------------------------
  void  Canvas3d::makeMenuPrimitiv(Fl_Menu_Button& pMenu)
  {
    pMenu.add( StrMenu_CreateCube , "^c",   MyMenuCallbackPrimitiv,   this);
    pMenu.add( StrMenu_CreateTetra, "^t",   MyMenuCallbackPrimitiv,   this);
    pMenu.add( StrMenu_CreatePyramid, "^t", MyMenuCallbackPrimitiv, this);
    pMenu.add( StrMenu_CreateOcto,   "^t",  MyMenuCallbackPrimitiv,    this);
    pMenu.add( StrMenu_CreateDodec,   "^t", MyMenuCallbackPrimitiv,   this);
    pMenu.add( StrMenu_CreateIcosahe, "^t", MyMenuCallbackPrimitiv, this, FL_MENU_DIVIDER);

    if( Application::Instance().getDatabase()->isCurrentPoints()&& Application::Instance().getDatabase()->getNbCurrentPoints() == 2 )
      pMenu.add(StrMenu_CreateShapeLine, "", MyMenuCallbackPrimitiv,this);
		
    if( Application::Instance().getDatabase()->isCurrentPoints() && Application::Instance().getDatabase()->getNbCurrentPoints()  >= 3 )
      pMenu.add(StrMenu_CreateShapeFacet, "", MyMenuCallbackPrimitiv, this);

    if( Application::Instance().getDatabase()->isCurrentPoints()&& Application::Instance().getDatabase()->getNbCurrentPoints() >= 2 )
      pMenu.add(StrMenu_CreateShapePolyline, "", MyMenuCallbackPrimitiv,this);
  }
  //-------------------------------------------
  //-------------------------------------------
  //-------------------------------------------
  #define BEGINCALL \
   static bool slFlagDialog=false; \
		\
    Fl_Menu_* mw = (Fl_Menu_*)w;\
    const Fl_Menu_Item* m = mw->mvalue();		\
    if (!m)\
      {\
	printf("NULL\n");\
			\
	return ;\
      }\
\
    printf("%s\n", m->label());\
    M3d::Canvas3d* lCanvas = reinterpret_cast<M3d::Canvas3d*>(pUserData);\
  

  //-------------------------------------------

  void Canvas3d::MyMenuCallbackPrimitiv(Fl_Widget* w, void* pUserData)
  {		
    BEGINCALL  
				
    if( strcmp( m->label(), StrMenu_CreateCube ) == 0)
      {
	CallDialogPrimitiv( slFlagDialog, lCanvas, PP3d::PrimitivFactory::Type::CUBE  );
      }
    else if( strcmp( m->label(), StrMenu_CreateTetra ) == 0)
      {
	CallDialogPrimitiv( slFlagDialog, lCanvas, PP3d::PrimitivFactory::Type::TETRA  );
      }
    else if( strcmp( m->label(), StrMenu_CreatePyramid ) == 0)
      {
	CallDialogPrimitiv( slFlagDialog, lCanvas, PP3d::PrimitivFactory::Type::PYRAMID  );
      }
    else if( strcmp( m->label(), StrMenu_CreateOcto ) == 0)
      {
	CallDialogPrimitiv( slFlagDialog,lCanvas, PP3d::PrimitivFactory::Type::OCTO  );
      }
    else if( strcmp( m->label(), StrMenu_CreateDodec ) == 0)
      {
	CallDialogPrimitiv( slFlagDialog, lCanvas, PP3d::PrimitivFactory::Type::DODEC  );
      }
    else if( strcmp( m->label(), StrMenu_CreateIcosahe ) == 0)
      {
	CallDialogPrimitiv( slFlagDialog, lCanvas, PP3d::PrimitivFactory::Type::ICOSAHED  );
      }
    else if( strncmp( m->label(), StrMenu_CreateShape, strlen(StrMenu_CreateShape)  ) == 0)
      {
	PP3d::Object* lShape=nullptr;
						
	if( strcmp( m->label(), StrMenu_CreateShapeFacet ) == 0)
	  {
	    if(  Application::Instance().getDatabase()->getNbCurrentPoints() >= 3 )
	      {
		lShape = Application::Instance().getDatabase()->convertCurrentLineToFacet();
	      }
	  }
	else if( strcmp( m->label(), StrMenu_CreateShapePolyline ) == 0)
	  {
	    if( Application::Instance().getDatabase()->getNbCurrentPoints() >= 2 )
	      {
		lShape = Application::Instance().getDatabase()->convertCurrentLineToPolylines();
	      }
	  } 
	else if( strcmp( m->label(), StrMenu_CreateShapeLine ) == 0)
	  {
	    if(  Application::Instance().getDatabase()->getNbCurrentPoints() == 2 )
	      {
		lShape = Application::Instance().getDatabase()->convertCurrentLineToLine();
	      }
	  } 
	if( lShape != nullptr )
	  {
	    //						lCanvas->Application::Instance().getDatabase()_>addObject( new PP3d::Object3d( lShape, PP3d::Object3d::GetNewObjecId(), lShape->getClassName() ));
								
	    Application::Instance().redrawAllCanvas3d();
	    Application::Instance().redrawObjectTree();
	  }
      }
  }


  //-------------------------------------------
  void Canvas3d::MyMenuCallbackExtrude(Fl_Widget* w, void* pUserData)
  {	  		
    BEGINCALL
    //    M3d::Canvas3d* lCanvas = reinterpret_cast<M3d::Canvas3d*>(pUserData);

		
    //============== TRANSFORMATION ====================
    if( strcmp( m->label(), StrMenu_ExtrudeX ) == 0)
      {
	std::cout << "Extrude X" << std::endl;
      }
  }
  //-------------------------------------------
  void Canvas3d::MyMenuCallbackSelect(Fl_Widget* w, void* pUserData)
  {	  		
     BEGINCALL  

		
    //============== TRANSFORMATION ====================
    if( strcmp( m->label(), StrMenu_MoveX ) == 0)
      {
	lCanvas->changeUserMode( ModeUser::MODE_TRANSFORM );
	Application::Instance().setCurrentTransformType(Transform::MoveX);
      }
    else if( strcmp( m->label(), StrMenu_MoveY ) == 0)
      {
	lCanvas->changeUserMode( ModeUser::MODE_TRANSFORM );
	Application::Instance().setCurrentTransformType(Transform::MoveY);
      }
    else if( strcmp( m->label(), StrMenu_MoveZ ) == 0)
      {
	lCanvas->changeUserMode( ModeUser::MODE_TRANSFORM );
	Application::Instance().setCurrentTransformType(Transform::MoveZ);
      }
    else if( strcmp( m->label(), StrMenu_MoveNormal ) == 0)
      {
	lCanvas->changeUserMode( ModeUser::MODE_TRANSFORM );
	Application::Instance().setCurrentTransformType(Transform::MoveNormal);
      }
    else if( strcmp( m->label(), StrMenu_RotX ) == 0)
      {
	lCanvas->changeUserMode( ModeUser::MODE_TRANSFORM );
	Application::Instance().setCurrentTransformType(Transform::CenterRotX);
      }
    else if( strcmp( m->label(), StrMenu_RotY ) == 0)
      {
	lCanvas->changeUserMode( ModeUser::MODE_TRANSFORM );
	Application::Instance().setCurrentTransformType( Transform::CenterRotY );
      }
    else if( strcmp( m->label(), StrMenu_RotZ ) == 0)
      {
	lCanvas->changeUserMode( ModeUser::MODE_TRANSFORM );
	Application::Instance().setCurrentTransformType(Transform::CenterRotZ );
      }

    
  }
  //-------------------------------------------
  void Canvas3d::MyMenuCallbackCutLine(Fl_Widget* w, void* pUserData)
  {
    BEGINCALL  ;

    int lNbCut=0;
    //============== CUT  ====================
    if( strcmp( m->label(), StrMenu_Cut2 ) == 0)
      {
	lNbCut =2;
      }
    else  if( strcmp( m->label(), StrMenu_Cut3 ) == 0)
      {
 	lNbCut = 3;
     }
    else  if( strcmp( m->label(), StrMenu_Cut4 ) == 0)
      {
 	lNbCut = 4;
      }
    else  if( strcmp( m->label(), StrMenu_Cut5 ) == 0)
      {
 	lNbCut = 5;
      }
    else  if( strcmp( m->label(), StrMenu_Cut6 ) == 0)
      {
  	lNbCut =6 ;
     }
    else  if( strcmp( m->label(), StrMenu_Cut7 ) == 0)
      {
 	lNbCut = 7;
      }
    else  if( strcmp( m->label(), StrMenu_Cut8 ) == 0)
      {
 	lNbCut = 8;
      }
    else  if( strcmp( m->label(), StrMenu_Cut9 ) == 0)
      {
 	lNbCut = 9;
      }
    else  if( strcmp( m->label(), StrMenu_Cut10 ) == 0)
      {
 	lNbCut = 10;
      }
    else  if( strcmp( m->label(), StrMenu_CutN ) == 0)
      {
	int lVal = 20;
	if( CallDialogInputInt( slFlagDialog, lCanvas, "Cut line", lVal ) )
	  lNbCut = lVal;
      }

    if( lNbCut > 1 )
      {
	//	Application::Instance().getDatabase()_>cutSelectLine( lNbCut );
      } 
  }
  
  //***************************************


} //namespace PP3d
