#include <stdio.h>
#include <msmouse.h>
#include <stdlib.h>
#include <time.h>

int main()
{
	if (msm_init() == -1)
	{
		printf("Mouse initialization succeeded\n");
		msm_showcursor();
		while (1)
		{
			int status;
			unsigned x, y;
			msm_showcursor();
			status = msm_getstatus(& x, &y);
			if ( status & LEFT_BUTTON || status & RIGHT_BUTTON )
			{
				break;
			}
			else
			{
				//printf("x = %u, y = %u\n", x, y);
			}
			usleep(20);
		}
		msm_term();
	}
	else
	{
		printf("Mouse initialization failed\n");
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
