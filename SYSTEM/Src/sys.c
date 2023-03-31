#include "../Inc/sys.h"

void run_void(){;};
void run_void_data(u8 data){;};

//THUMB指令不支持汇编内联
//采用如下方法实现执行汇编指令WFI
//void WFI_SET()
//{
//    __ASM volatile("wfi");
//}
////关闭所有中断
//void inter_DISABLE()
//{
//    __ASM volatile("cpsid i");
//}
////开启所有中断
//void inter_ENABLE()
//{
//    __ASM volatile("cpsie i");
//}















