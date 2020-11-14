#include "Selection.h"

#include "DataBase.h"
#include "ViewProps.h"

#include "Matrice.h"

#include "Object.h"

#include "DebugVars.h"

#include "OwnerVisitor.h"
#include "Modif/Modif.h"


using namespace std;

namespace PP3d {

  //******************************************

  
  Selection::Selection()
  {
  }
  //--------------------------------
  void Selection::getSelectPoints( std::unordered_set<Point*>& pPoints )
  {
    GetPoints( cSelectObj, pPoints );
  }
  //--------------------------------
  void Selection::changeSelectType( SelectType pType )
  {          
    if( cSelectType == pType )
      { 
	return;
      }    
    
    OwnerEntityVisitor lVisit;
    
    SelectType lOldType = cSelectType;
    cSelectType = pType;
    
    if( pType != SelectType::Null )
      {
	if( lOldType > cSelectType )
	  execVisitorOnEntity( lVisit );
	else
	  lVisit.addOwnersOf( cSelectObjVect );
      }
	  		
    removeAll();
    
    changeSelectType( pType, lVisit );
  }
  //--------------------------------
  void Selection::changeSelectType(  SelectType pType, OwnerEntityVisitor & lVisit )
  {            
    std::vector<EntityPtr> * lVectEntity = nullptr;
    switch( pType )
      {
      case SelectType::Point:
	lVectEntity = (std::vector<EntityPtr>*) & lVisit.cVectPoints;
	break;
	
      case SelectType::Line:  ;	
	lVectEntity = (std::vector<EntityPtr>*) & lVisit.cVectLines;
	break;
	
      case SelectType::Facet:  ;
	lVectEntity = (std::vector<EntityPtr>*) & lVisit.cVectFacets;
	break;
	
      case SelectType::Object:;
	lVectEntity = (std::vector<EntityPtr>*) & lVisit.cVectObjects;
	break;
	
      case SelectType::Poly: ;
	lVectEntity = (std::vector<EntityPtr>*) & lVisit.cVectPolys;
	break;
	
      case SelectType::All: ;
	lVectEntity = (std::vector<EntityPtr>*) & lVisit.cVectAllEntity;
	break;

      case SelectType::Null: ; 	
  	break;
    }
    if( lVectEntity != nullptr )
      {	
	for( EntityPtr lEntity : *lVectEntity )
	  {
	    addEntity( lEntity, true );
	  }
      }
  }
  //--------------------------------
  void Selection::addGoodEntityFor( std::vector<EntityPtr> & iToSelect )
  {
   OwnerEntityVisitor lVisit;
    
    for( EntityPtr lEntity : iToSelect )
      {
	lVisit.addOwnersOf( lEntity );
      }
    changeSelectType( cSelectType, lVisit);
  }
  //--------------------------------
  void Selection::addEntity( EntityPtr ioEntity, bool iSelectAll)
  {
    //    std::cout << "Selection::addEntity " << ioEntity->getId() << ":" << ioEntity->getType() << std::endl;


    auto lPair = cSelectObj.emplace( ioEntity );
    if( lPair.second == false )
      return ; // already selected

    
    ioEntity->setSelect( true );
    cSelectObjVect.push_back( ioEntity );

    
    if( iSelectAll && ioEntity->getType() == ShapeType::Line )
      {
	LinePtr lLine =  dynamic_cast<LinePtr>(ioEntity)->getReverseLine();
	if( lLine )
	  {
	    addEntity( lLine, false );	    
	  }
      }
    //    std::cout << "======================================="<< std::endl;
    //    std::cout << *this << std::endl;
    //    std::cout << "======================================="<< std::endl;
  }

