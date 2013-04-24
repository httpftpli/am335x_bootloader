
#include "pf_mainloop.h"

extern  LOOP __loop_0;

#pragma section = "mainloop"
/*int main(void){
   unsigned int loop = (unsigned int )__section_begin("mainloop");
   unsigned int n = __section_size("mainloop")/4;  
   unsigned int i = 0;
   while (1) {      
      ((LOOP)(loop+i++<<2))();
      if (i==n) {
         i = 0;
      }
   }

}*/