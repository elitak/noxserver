#include "global.h"
#include "master.h"

int main(int argv, char* argc[])
{
	master::instance->run();

	return 0;
}