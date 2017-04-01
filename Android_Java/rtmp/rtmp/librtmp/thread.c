/*  Thread compatibility glue
 *  Copyright (C) 2009 Howard Chu
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with RTMPDump; see the file COPYING.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA  02110-1301, USA.
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

#include "thread.h"
#include "log.h"


pthread_t
ThreadCreate(thrfunc *routine, void *args)
{
  pthread_t id = 0;
  pthread_attr_t attributes;
  int ret;

  pthread_attr_init(&attributes);
  pthread_attr_setdetachstate(&attributes, PTHREAD_CREATE_DETACHED);

  ret =
    pthread_create(&id, &attributes, routine, args);
  if (ret != 0)
    RTMP_LogPrintf("%s, pthread_create failed with %d\n", __FUNCTION__, ret);

  return id;
}

//初始化互斥锁
int rtmp_mutex_init( pthread_mutex_t *p_mutex )
{
    pthread_mutexattr_t attr;

    if (pthread_mutexattr_init (&attr))
    {
		RTMP_LogPrintf(" pthread_mutexattr_init() fail \n ");
		return 0;
	}
        

    pthread_mutexattr_settype (&attr, PTHREAD_MUTEX_DEFAULT);

    if (pthread_mutex_init (p_mutex, &attr))
    {
		RTMP_LogPrintf(" pthread_mutex_init() fail \n");
		return 0;
	}
        
    pthread_mutexattr_destroy( &attr );
	return 1;
	
}

//销毁互斥锁
int rtmp_mutex_destroy (pthread_mutex_t *p_mutex)
{
    int val = pthread_mutex_destroy( p_mutex );
    if(val)
    {
		RTMP_LogPrintf(" rtmp_mutex_destroy() fail \n");
		return 0;
	}

	return 1;
}



