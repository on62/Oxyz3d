#include "Modif.h"


#include <set>

namespace PP3d{

  //**************************************************
  bool Modif::SubdivideFacet(  std::vector<FacetPtr>&  iVect, DataBase * iBase, SubDiv::SubParam * iSubDiv )
  {  
    DBG_MODIF( "Modif::SubdivideFacet Nb : " << iVect.size() );
    
    for( FacetPtr lFac : iVect )
      {
	PolyPtr lPoly = (PolyPtr)lFac->firstOwner();
	if( lPoly == nullptr
	    || lPoly->getType() != ShapeType::Poly )
	  {
	    std::cerr << "Modif::SubdivideFacet not a polyedre" << std::endl;
	    continue;
	  }
	
	int lDepth = 1;
	SubDiv::SubParam lSubDivLocal( 1, 1, false, SubDiv::SubNormalizeType::NORMALIZE_NONE );
	
	if( iSubDiv == nullptr )
	  {	   
	    iSubDiv = & lSubDivLocal;	
	  }
	else
	  {
	    iSubDiv->clear(); // !!!!!!!!!
	  }
    
	PP3d::SortEntityVisitorPointLine lVisit;
	lFac->execVisitor( lVisit );

	std::vector<PointPtr> & lPts   = lVisit.cVectPoints;
	
	SubDiv::SubdivideN( *iSubDiv, lPts, lDepth);


	

	DBG_MODIF( "Modif::SubdivideFacet New Facet : "
		   <<  iSubDiv->cFacets.size()
		   << " Pts :" << iSubDiv->cPoints.size() );


	
	if(  iSubDiv->cPoints.size()  && iSubDiv->cFacets.size() > 1 )
	  {	
	    std::vector<PointPtr> & lOldPts   = lVisit.cVectPoints;
	    std::vector<LinePtr>  & lOldLines = lVisit.cVectLines;


	    // On va reutiliser les anciens points et les anciennes lignes de la facette

	    
	    
	    // On enleve des anciens points les liens vers les vielles lignes
	    for( PointPtr lOldPt :  lOldPts)
	      {
		for( LinePtr lOldLine : lOldLines )
		  {
		    lOldPt->removeOwner( lOldLine );
		  }
	      }
	  

		
	    std::vector<PointPtr> lNewPoints( iSubDiv->cPoints.size() );
	    //=========================
	    for( size_t p =0; p< iSubDiv->cPoints.size(); p++)
	      {
		PointPtr lSearchPt = Modif::FindPointByExactValues( lOldPts, iSubDiv->cPoints[p] );
		if( lSearchPt != nullptr )
		  {
		    DBG_MODIF( "Find Old ==  " << p  );
		    lNewPoints[p] = lSearchPt; // xyz sont forcement bonnes!
		  }
		else
		  {
		    lNewPoints[p] = new Point( iSubDiv->cPoints[p] ); 
		    iBase->validOneEntityLevel(lNewPoints[p]);        // on donne un id et on stocke
		  }
	      }
	    //=========================	    

	    size_t lNbOldLineUse=0; // pour compter le recyclage des anciennes lignes
	    
	    // on recycle l'ancienne facette
	    lFac->getLines().clear();
	    Facet* lCurFacet = lFac;
	    
	    //=========================	    
	    for( size_t f=0; f< iSubDiv->cFacets.size() ; f++ ) 
	      {
		PrimFacet & lPrimFacet  = *(iSubDiv->cFacets[f]); // juste pour simplifier le code
		
		if( lCurFacet == nullptr)
		  {
		    lCurFacet = new Facet();                // nouvelle facette vide sans id
		    iBase->validOneEntityLevel(lCurFacet);
		    lPoly->addFacet( lCurFacet );
		  }

		uint i=0;
		for( ; i< lPrimFacet.size()-1; i++ ) // pour les points de la facette courante
		  {
		    LinePtr lLine = nullptr;		    
		    if( lNbOldLineUse < lOldLines.size() )
		      {
			lLine = lOldLines[lNbOldLineUse++];
			lLine->removeOwner( lFac );
			lLine->set( lNewPoints[ lPrimFacet[i]] , lNewPoints[ lPrimFacet[i+1]] );
		      }
		      else
		      {
			lLine = new Line( lNewPoints[ lPrimFacet[i]] , lNewPoints[ lPrimFacet[i+1]] );
			iBase->validOneEntityLevel(lLine);
		      }
		    lCurFacet->addLine( lLine );
		  }
		LinePtr lLine = new Line( lNewPoints[ lPrimFacet[i] ] , lNewPoints[ lPrimFacet[0] ] );
		iBase->validOneEntityLevel(lLine);		
		lCurFacet->addLine( lLine );
		
		lCurFacet = nullptr; 
	      }
	    //=========================
	  }
	
	
      }

    return true;
  } 
  //**************************************************
} // end namespace
