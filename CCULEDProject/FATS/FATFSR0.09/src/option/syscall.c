/*
*********************************************************************************************************
*
*	模块名称 : fatfs和rtos的接口文件
*	文件名称 : syscall.c
*	版    本 : V1.0
*	说    明 : Sample code of OS dependent controls for FatFs
*              这个文件是作者提供的，这里为其加入uCOS-III的支持，使用前需要在
*              ffconf.h文件中加入头文件os.h
*
*	修改记录 :
*		版本号   日期         作者           说明
*       v1.0    2014-06-19   Eric2013        首发
*
*	Copyright (C), 2014-2015, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#include <stdlib.h>		/* ANSI memory controls */
#include "ff.h"


#if _FS_REENTRANT

OS_MUTEX   FILE_MUTEX;	   /* 用于互斥 */

/*-------------------------------------------------------------------------------------------*/
/* Create a Synchronization Object                                                           */
/*-------------------------------------------------------------------------------------------*/
/* 
   This function is called by f_mount() function to create a new
   synchronization object, such as semaphore and mutex. When a 0 is
   returned, the f_mount() function fails with FR_INT_ERR.
*/
int ff_cre_syncobj (	/* 1:Function succeeded, 0:Could not create due to any error */
	BYTE vol,			/* Corresponding logical drive being processed */
	_SYNC_t* sobj		/* Pointer to return the created sync object */
)
{
	OS_ERR     err;
	int ret;
	
	OSMutexCreate ((OS_MUTEX  *)&FILE_MUTEX,   /* uC/OS-III */
                   (CPU_CHAR  *)"FILE_MUTEX",
                   (OS_ERR    *)&err);
	
    *sobj = FILE_MUTEX;
	
	ret = (int)(err == OS_ERR_NONE);
	
//	*sobj = CreateMutex(NULL, FALSE, NULL);		/* Win32 */
//	ret = (int)(*sobj != INVALID_HANDLE_VALUE);

//	*sobj = SyncObjects[vol];		            /* uITRON (give a static created semaphore) */
//	ret = 1;

//	*sobj = OSMutexCreate(0, &err);	            /* uC/OS-II */
//	ret = (int)(err == OS_NO_ERR);

//  *sobj = xSemaphoreCreateMutex();	        /* FreeRTOS */
//	ret = (int)(*sobj != NULL);
	
	return ret;
}

/*-------------------------------------------------------------------------------------------*/
/* Delete a Synchronization Object                                                           */
/*-------------------------------------------------------------------------------------------*/
/* 
  This function is called in f_mount() function to delete a synchronization
  object that created with ff_cre_syncobj() function. When a 0 is
  returned, the f_mount() function fails with FR_INT_ERR.
*/
int ff_del_syncobj (	/* 1:Function succeeded, 0:Could not delete due to any error */
	_SYNC_t sobj		/* Sync object tied to the logical drive to be deleted */
)
{
	OS_ERR     err;
	int ret;

	OSMutexDel ((OS_MUTEX  *)&sobj,
                (OS_OPT     )OS_OPT_DEL_ALWAYS,
                (OS_ERR    *)&err);
	
	ret = (int)(err == OS_ERR_NONE);

//	ret = CloseHandle(sobj);	             /* Win32 */

//	ret = 1;					             /* uITRON (nothing to do) */

//	OSMutexDel(sobj, OS_DEL_ALWAYS, &err);	 /* uC/OS-II */
//	ret = (int)(err == OS_NO_ERR);

//  xSemaphoreDelete(sobj);		             /* FreeRTOS */
//	ret = 1;

	return ret;
}

/*-------------------------------------------------------------------------------------------*/
/* Request Grant to Access the Volume                                                        */
/*-------------------------------------------------------------------------------------------*/
/* This function is called on entering file functions to lock the volume.
/  When a FALSE is returned, the file function fails with FR_TIMEOUT.
*/

int ff_req_grant (	/* TRUE:Got a grant to access the volume, FALSE:Could not get a grant */
	_SYNC_t sobj	/* Sync object to wait */
)
{
	OS_ERR     err;
	int ret;
	
	OSMutexPend ((OS_MUTEX  *)&sobj,
                 (OS_TICK    )_FS_TIMEOUT,
                 (OS_OPT     )OS_OPT_PEND_BLOCKING,
                 (CPU_TS     )0,
                 (OS_ERR    *)&err);	

//	ret = (int)(WaitForSingleObject(sobj, _FS_TIMEOUT) == WAIT_OBJECT_0);	/* Win32 */

//	ret = (int)(wai_sem(sobj) == E_OK);			/* uITRON */

//	OSMutexPend(sobj, _FS_TIMEOUT, &err));		/* uC/OS-II */
//	ret = (int)(err == OS_NO_ERR);

//	ret = (int)(xSemaphoreTake(sobj, _FS_TIMEOUT) == pdTRUE);	/* FreeRTOS */

	return ret;
}



/*-------------------------------------------------------------------------------------------*/
/* Release Grant to Access the Volume                                                        */
/*-------------------------------------------------------------------------------------------*/
/* 
  This function is called on leaving file functions to unlock the volume.
*/
void ff_rel_grant (
	_SYNC_t sobj	/* Sync object to be signaled */
)
{
	OS_ERR  err;

	OSMutexPost ((OS_MUTEX  *)&sobj,
                 (OS_OPT     )OS_OPT_POST_1,
                 (OS_ERR    *)&err);
	
//	ReleaseMutex(sobj);		/* Win32 */

//	sig_sem(sobj);			/* uITRON */

//	OSMutexPost(sobj);		/* uC/OS-II */

//	xSemaphoreGive(sobj);	/* FreeRTOS */
}

#endif


#if _USE_LFN == 3	/* LFN with a working buffer on the heap */
/*-------------------------------------------------------------------------------------------*/
/* Allocate a memory block                                                                   */
/*-------------------------------------------------------------------------------------------*/
/* If a NULL is returned, the file function fails with FR_NOT_ENOUGH_CORE.
*/

void* ff_memalloc (	/* Returns pointer to the allocated memory block */
	UINT msize		/* Number of bytes to allocate */
)
{
	return malloc(msize);
}

/*-------------------------------------------------------------------------------------------*/
/* Free a memory block                                                                       */
/*-------------------------------------------------------------------------------------------*/

void ff_memfree (
	void* mblock	/* Pointer to the memory block to free */
)
{
	free(mblock);
}

#endif
