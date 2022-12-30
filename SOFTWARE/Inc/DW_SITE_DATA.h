/**
* @Author kokirika
* @Name DW_SITE_DATA
* @Date 2022-12-15
**/

#ifndef KOKIRIKA_DW_SITE_DATA_H
#define KOKIRIKA_DW_SITE_DATA_H

#include "DW_DIS.h"

#define text_address    0X1000
#define  point_address  0x2000
#define  key_address    0X3000
#define  Data_address   0X3100
#define  pic_address    0X3300

#define TEXT_interval 0x40
#define PIC_interval  0x20
#define Value_interval 0x04

#define TEXT_ADD(n)   (text_address+TEXT_interval*((n)-1))
#define PIC_ADD(n)   (pic_address+PIC_interval*((n)-1))


Check_Box Samp_mode_l(PIC_ADD(1),6,35,238,71,270);
Check_Box Samp_mode_s(PIC_ADD(1),6,35,306,71,340);

Check_Box stove_work_ON(PIC_ADD(1),10,640,80,732,128);
Check_Box flow_work_ON(PIC_ADD(2),10,228,134,313,191);

#endif //KOKIRIKA_DW_SITE_DATA_H
