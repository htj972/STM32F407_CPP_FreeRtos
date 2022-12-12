/**
* @Author kokirika
* @Name TMC220XUART_H
* @Date 2022-11-29
**/

#include <cmath>
#include "TMC220xUart.h"
#include "delay.h"



//uint8_t GCONF = 0x00;
//uint8_t GSTAT = 0x01;
//uint8_t IFCNT = 0x02;
//uint8_t IOIN = 0x06;
//uint8_t IHOLD_IRUN = 0x10;
//uint8_t TSTEP = 0x12;
//uint8_t VACTUAL = 0x22;
//uint8_t TCOOLTHRS = 0x14;
//uint8_t SGTHRS = 0x40;
//uint8_t SG_RESULT = 0x41;
//uint8_t MSCNT = 0x6A;
//uint8_t CHOPCONF = 0x6C;
//uint8_t DRVSTATUS = 0x6F;

////GCONF
//uint8_t i_scale_analog = 1 << 0;
//uint8_t internal_rsense = 1 << 1;
//uint8_t en_spreadcycle = 1 << 2;
//uint8_t shaft = 1 << 3;
//uint8_t index_otpw = 1 << 4;
//uint8_t index_step = 1 << 5;
//uint8_t pdn_disable = 1 << 6;
//uint8_t mstep_reg_select = 1 << 7;
//
////GSTAT
//uint8_t reset = 1 << 0;
//uint8_t drv_err = 1 << 1;
//uint8_t uv_cp = 1 << 2;
//
////CHOPCONF
//uint32_t vsense = 1 << 17;
//uint32_t msres0 = 1 << 24;
//uint32_t msres1 = 1 << 25;
//uint32_t msres2 = 1 << 26;
//uint32_t msres3 = 1 << 27;
//uint32_t intpol = 1 << 28;
//
////IOIN
//uint8_t io_enn = 1 << 0;
//uint8_t io_step = 1 << 7;
//uint16_t io_spread = 1 << 8;
//uint16_t io_dir = 1 << 9;
//
////DRVSTATUS
//uint32_t stst = 1 << 31;
//uint32_t stealth = 1 << 30;
//uint32_t cs_actual = 31 << 16;
//uint16_t t157 = 1 << 11;
//uint16_t t150 = 1 << 10;
//uint16_t t143 = 1 << 9;
//uint16_t t120 = 1 << 8;
//uint8_t olb = 1 << 7;
//uint8_t ola = 1 << 6;
//uint8_t s2vsb = 1 << 5;
//uint8_t s2vsa = 1 << 4;
//uint8_t s2gb = 1 << 3;
//uint8_t s2ga = 1 << 2;
//uint8_t ot = 1 << 1;
//uint8_t otpw = 1 << 0;
//
////IHOLD_IRUN
//uint8_t ihold = 31 << 0;
//uint32_t irun = 31 << 8;
//uint32_t iholddelay = 15 << 16;
//
////SGTHRS
//uint8_t sgthrs = 255 << 0;

// ˳ʱ����ת
uint8_t dir_right[8] = {0x05, 0x00, 0x80, 0x00, 0x00, 0x00, 0x89};
// ��ʱ����ת
uint8_t dir_left[8] = {0x05, 0x00, 0x80, 0x00, 0x00, 0x00, 0x81};
/*!
 * ���ڷ���λ�ò���
 * @param uartx ����ָ��
 * @param STEP_Pin  ��������
 * @param EN_Pin    ʹ������
 * @param DIAG_Pin  �ϱ����� �ж�����
 */
TMC220xUart::TMC220xUart(_USART_ *uartx, GPIO_TypeDef *STEP_Port, uint16_t STEP_Pin, GPIO_TypeDef *EN_Port,
                         uint16_t EN_Pin, GPIO_TypeDef *DIAG_Port, uint16_t DIAG_Pin) {
    this->uart=uartx;
    this->STEP.init(STEP_Port,STEP_Pin,HIGH);
    this->EN.init(EN_Port,EN_Pin,HIGH);
    this->DIAG.init(DIAG_Port,DIAG_Pin,HIGH);
}
/*!
 * ���ڷ���λ�ò���
 * @param uartx ����ָ��
 * @param STEP_Pin  ��������
 * @param EN_Pin    ʹ������
 * @param DIAG_Pin  �ϱ����� �ж�����
 */
