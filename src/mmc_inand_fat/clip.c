//fdisk 

    DWORD plist[] = {100, 0, 0, 0};  /* Divide drive into two partitions */
    BYTE work[_MAX_SS];
    f_fdisk(0, plist, work);  /* Divide physical drive 0 */
    f_mount(0, &Fatfs);
    f_mkfs(0, 0, 0);          /* Create an FAT volume on the logical drive 0. 2nd argument is ignored. */
    f_mount(0, 0);

//format partition
    FATFS fs;
    f_mount(0,  &fs);
    status = f_mkfs(0,0,4*512);
    if (status == 0) {
       mdDebug("driver 1 format success \r\n");
    }else{
       mdDebug("driver 1 format fail \r\n");
    }
//copy fist sector
    status = MMCSDP_CardInit(&mmcsdctr[0], MMCSD_CARD_AUTO);
    if (status == 0) {
       return  0;
    }
    status = MMCSDP_CardInit(&mmcsdctr[1], MMCSD_CARD_AUTO);
    if (status == 0) {
       return  0;
    }
    unsigned char buf[512];
    status = MMCSDP_Read(&mmcsdctr[0], buf, 0, 1);
    if (status == 0) {
       return  0;
    }
    status = MMCSDP_Write(&mmcsdctr[1],buf,0,1);
    if (status == 0) {
       return  0;
    }
    return  0;


//scan dir

