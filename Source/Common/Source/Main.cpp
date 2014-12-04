#include <iostream>
#include <Game.hpp>

int main( int p_Argc, char **p_ppArgv )
{
	std::cout << "LD 31 Game by Red Ring Rico" << std::endl;

	LD::Game TheGame;

	if( TheGame.Initialise( ) != LD_OK )
	{
		std::cout << "[7DLD::main] <ERROR> Something went wrong initialising "
			"the game" << std::endl;

		return LD_FAIL;
	}

	return TheGame.Execute( );
}