TMC220xUart::TMC220xUart(_USART_ *uartx,uint32_t STEP_Pin,uint32_t EN_Pin,uint32_t DIAG_Pin) {
    this->uart=uartx;
    this->STEP.init(STEP_Pin,HIGH);
    this->EN.init(EN_Pin,HIGH);
    this->DIAG.init(DIAG_Pin,HIGH);
}
/*!
 * ��ʼ��
 * @param mres ϸ��
 * @param maxdistance  ����г�
 * @param minMovedistance    ���һȦ�ƶ��ľ���
 * @param stallGuardThreshold  ��ײ��ֵ
 * @param Reverse  ������
 */
void TMC220xUart::initTMC2209(uint8_t mres,uint16_t maxdistance, uint16_t minMovedistance,
                              uint8_t stallGuardThreshold, bool Reverse) {
    this->stepAngle = 1.8;
    this->maxDistance = maxdistance;
    this->minMoveDistance = minMovedistance;
    this->ZERO_flag = false;
    this->stop = false;
    this->reverse=Reverse;
    this->DIAG.upload_extern_fun(this);/**�ж��Զ��ص�**/
    this->clearGSTAT();
    // ϸ��
    this->setMicrosteppingResolution(mres);
    // �����˴� ��ֵ ��ײ
    this->stallGuard(stallGuardThreshold);
    this->ZERO_flag = false;
}

void TMC220xUart::Callback(int,char** data) {
//    if(data[1][0]==this->DIAG.get_pin_num())
    this->stopMotor();
}
/*!
 * ����λ����
 */
void TMC220xUart::Return_to_zero() {
    this->ZERO_flag= false;
    this->moveToUART(Direction::Back,this->maxDistance);
    while(!this->get_stop_flag());
    this->ZERO_flag= true;
    this->stop = false;
}
/*!
 * ���ڷ���λ�ò���
 * @param DIR_Flag ����
 * @param moveDistance  �ƶ�����
 */
void TMC220xUart::moveToUART(uint8_t DIR_Flag, uint32_t moveDistance) {
    if(this->uart!= nullptr) {
        if (moveDistance > this->maxDistance)
            moveDistance = this->maxDistance;
        // ���ת��һȦ��Ҫ�Ĳ���
        float steps = ((360 * (float) this->div) / this->stepAngle);
        // ������Ҫ����Ȧ����ת��ָ��λ��
        float nums = ((float) moveDistance / (float) this->minMoveDistance);
        this->startMotor();//���ʹ��
        delay_ms(10);
        if (this->reverse ? DIR_Flag : !DIR_Flag) //��ת
            this->uart->write(dir_right, 8);
        else  //��ת
            this->uart->write(dir_left, 8);
        this->moveStepUart((uint64_t) (steps * nums));
        this->stopMotor();//�����ʹ��
    }
}

void TMC220xUart::calcCrc(uint8_t *datagram, uint8_t datagramLength) {
    int i, j;
    uint8_t *crc = datagram + (datagramLength - 1);
    uint8_t currentByte;
    *crc = 0;
    for (i = 0; i < (datagramLength - 1); i++) {
        currentByte = datagram[i];
        for (j = 0; j < 8; j++) {
            if ((*crc >> 7) ^ (currentByte & 0x01)) { *crc = (*crc << 1) ^ 0x07; }
            else { *crc = (*crc << 1); }
            currentByte = currentByte >> 1;
        }
    }
}

/*!
 * ������2Ϊ�׵Ķ���
 * @param x ��������
 * @return
 */
int TMC220xUart::fastLog2(int x) {
    float fx;
    unsigned long ix, exp;

    fx = (float) x;
    ix = *(unsigned long *) &fx;
    exp = (ix >> 23) & 0xFF;

    return (int)(exp - 127);
}
/*!
 * ֹͣ���
 */
void TMC220xUart::stopMotor() {
    this->EN.set_value(LOW);
    this->set_stop_flag(true);
}
/*!
 * ʹ�ܵ��
 */
void TMC220xUart::startMotor() {
    this->EN.set_value(HIGH);
    this->set_stop_flag(false);
}

//55 00 01 46   //05 FF 01    00 00 00 00   13       00 00 00
uint32_t TMC220xUart::readReg(uint8_t regAddr) {
    uint8_t send[4] = {0x55, 0x00, regAddr};
    uint32_t cache;
    calcCrc(send, 4);
    this->uart->write(send, 4);
    delay_us(5);
    string get_buff=this->uart->read_data();
    cache = (get_buff[7] << 24) + (get_buff[8] << 16) + (get_buff[9] << 8) + get_buff[10];
    return cache;
}

/*!
 * ����ȫ��״̬
 */
void TMC220xUart::clearGSTAT() {
    uint32_t data = this->readReg(0x01);
    data = data | 0x03;
    this->uartWriteInt(0x01, data);
}

