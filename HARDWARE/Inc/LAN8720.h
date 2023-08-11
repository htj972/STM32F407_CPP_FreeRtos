/**
* @Author kokirika
* @Name LAN8720
* @Date 2023-6-2
**/

#ifndef KOKIRIKA_LAN8720_H
#define KOKIRIKA_LAN8720_H

#include "sys.h"
#include "stm32f4x7_eth.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LAN8720_PHY_ADDRESS  	0x00				//LAN8720 PHY芯片地址.
#define PHY_REG_BSR 0x01
#define LAN8720_RST_PORT 		GPIOA 			//LAN8720复位引脚
#define LAN8720_RST_GPIO 		Pin_6 			//LAN8720复位引脚
#define LAN8720_LINKED          0x01
#define LAN8720_UNLINK          0x00

extern __attribute__((aligned(4))) ETH_DMADESCTypeDef *DMARxDscrTab;			//以太网DMA接收描述符数据结构体指针
extern __attribute__((aligned(4))) ETH_DMADESCTypeDef *DMATxDscrTab;			//以太网DMA发送描述符数据结构体指针
extern __attribute__((aligned(4))) uint8_t *Rx_Buff; 							//以太网底层驱动接收buffers指针
extern __attribute__((aligned(4))) uint8_t *Tx_Buff; 							//以太网底层驱动发送buffers指针
extern ETH_DMADESCTypeDef  *DMATxDescToSet;			//DMA发送描述符追踪指针
extern ETH_DMADESCTypeDef  *DMARxDescToGet; 		//DMA接收描述符追踪指针
extern ETH_DMA_Rx_Frame_infos *DMA_RX_FRAME_infos;	//DMA最后接收到的帧信息指针


uint8_t LAN8720_Init(void);
uint8_t LAN8720_Get_Speed(void);
uint8_t ETH_MACDMA_Config(void);
FrameTypeDef ETH_Rx_Packet(void);
uint8_t ETH_Tx_Packet(u16 FrameLength);
uint32_t ETH_GetCurrentTxBuffer(void);
uint8_t ETH_Mem_Malloc(void);
void ETH_Mem_Free(void);
uint8_t CheckLinkStatus();



#ifdef __cplusplus
}
#endif


#endif //KOKIRIKA_LAN8720_H
