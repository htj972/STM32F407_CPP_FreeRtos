#include "../Inc/sys.h"

void run_void(){;};
void run_void_data(u8 data){;};

//THUMBָ�֧�ֻ������
//�������·���ʵ��ִ�л��ָ��WFI
void WFI_SET(void)
{
    __ASM volatile("wfi");
}
//�ر������ж�
void INTX_DISABLE(void)
{
    __ASM volatile("cpsid i");
}
//���������ж�
void INTX_ENABLE(void)
{
    __ASM volatile("cpsie i");
}















