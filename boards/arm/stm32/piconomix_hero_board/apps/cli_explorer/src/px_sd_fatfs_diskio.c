/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */
#include "px_sd.h"
#if 0
#include "px_rtc.h"
#endif

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
    uint16_t status;
	if(!px_sd_get_status(&status))
    {
        return STA_NOINIT;
    }
    if(status != 0)
    {
        return STA_NOINIT;
    }
    else
    {
        return 0;
    }
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
    if(!px_sd_reset())
    {
        return STA_NOINIT;
    }
    else
    {
        return 0;
    }
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
    if(px_sd_read_blocks(buff, sector, count) == count)
    {
        return RES_OK;
    }
    else
    {
        return RES_ERROR;
    }
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
    if(px_sd_write_blocks(buff, sector, count) == count)
    {
        return RES_OK;
    }
    else
    {
        return RES_ERROR;
    }
}



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	switch(cmd)
    {
    case CTRL_SYNC :        /* Make sure that no pending write process */
        return RES_OK;

    case GET_SECTOR_COUNT : /* Get number of sectors on the disk (DWORD) */
        {
            px_sd_csd_t csd;
            if(!px_sd_read_csd(&csd))
            {
                return RES_ERROR;
            }
            *(uint32_t * )buff = px_sd_get_capacity_in_blocks(&csd);
            return RES_OK;
        }

    case GET_BLOCK_SIZE :   /* Get erase block size in unit of sector (DWORD) */
        *(DWORD * )buff = PX_SD_BLOCK_SIZE / sizeof(uint32_t);
        return RES_OK;

    default:
        return RES_PARERR;
    }
}

DWORD get_fattime(void)
{
#if 0
    rtc_time_t time;
    rtc_get_time(&time);

    return	 ((DWORD)(2012 - 1980 + time.year) << 25)
           | ((DWORD)(time.month) << 21)
           | ((DWORD)(time.day) << 16)
           | ((DWORD)(time.hour) << 11)
           | ((DWORD)(time.min) << 5)
           | ((DWORD)(time.sec) >> 1);
#else
    return 0;
#endif
}
