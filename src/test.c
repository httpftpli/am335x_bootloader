
void DDRTest(){
  unsigned int *p = (unsigned int *)(0x80000000);
  for (int i = 0; i < 256 * 1024 * 1024  / 4; i++) {
      p[i] = 0x55555555;
      if (p[i] != 0x55555555){
           //UARTPuts("ddr error \n\r ", -1);
          while (1); 
      }
   }
   //UARTPuts("ddr normal \n\r ", -1);
   for (int i = 0; i < 256 * 1024 * 1024  / 4; i++) {
      p[i] = 0xaaaaaaaa;
      if (p[i] != 0xaaaaaaaa) {
         //UARTPuts("ddr error \n\r ", -1);
          while (1);          
      }
   }
   //UARTPuts("ddr normal \n\r ", -1);      
   } 