  //--------------------------------
  void Selection::removeEntity( EntityPtr ioEntity, bool iSelectAll )
  {
    if( ioEntity->isSelect() )
      {
	ioEntity->setSelect( false );
	cSelectObj.erase( ioEntity );
	for( size_t i = 0; i< cSelectObjVect.size(); i++ )
	  {
	    if( cSelectObjVect[i] == ioEntity )
	      {
		cSelectObjVect.erase( cSelectObjVect.begin() + i );
		break;
	      }

	  }
	
	if( iSelectAll && ioEntity->getType() == ShapeType::Line )
	  {
	    LinePtr lLine =  dynamic_cast<LinePtr>(ioEntity)->getReverseLine();
	    if( lLine )
	      removeEntity( lLine, false );
	  }
      }
  }
  //--------------------------------
  void Selection::removeAll()
  {
    for( EntityPtr lEntity : cSelectObj )
      {
	lEntity->setSelect( false );
      }
		
    cSelectObj.clear();
    cSelectObjVect.clear();
  }
  //--------------------------------
  bool Selection::addToSelection( EntityPtr lEntity)
  {
    DBG_SEL_NL( "addToSelection " << isSelected(lEntity) << " " << lEntity->isSelect() << " " );
    if( lEntity->isSelect() )
      {
	DBG_SEL_NN( " removeEntity " );
	removeEntity( lEntity );
      }
    else
      {
	DBG_SEL_NN( " addEntity " );
	addEntity( lEntity );
      }
		
    DBG_SEL_LN( " --> " << isSelected(lEntity) << " " << lEntity->isSelect() << " " );
	
    return true;
  }
  //--------------------------------

  Point3d Selection::getCenter( DataBase& pDatabase)
  {
    Point3d lCenter;
    std::unordered_set<Point*> cPoints;

    // comme il ne faut pas compter plusieurs fois un point, on
    // va les recuperer dans un set ( pas trés rapide mais l'autre solution serait de marquer les point comme deja fais qq part : bof )
    GetPoints( cSelectObj, cPoints );
		
    for( auto lIter = cPoints.begin(); lIter != cPoints.end(); ++lIter )
      {
	lCenter += (*lIter)->cPt;
      }
		
    if( cPoints.size() > 0 )
      lCenter /= (double) cPoints.size();

    return lCenter;
  }
  //--------------------------------
  bool MyCmp( PP3d::PickingHit & A, PP3d::PickingHit& B)
  {
    //		 cout << " sort A " << *A << endl;
    //	 cout << " sort B " << *B << endl;
		 
    if( A.cEntity->getType() == B.cEntity->getType() )
      return ( A.cZ1 > B.cZ1);

    return ( A.cEntity->getType() >  B.cEntity->getType());
  }
  //--------------------------------
	
  void insertionSort( std::vector<PP3d::PickingHit>& pVect ) 
  { 
    long int i,  j, size;
	
    size = (long)pVect.size();
		
    for (i = 1; i < size; i++) 
      { 
	PP3d::PickingHit key = pVect[i];
				
	j = i-1;  
	DBG_SEL( "i:" << i << " j:" << j << " size:" << size );
	while (j >= 0 && MyCmp( pVect[j], key )  )
	  {
	    pVect[j+1] = pVect[j]; 
						
	    j = j-1; 					
	  } 

	pVect[j+1] = key; 
      } 
  } 
	
