#include <stdio.h>
#include <sockios.h>
int main()
{
	char str[10];
	int ret = snprintf(str,5,"hello%s","world11");
	/**
	 * 12,hell
	 */
	printf("%d,%s",ret, str);
	return 0;
}

