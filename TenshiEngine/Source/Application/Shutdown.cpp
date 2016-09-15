
namespace App{
	bool exit = false;
	void Shutdown(){
		exit = true;
	}
	bool IsShutdown(){
		return exit;
	}
}