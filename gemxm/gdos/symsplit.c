
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char buf[120];

int main(void)
{
	while (fgets(buf, sizeof(buf), stdin))
	{
		char *s;
		for (s = strtok(buf, "\t\n");
		     s != NULL;
		     s = strtok(NULL, "\t\n"))
		{
			printf("%s\n", s);
		}	
	}
	return 0;
}