/*!
 * ����ն��ģʽ
 * @param en_spread false ����ն�� true����ն��
 */
void TMC220xUart::setSpreadCycle(bool en_spread) {
    uint32_t data = this->readReg(0x00);
    if (en_spread) {
        data = data | (1 << 2);
    } else {
        data = data & ~(1 << 2);
    }
    this->uartWriteInt(0x00, data);
}

/*!
 * ����ϸ��ģʽ
 * @param en 0�ⲿϸ�� 1�ڲ�ϸ��
 */
void TMC220xUart::setStepResolutionRegSelect(bool en) {
    uint32_t data = this->readReg(0x00);
    if (en) {
        data = data | (1 << 7);
    } else {
        data = data & ~(1 << 7);
    }
    this->uartWriteInt(0x00, data);
}

/*!
 * �����ڲ�ϸ����
 * @param mres ϸ���� 256, 128, 64, 32, 16, 8, 4, 2, FULLSTEP
 */
void TMC220xUart::setMicrosteppingResolution(uint8_t mres) {
    uint32_t chopconf = this->readReg(0x6c);
    uint32_t msresdezimal = TMC220xUart::fastLog2(mres);
    msresdezimal = 8 - msresdezimal;
    chopconf = chopconf & 0xF0FFFFFF;
    chopconf = chopconf | msresdezimal << 24;
    this->uartWriteInt(0x6c, chopconf);
    this->setStepResolutionRegSelect(false);
    this->stepsPerRevolution = 200 * mres;
    this->div = mres;
}

/*!
 * ���ü��ٶ�TODO:ֻд�˼��ٶ�Ϊ0�����
 * @param vactual
 * @param acceleration
 * @return
 */
void TMC220xUart::setVactual(uint32_t vactual, int acceleration) {
    if (acceleration == 0) {
        this->uartWriteInt(0x22, vactual);
    }
}

//
void TMC220xUart::setVactualRps(float rps) {
    float vactual = rps / 0.715f * (float)stepsPerRevolution;
    this->setVactual((uint32_t) roundf(vactual), 0);
}

void TMC220xUart::setVactualRpm(uint32_t rpm) {
    this->setVactualRps((float)rpm / 60.0f);
}

/*!
 * ��������λ����
 * @param threshold ��ײ�����ֵ
 */
void TMC220xUart::stallGuard(uint32_t threshold) {
    this->setSpreadCycle(false);
    this->uartWriteInt(0x40, threshold);
    this->uartWriteInt(0x14, 2000);
    this->setVactualRpm(30);
}
/*!
 * ���ڷ�������
 * @param address ��ַ
 * @param value ֵ
 */
void TMC220xUart::uartWriteInt(unsigned char address, unsigned int value) {
    uint8_t writeData[8];

    writeData[0] = 0x05;                              // Sync byte
    writeData[1] = 0x00;                              // Slave address
    writeData[2] = address | 0x80;                    // Register address with write bit set
    writeData[3] = value >> 24;                       // Register Data
    writeData[4] = value >> 16;                       // Register Data
    writeData[5] = value >> 8;                        // Register Data
    writeData[6] = value & 0xFF;                      // Register Data
    calcCrc(writeData, 8);    // Cyclic redundancy check
//    delay_ms(100);
    this->uart->write(writeData, 8);
    delay_us(5);
//    delay_ms(100);
}
/*!
 * ���ڷ���λ�ò���
 * @param Step ����
 */
void TMC220xUart::moveStepUart(uint64_t Step) {
    uint64_t i;
    if (this->ZERO_flag) {
        uint16_t add = TMC_len;
        for (i = 0; i < Step; i++) {
            static uint8_t addc = 0;
            //��������
            this->STEP.set_value(LOW);
            delay_us(10 + add);
            this->STEP.set_value(HIGH);
            delay_us(10 + add);
            addc++;  //�ö�ʱ����Ч�������
            if ((Step - TMC_len * TMC_acc <= i) && (addc == TMC_acc)) {
                addc = 0;
                add++;
            } else if ((addc == TMC_acc) && (add > 0)) {
                addc = 0;
                add--;
            }
            if (this->get_stop_flag())break;
        }
    } else {
        for (i = 0; i < Step; i++) {
            this->STEP.set_value(LOW);
            delay_us(10);
            this->STEP.set_value(HIGH);
            delay_us(10);
            if (this->get_stop_flag())break;
        }
    }

}

void TMC220xUart::set_stop_flag(bool flag) {
    this->stop=flag;
}
/*!
 * ��ȡ����״̬
 */
bool TMC220xUart::get_stop_flag() const {
    return this->stop;
}


