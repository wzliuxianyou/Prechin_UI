/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-27     SummerGift   add spi flash port file
 */

#include <rtthread.h>
#include <fal.h>
#include <dfs_fs.h>
#include <dfs_posix.h>


#include <drv_log.h>
#if defined(RT_USING_DFS)

#define FS_PARTITION_NAME  "filesystem"

// 挂载整个flash设备："norflash0":挂载的设备名称，"/":挂载路径，这里挂载到跟目录下
// 挂载flash设备的其中一个fal分区：需要先在该分区创建块设备
int rt_filesystem_init(void)
{
	struct rt_device *flash_dev = RT_NULL;
	int ret = -1;
	fal_init();
	flash_dev = fal_blk_device_create(FS_PARTITION_NAME);
	if(flash_dev == NULL)
	{
		LOG_E("Can't create a block device on %s partition.\n", FS_PARTITION_NAME);	
		return -1;
	}else{		
		LOG_D("Create a block device on %s partition successful.\n", FS_PARTITION_NAME);			
	}

	if (dfs_mount(flash_dev->parent.name, "/", "elm", 0, 0) == 0)  
	{
		LOG_D("file system initialization done!\n");
	}
	else
	{
		if(dfs_mkfs("elm", flash_dev->parent.name) == 0) // 如果是第一次挂载文件系统必须要先格式化
		{
			if (dfs_mount(flash_dev->parent.name, "/", "elm", 0, 0) == 0)
			{
				LOG_D("file system initialization done!\n");
			}
			else
			{
				LOG_E("file system initialization failed!\n");
				return -1;
			}
		}else {
			LOG_E("file system mkfs failed!\n");	
			return -1;
		}
	}
	DIR *dirP = opendir("/sdcard");
	if(dirP == RT_NULL)
	{
		ret = mkdir("/sdcard", 0777);
		if(ret < 0)
		{
			LOG_E("dir error\n");
			return -1;
		}else{
			LOG_I("sdcard mkdir ok\n");
		}
	}else{
		LOG_I("sdcard dir is exist\n");
		closedir(dirP);
	}
	if (dfs_mount("sd0", "/sdcard", "elm", 0, 0) == 0)  
	{
		LOG_I("sd mount fs system success\n");
	}else{
#if 0		
		if(dfs_mkfs("elm", "sd0") == 0) // 如果是第一次挂载文件系统必须要先格式化
		{
			if (dfs_mount("sd0", "/", "elm", 0, 0) == 0)
			{
				LOG_D("sdcard initialization done!\n");
			}
			else
			{
				LOG_E("sdcard initialization failed!\n");
				return -1;
			}
		}else {
			LOG_E("file system mkfs failed!\n");	
			return -1;
		}
#endif
	}	
	return 0;
}
INIT_ENV_EXPORT(rt_filesystem_init);


void fileSystem_unmount(void)
{
    char *fullpath = NULL;

    fullpath = dfs_normalize_path(NULL, "/");
    if (dfs_unmount(fullpath) == 0)
    {
        rt_kprintf("umount filesystem Success\n");
    }
    else
    {
        rt_kprintf("umount filesystem fail!\n");
    }
}
MSH_CMD_EXPORT(fileSystem_unmount, mount_to_usb);

void fileSystem_mount(void)
{
    rt_device_t dev;

    dev = rt_device_find(FS_PARTITION_NAME);
    if (dev != RT_NULL && dev->open_flag != 0)
    {
        rt_device_close(dev);
        rt_kprintf("close usb device ok!\n");
    }
    if (dfs_mount(FS_PARTITION_NAME, "/", "elm", 0, 0) == 0)
    {
        rt_kprintf("mount_to_flash ok!\n");
    }
    else
    {
        rt_kprintf("mount_to_flash error!!\n");
    }
}
MSH_CMD_EXPORT(fileSystem_mount, mount_to_flash);
#endif

