/*
 * timeutils.c
 *  c/c++ program:1350354127212
    Java program  :1350354129299

    ﻿﻿﻿﻿﻿﻿public static void main(String[] args){
   	   System.out.println("java program :" + System.currentTimeMillis());
	}﻿﻿﻿

 *  Created on: 2017-3-16
 *      Author: yiqin
 */
#include<stdlib.h>

long getCurrentTime()
{
   struct timeval tv;
   gettimeofday(&tv,NULL);
   return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}