  //--------------------------------
  bool Selection::selectPickingHit( std::vector< PP3d::PickingHit>& pHits, DataBase& cBase, SelectMode& pSelectMode, bool pFlagOnlyHightlight )
  {
    //  DBG_SEL_NL(" <Selection::selectPickingHit : " << pHits.size() <<  " SM:" << pSelectMode << " " );
		
    //    std::cout << std::endl << "<<<<<Selection::selectPickingHit size: " << pHits.size() <<  " SM:" << pSelectMode << " " << std::endl;
		
    for( PP3d::PickingHit& pHit : pHits )
      {
	EntityPtr lEntity = cBase.findEntity( pHit.cName );				
	if( lEntity == nullptr )
	  {
	    std::cout <<  "***************** Selection "<<pHit.cName<<" not found ! ABORT selectPickingHit **********************" << endl;
	    DBG_SEL( "***************** Selection "<<pHit.cName<<" not found ! ABORT selectPickingHit **********************");
	    return false;
	  }				
				
	pHit.cEntity = lEntity;
      }

    //    cout<<"**************** 22222 ***********************"<< endl;
		
    // On tri les plus petits objets en premier, entre meme type c'est le Z qui compte
    //	std::sort( pHits.begin(), pHits.end(), MyCmp );

    // Je n'utilise pas std::sort : il plante !!!
    insertionSort( pHits );

    //   for( PP3d::PickingHit& pHit : pHits )
    //     {
    //	DBG_SEL( "Hit :" << pHit );			
    //    }
    //    cout<<"**************** 333 ***********************"<< endl;

						
#if __GNUC__ > 6 
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
#endif
    /*    for( PP3d::PickingHit& pHit : pHits )
      {				
	cout << ">>> Hit:" << pHit << endl;
      }
    */
    for( PP3d::PickingHit& pHit : pHits )
      {				
	//	cout << "Hit:" << pHit << endl;
	EntityPtr lEntity = pHit.cEntity;
								
	if(pFlagOnlyHightlight)
	  {
	    lEntity->setHighlight( true );
	    return true;
	  }
	

	if( isSelected( lEntity ) )
	  {
	    if ( lEntity->isSelect() == false )
	      {
		cerr <<"Selection Error "<< lEntity->getId() <<" mismach"<< endl;
	      }


	    switch( pSelectMode )
	      {
	      case SelectMode::Undefine:
		pSelectMode = SelectMode::Unselect;
	      case SelectMode::Inverse:
	      case SelectMode::Unselect:
		removeEntity(lEntity);
		break;
	      case SelectMode::Select:
		DBG_SEL( " NO " );
		//		return false;
	      }
	    //	    cout <<  " R ->SM:" << pSelectMode << endl;
	    return true;
	  }				
	else
	  {
	    //	    cout <<  " U " ;
	    switch( pSelectMode )
	      {
	      case SelectMode::Undefine:
		pSelectMode = SelectMode::Select;
	      case SelectMode::Inverse:
	      case SelectMode::Select:
		/*
		std::cout << " **** selectPickingHit:" << cSelectType << ":" << lEntity->getType() << ":" <<  lEntity->getOwners().size()  << std::endl;
		if(  cSelectType == SelectType::Poly
		     && lEntity->getType() != ShapeType::Object
		     && lEntity->getOwners().size() > 0 )
		  { // il faut recuperer l'object a partir de owners
		    for( Entity * lTmp : lEntity->getOwners() )
		      { 
			std::cout << "selectPickingHit add owner :"    << lTmp->getType()   << std::endl;
			addEntity( lTmp );
		      }
		  }
		else
		*/
		  addEntity( lEntity );								
		break;
	      case SelectMode::Unselect:
		//		cout <<  " NO " << endl;
		return false;
	      }
						
	    //	    cout <<  " A ->SM:" << pSelectMode << endl;
	    return true;
	  }				
      }
#if __GNUC__ > 6 
#pragma GCC diagnostic pop
#endif		
		
    //   cout <<  " KO " << endl;
    return false;
  }
  //--------------------------------
  bool Selection::selectPickingHitRect( std::vector< PP3d::PickingHit>& pHits, DataBase& cBase, SelectMode& pSelectMode, bool pFlagOnlyHightlight )
  {
    //    DBG_SEL_NL(" <Selection::selectPickingHitRect : " << pHits.size() <<  " SM:" << pSelectMode << " " );

    std::vector<EntityPtr> lToAdd;
    std::vector<EntityPtr> lToRemove;

    lToAdd.reserve(pHits.size() );
    lToRemove.reserve(pHits.size() );
      
    for( PP3d::PickingHit& pHit : pHits )
      {       
	EntityPtr lEntity = cBase.findEntity( pHit.cName );				
	if( lEntity == nullptr )
	  {
	    cout<<  "***************** Selection "<< pHit.cName<<" not found ! ABORT selectPickingHit **********************" << endl;
	    DBG_SEL( "***************** Selection "<<pHit.cName<<" not found ! ABORT selectPickingHit **********************");
	    continue;
	  }								
	pHit.cEntity = lEntity;
      }

						
#if __GNUC__ > 6 
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
#endif

    for( PP3d::PickingHit& pHit : pHits )
      {
	EntityPtr lEntity = pHit.cEntity;
	
	if( lEntity == nullptr )
	  continue;
	
								
	if(pFlagOnlyHightlight)
	  {
	    lEntity->setHighlight( true );
	    continue;
	  }
	
	if( isSelected( lEntity ) )
	  {
	    if ( lEntity->isSelect() == false )
	      {
		cerr <<"Selection Error "<< lEntity->getId() <<" mismach"<< endl;
		continue;
	      }

	    switch( pSelectMode )
	      {
	      case SelectMode::Undefine:
		pSelectMode = SelectMode::Unselect;
	      case SelectMode::Inverse:
	      case SelectMode::Unselect:
		lToRemove.push_back(lEntity);
		break;
	      case SelectMode::Select:
		DBG_SEL( " NO " );
	      }
	    //	    cout <<  " R ->SM:" << pSelectMode << endl;
	    continue;
	  }				
	else
	  {
	    //	    cout <<  " U " ;
	    switch( pSelectMode )
	      {
	      case SelectMode::Undefine:
		pSelectMode = SelectMode::Select;
	      case SelectMode::Inverse:
	      case SelectMode::Select:
		lToAdd.push_back( lEntity );
		break;
	      case SelectMode::Unselect:
		//		cout <<  " NO " << endl;
		continue;
	      }
						
	    //	    cout <<  " A ->SM:" << pSelectMode << endl;
	  }				
      }


   for( EntityPtr lEntity : lToRemove )
      {
	removeEntity(lEntity);
      }
   for( EntityPtr lEntity : lToAdd )
      {
	addEntity( lEntity );
      }



#if __GNUC__ > 6 
#pragma GCC diagnostic pop
#endif		
		
    //   cout <<  " KO " << endl;
    return true;
  }
  //--------------------------------
  const char* Selection::GetStrSelectType( SelectType pSelectType )
  {
    switch( pSelectType )
      {
      case SelectType::Null:   return "None";
      case SelectType::Point:  return "Point";
      case SelectType::Line:   return "Line";
      case SelectType::Facet:  return "Facet";
      case SelectType::Poly:   return "Poly";
      case SelectType::Object: return "Object";
      case SelectType::All:    return "All";
      }
    return "SelectType::unknown";
  }
  //--------------------------------
  SelectType  Selection::GetSelectTypeFromStr( const char* pStr )
  {
    if( ::strcmp( pStr, "None" ) == 0 )
      return SelectType::Null;
    else                if( ::strcmp( pStr, "Point" ) == 0 )
      return SelectType::Point;
    else		if( ::strcmp( pStr, "Line" ) == 0 )
      return SelectType::Line;
    else		if( ::strcmp( pStr, "Facet" ) == 0 )
      return SelectType::Facet;
    else		if( ::strcmp( pStr, "Poly" ) == 0 )
      return SelectType::Poly;
    else		if( ::strcmp( pStr, "Object" ) == 0 )
      return SelectType::Object;
    else		if( ::strcmp( pStr, "All" ) == 0 )
      return SelectType::All;
    
    return SelectType::Null;
  }
  //--------------------------------
  void Selection::deleteAllFromDatabase(DataBase& pDatabase )
  {
    if( cSelectType == SelectType::Object )
      {
	std::vector<EntityPtr> lDelList;
	for(  EntityPtr lEntity : cSelectObjVect )
	  {
	    if( pDatabase.deleteEntity( lEntity ) )
	      {
		lDelList.push_back( lEntity );
	      }
	  }
		
	for( EntityPtr lEntity : lDelList )
	  {
	    pDatabase.deleteEntity( lEntity );
	  }
      }
    else 
    if( cSelectType == SelectType::Point )
      {
	std::vector<PointPtr> lPoints;
	
	for(  EntityPtr lEntity : cSelectObjVect )
	  {
	    lPoints.push_back( dynamic_cast<PointPtr>( lEntity) );
	  }
	Modif::DeletePoints( &pDatabase, lPoints, true );
      }
	
    cSelectObj.clear();
    cSelectObjVect.clear();
  }
  //--------------------------------
  void Selection::addSelectionToInput( DataBase& pDatabase, bool pFlagLink )				
  {
    for(  EntityPtr lEntity : cSelectObjVect )
      {
	pDatabase.addToInput( lEntity, pFlagLink );				
      }	
  }
  //--------------------------------
  /*
    void Selection::drawGL( DataBase& pDatabase, ViewProps& pViewProps )
    {
    if( cSelectType != SelectType::Poly )
    return;
    ViewProps lViewProps( pViewProps );
		
    lViewProps.cColorPoint = ColorRGBA( 1, 0.5, 0, 0 );
    lViewProps.cColorLine  = ColorRGBA( 1, 0.5, 0, 0 );
    lViewProps.cColorFacet = ColorRGBA( 1, 0.5, 0, 0 );

 
    glPushMatrix();
		
    //	cCurrentTransform.execGL();
    glLoadMatrixd( cCurrentMatrice.vectForGL() );
		
    for( auto lIter = cSelectObjVect.begin() ; lIter != cSelectObj.end(); ++lIter  )
    {
    //		std::cout << "        Selection::drawGL " << lSubSel.cObjId << std::endl;
    Entity* lObj =  (*lIter);
    if( lObj != nullptr )
    {
    std::cout << "<<<     call    drawSelectionGL" << std::endl;
    lObj->drawSelectionGL( lViewProps );
    }
    std::cout << ">>>     fin call    drawSelectionGL" << std::endl;

    }
    glPopMatrix();
    }
  */
  //******************************************

}
