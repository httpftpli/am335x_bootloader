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


//copy file 
    FATFS fs1,fs2;
    unsigned char buf[512];
    f_mount(0, &fs1);
    f_mount(3, &fs2);

   f_copy("3:/MLO", "0:/MLO", buf, sizeof buf);


    I2CInit(SOC_I2C_1_REGS,400000,0,0);
   //Rx8025Init();
   unsigned char h,m,s;
   Rx8025GetTime(&h,&m,&s);


   CAN_FRAME canFrame;
   canFrame.id = 0x512<<18;
   canFrame.xtd = 0;
   canFrame.data[0] = 0;
   canFrame.data[1] = 0x04;
   canFrame.dlc = 8;
   canFrame.dir = 0;
   CANRegistRcvedHandler(canrcvhandler);
   CANInit(SOC_DCAN_0_REGS,CAN_MODE_NORMAL, DCAN_IN_CLK,1000000);
   while (1) {
      CANSend_noblock(SOC_DCAN_0_REGS,&canFrame);
      while (ack == 0);
      ack = 0;
   }


   QEPInit(SOC_EQEP_2_REGS,100*1000*1000,QEP_MODE_QUAD);
   QEPSetPosFactor(1);
   QEPSetPosCompare(SOC_EQEP_2_REGS,3200);
   QEPVelocityDetectStart(SOC_EQEP_2_REGS,5*1000*1000);  


   TimerTickConfigure();
   TimerTickStart();


   unsigned char a[100] = "上海福建有震感";
   WCHAR b[128],c;
   c = strLen_UTF8(a);
   UTF8toUCS2_string(a, b, 100);


   MMCSDP_CtrlInfoInit(& mmcsdctr[0], SOC_MMCHS_0_REGS, 96000000, 24000000, MMCSD_BUSWIDTH_4BIT, 0,
                       &card0, HSMMCSDXferSetup,
                       HSMMCSDCmdStatusGet, HSMMCSDXferStatusGet);
   MMCSDP_CtrlInfoInit(& mmcsdctr[1], SOC_MMCHS_1_REGS, 96000000, 24000000, MMCSD_BUSWIDTH_4BIT, 0,
                       &card1, HSMMCSDXferSetup,
                       HSMMCSDCmdStatusGet, HSMMCSDXferStatusGet);
   MMCSDP_CtrlInit(& mmcsdctr[0]);
   MMCSDP_CtrlInit(& mmcsdctr[1]);

   while (1) {
    usbMscProcess();
    if(USBMSC_DEVICE_READY == g_usbMscState) {
      FIL file;
      unsigned char res = f_open(&file,"2:/YS_SinkT.ARA",FA_OPEN_EXISTING |FA_READ);
    }
   return 0;


//mmc test
   MMCSDP_CtrlInfoInit(& mmcsdctr[0], SOC_MMCHS_0_REGS, 96000000, 24000000, MMCSD_BUSWIDTH_4BIT, 0,
                       &card0, HSMMCSDXferSetup,
                       HSMMCSDCmdStatusGet, HSMMCSDXferStatusGet);
   MMCSDP_CtrlInit(& mmcsdctr[0]);
   MMCSDP_CardInit(mmcsdctr,MMCSD_CARD_AUTO);
   

   for (int i=0;i<sizeof(bufmmc1)/4;i++) {
      bufmmc1[i]=0x55555555;
   }
   for (int i=0;i<sizeof(bufmmc2)/4;i++) {
      bufmmc2[i]=0;
   }
   
   MMCSDP_Write(mmcsdctr,bufmmc1,0,1024);
   MMCSDP_Read(mmcsdctr,bufmmc2,0,1024);
   for(int i=0;i<512*1024/4;i++){
     if(bufmmc2[i]!=bufmmc1[1])
        while(1);
   } 



   //mmc test finish

//DDR TEST
      unsigned int *p = (unsigned int *)(0x80000000 + 32 * 1024 * 1024);

   for (int i = 0; i < (256 * 1024 * 1024 - 32 * 1024 * 1024) / 4; i++) {
      p[i] = 0x55555555;
      if (p[i] != 0x55555555)
          while (1);
   }
   for (int i = 0; i < (256 * 1024 * 1024 - 32 * 1024 * 1024) / 4; i++) {
      p[i] = 0xaaaaaaaa;
      if (p[i] != 0xaaaaaaaa)
          while (1);
   }

   //DDR TEST FINISH
