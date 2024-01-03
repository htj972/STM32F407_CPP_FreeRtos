/**
* @Author kokirika
* @Name Device_Node_Def
* @Date 2023-11-23
*/
#ifndef kokirika_Device_Node_Def_H
#define kokirika_Device_Node_Def_H

#include "IIC.h"
#include "USART.h"

class Device_Node_Def {
public:
    enum error_code: uint8_t{
        success=0,
        same_name,
        no_node,
        no_mem,
        error_else,
    };
    enum data_type: uint8_t{
        float_type=0,
        int_type,
        uint_type,
        bool_type,
    };
    enum Bus{
        rs485=0,
        lora,
    };
    //8bit对齐
    typedef struct mem_node_{
        uint16_t add;
        data_type type;
        float compute;
        std::string name;
        mem_node_ *next;
    }mem_node;
    typedef struct device_node_{
        mem_node *mem{};
        uint16_t num;
        std::string name;
        uint8_t id{};
        device_node_ *next{};
        device_node_(){
            this->num=0;
        }
    }device_node;
    struct node_bus{
        device_node *device{};
        uint16_t num;
        node_bus() {
            this->num=0;
        }
    }Lora_Bus,RS485_Bus;

protected:
    //新建节点到总线
    static error_code add_device_to_bus(device_node *device, node_bus &bus);
    error_code add_node(Bus bus,device_node *device);
    static void del_all_mem(device_node *device);
    static std::string display_tpye(data_type type) ;
public:
    error_code new_node(Bus bus,std::string name,uint8_t id);
    error_code del_node(Bus bus,const std::string& name);
    static mem_node *new_mem(const std::string &name, uint16_t add, data_type type, float compute);
    error_code add_mem(Bus bus,const std::string &device_name, mem_node *new_mem_node) const;
    error_code del_mem(Bus bus,const std::string &device_name, const std::string &mem_name) const;
    error_code del_all_mem(Bus bus,const std::string &device_name) const;
    void clean_empty_nodes(Bus bus);
    uint16_t get_Device_Num(Bus bus) const;
    uint16_t get_Mem_Num(Bus bus,const std::string &device_name) const;
    static std::string display_error_code(error_code code) ;

    std::string display_bus(Bus bus) const;

    virtual std::string Command(const std::string &COM);

    std::string outputNodes(Device_Node_Def::Bus bus) const;
    std::string inputNodes(const std::string &data,_USART_ *Debug);

};


#endif //kokirika_Device_Node_Def_H
