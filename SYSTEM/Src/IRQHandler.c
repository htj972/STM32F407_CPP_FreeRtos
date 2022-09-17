/**
* @Author kokirika
* @Name IRQHandler
* @Date 2022-09-15
**/
#include "IRQHandler.h"

_IRQ_STRUCT_ HARD_IQR;

/**************************/
//   USART_IRQ_Handle
/**************************/
void USART1_IRQHandler()  {              	//串口1中断服务程序
    u8 Res;
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET){
        Res =USART_ReceiveData(USART1);	//读取接收到的数据
        HARD_IQR.Usart_IRQ_link(0,Res);
        HARD_IQR.extern_IRQ_link(0,Res);
    }
    USART_ClearITPendingBit(USART1,USART_IT_RXNE);
}

void USART2_IRQHandler() {               	//串口2中断服务程序
    u8 Res;
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET){
        Res =USART_ReceiveData(USART2);	//读取接收到的数据
        HARD_IQR.Usart_IRQ_link(1,Res);
        HARD_IQR.extern_IRQ_link(1,Res);
    }
    USART_ClearFlag(USART2,USART_IT_RXNE);
}

void USART3_IRQHandler()  {              	//串口3中断服务程序
    u8 Res;
    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET){
        Res =USART_ReceiveData(USART3);	//读取接收到的数据
        HARD_IQR.Usart_IRQ_link(2,Res);
        HARD_IQR.extern_IRQ_link(2,Res);
    }
    USART_ClearFlag(USART3,USART_IT_RXNE);
}

void UART4_IRQHandler() {              	//串口4中断服务程序
    u8 Res;
    if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET){
        Res =USART_ReceiveData(UART4);	//读取接收到的数据
        HARD_IQR.Usart_IRQ_link(3,Res);
        HARD_IQR.extern_IRQ_link(3,Res);
    }
    USART_ClearFlag(UART4,USART_IT_RXNE);
}

void UART5_IRQHandler() {                	//串口5中断服务程序
    u8 Res;
    if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET){
        Res =USART_ReceiveData(UART5);	//读取接收到的数据
        HARD_IQR.Usart_IRQ_link(4,Res);
        HARD_IQR.extern_IRQ_link(4,Res);
    }
    USART_ClearFlag(UART5,USART_IT_RXNE);
}

void USART6_IRQHandler() {                	//串口6中断服务程序
    u8 Res;
    if(USART_GetITStatus(USART6, USART_IT_RXNE) != RESET){
        Res =USART_ReceiveData(USART6);	//读取接收到的数据
        HARD_IQR.Usart_IRQ_link(5,Res);
        HARD_IQR.extern_IRQ_link(5,Res);
    }
    USART_ClearFlag(USART6,USART_IT_RXNE);
}

/**************************/
//   EXIT_IRQ_Handle
/**************************/
_EXIT_STRUCT_ EXIT_IQR;
void EXTI0_IRQHandler(){                    //外部中断0
    if(EXTI_GetITStatus(EXTI_Line0)==SET){
        EXIT_IQR.EXIT_IRQ_link(0);
        EXIT_IQR.extern_IRQ_link(0);
        EXTI_ClearITPendingBit(EXTI_Line0);
    }
}

void EXTI1_IRQHandler(){                    //外部中断0
    if(EXTI_GetITStatus(EXTI_Line1)==SET){
        EXIT_IQR.EXIT_IRQ_link(1);
        EXIT_IQR.extern_IRQ_link(1);
        EXTI_ClearITPendingBit(EXTI_Line1);
    }
}

void EXTI2_IRQHandler(){                    //外部中断0
    if(EXTI_GetITStatus(EXTI_Line2)==SET){
        EXIT_IQR.EXIT_IRQ_link(2);
        EXIT_IQR.extern_IRQ_link(2);
        EXTI_ClearITPendingBit(EXTI_Line2);
    }
}

void EXTI3_IRQHandler(){                    //外部中断0
    if(EXTI_GetITStatus(EXTI_Line3)==SET){
        EXIT_IQR.EXIT_IRQ_link(3);
        EXIT_IQR.extern_IRQ_link(3);
        EXTI_ClearITPendingBit(EXTI_Line3);
    }
}
void EXTI4_IRQHandler(){                    //外部中断0
    if(EXTI_GetITStatus(EXTI_Line4)==SET){
        EXIT_IQR.EXIT_IRQ_link(4);
        EXIT_IQR.extern_IRQ_link(4);
        EXTI_ClearITPendingBit(EXTI_Line4);
    }
}

void EXTI9_5_IRQHandler(){                    //外部中断0
    if(EXTI_GetITStatus(EXTI_Line5)==SET){
        EXIT_IQR.EXIT_IRQ_link(5);
        EXIT_IQR.extern_IRQ_link(5);
        EXTI_ClearITPendingBit(EXTI_Line5);
    }
    if(EXTI_GetITStatus(EXTI_Line6)==SET){
        EXIT_IQR.EXIT_IRQ_link(6);
        EXIT_IQR.extern_IRQ_link(6);
        EXTI_ClearITPendingBit(EXTI_Line6);
    }
    if(EXTI_GetITStatus(EXTI_Line7)==SET){
        EXIT_IQR.EXIT_IRQ_link(7);
        EXIT_IQR.extern_IRQ_link(7);
        EXTI_ClearITPendingBit(EXTI_Line7);
    }
    if(EXTI_GetITStatus(EXTI_Line8)==SET){
        EXIT_IQR.EXIT_IRQ_link(8);
        EXIT_IQR.extern_IRQ_link(8);
        EXTI_ClearITPendingBit(EXTI_Line8);
    }
    if(EXTI_GetITStatus(EXTI_Line9)==SET){
        EXIT_IQR.EXIT_IRQ_link(9);
        EXIT_IQR.extern_IRQ_link(9);
        EXTI_ClearITPendingBit(EXTI_Line9);
    }
}

void EXTI15_10_IRQHandler(){                    //外部中断0
    if(EXTI_GetITStatus(EXTI_Line10)==SET){
        EXIT_IQR.EXIT_IRQ_link(10);
        EXIT_IQR.extern_IRQ_link(10);
        EXTI_ClearITPendingBit(EXTI_Line10);
    }
    if(EXTI_GetITStatus(EXTI_Line11)==SET){
        EXIT_IQR.EXIT_IRQ_link(11);
        EXIT_IQR.extern_IRQ_link(11);
        EXTI_ClearITPendingBit(EXTI_Line11);
    }
    if(EXTI_GetITStatus(EXTI_Line12)==SET){
        EXIT_IQR.EXIT_IRQ_link(12);
        EXIT_IQR.extern_IRQ_link(12);
        EXTI_ClearITPendingBit(EXTI_Line12);
    }
    if(EXTI_GetITStatus(EXTI_Line13)==SET){
        EXIT_IQR.EXIT_IRQ_link(13);
        EXIT_IQR.extern_IRQ_link(13);
        EXTI_ClearITPendingBit(EXTI_Line13);
    }
    if(EXTI_GetITStatus(EXTI_Line14)==SET){
        EXIT_IQR.EXIT_IRQ_link(14);
        EXIT_IQR.extern_IRQ_link(14);
        EXTI_ClearITPendingBit(EXTI_Line14);
    }
    if(EXTI_GetITStatus(EXTI_Line15)==SET){
        EXIT_IQR.EXIT_IRQ_link(15);
        EXIT_IQR.extern_IRQ_link(15);
        EXTI_ClearITPendingBit(EXTI_Line15);
    }
}