#include "ChatClient/Chat.h"
#include "CollisionCrisis/Crisis.h"
#include "Exploring/Exploring.h"

int main()
{
    short what_program = 1;

	if (what_program == 0)
	{
		Chat chat;
	}
	else if (what_program == 1)
	{
		Crisis crisis;
	}
	else if (what_program == 2)
	{
		Exploring exploring;
	}
	else if (what_program == 3)
	{
		// game of life
	}
	else if (what_program == 4)
	{
		// memory & cache Optimization
	}

	return 0;
}
