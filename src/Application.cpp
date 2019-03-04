#include "Application.h"


#include "Win3d.h"
#include "WinObjTree.h"

#include "Shape/DataBase.h"

#include <sstream>
#include <ostream>



namespace M3d{

	Application* Application::sTheAppli = nullptr;

	//************************************
	Application::Application( )	

	  :cuDatabase( std::unique_ptr<PP3d::DataBase>( new PP3d::DataBase() ))
		, cCurrentTransform( Transform::Nothing)
	{
		std::cout << "========= Application::Application" << std::endl;
	}
	//-----------------------------------
	Application::~Application()
	{
	}
	//-----------------------------------
	//-----------------------------------
	//-----------------------------------
 	int Application::init( int argc, char* argv[] )
	{
		return 0;
	}
	//-----------------------------------
	Win3d & Application::createNewWin3d( int pW, int pH )
	{
		static int lNumWin=1;

		std::stringstream lStr;

		lStr << "Oxyz3d " << lNumWin++;

		
		std::unique_ptr<Win3d> luWin3d( new Win3d(  lStr.str().c_str(), pW, pH, *(cuDatabase.get()) ));

		Win3d* lReturn =luWin3d.get();

		cAllWin3d.push_back(  std::unique_ptr<Win3d>(luWin3d.release()) );
		
		return *lReturn;
	
	}
	//-----------------------------------	
	void Application::redrawAllCanvas3d()
	{
		for( std::unique_ptr<Win3d> &lWin : cAllWin3d )
			{
				lWin->canvasRedraw();
			}
	}
	//-----------------------------------	
	void Application::setCursorPosition( PP3d::Point3d& pPos)
	{
		for( std::unique_ptr<Win3d> &lWin : cAllWin3d )
			{
				lWin->setCursorPosition(pPos);
			}
	}
	//----------------------------------------
	void Application::createObjectTree()
	{
		WinObjTree::Instance().show();
		redrawObjectTree();
	}
	//----------------------------------------
	void Application::redrawObjectTree()
	{
		WinObjTree::Instance().rebuild();		
	}
	//************************************
};
