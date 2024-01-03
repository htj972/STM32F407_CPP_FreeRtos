/**
* @Author kokirika
* @Name Device_Node_Def
* @Date 2023-11-23
*/
#include <sstream>
#include <algorithm>
#include <vector>
#include "Device_Node_Def.h"


Device_Node_Def::error_code Device_Node_Def::add_device_to_bus(Device_Node_Def::device_node *device, Device_Node_Def::node_bus &bus) {
    Device_Node_Def::device_node *device_t = bus.device;
    if (device_t == nullptr) {
        bus.device = device;
    } else {
        if (device_t->name == device->name) {
            return error_code::same_name;
        }
        while (device_t->next != nullptr) {
            device_t = device_t->next;
            if (device_t->name == device->name) {
                return error_code::same_name;
            }
        }
        device_t->next = device;
    }
    bus.num++;
    return success;
}

Device_Node_Def::error_code Device_Node_Def::add_node(Device_Node_Def::Bus bus,Device_Node_Def::device_node *device) {
    switch (bus) {
        case rs485:
            return add_device_to_bus(device, this->RS485_Bus);
        case lora:
            return add_device_to_bus(device,this->Lora_Bus);
    }
    return error_else;
}

Device_Node_Def::error_code Device_Node_Def::new_node(Device_Node_Def::Bus bus,std::string name,uint8_t id) {
    auto *device  = new device_node;
    device->name  = std::move(name);
    device->id    = id;
    device->next  = nullptr;
    device->mem   = nullptr;
    device->num   = 0;
    return this->add_node(bus,device);
}

Device_Node_Def::error_code Device_Node_Def::del_node(Device_Node_Def::Bus bus,const std::string& name) {
    Device_Node_Def::device_node **device;
    uint16_t *bus_num;
    switch (bus) {
        case rs485:
            device = &this->RS485_Bus.device;
            bus_num = &this->RS485_Bus.num;
            break;
        case lora:
            device = &this->Lora_Bus.device;
            bus_num = &this->Lora_Bus.num;
            break;
    }
    while (*device != nullptr) {
        if ((*device)->name == name) {
            del_all_mem(*device);
            Device_Node_Def::device_node *next_node = (*device)->next;
            delete *device;
            *device = next_node;
            (*bus_num)--;
            return success;
        }
        device = &(*device)->next;
    }
    return no_node;
}

Device_Node_Def::mem_node* Device_Node_Def::new_mem(const std::string& name, uint16_t add, data_type type, float compute) {
    auto *newMemNode = new mem_node;
    newMemNode->name = name;
    newMemNode->add = add;
    newMemNode->type = type;
    newMemNode->compute = compute;
    newMemNode->next = nullptr;
    return newMemNode;
}

Device_Node_Def::error_code Device_Node_Def::add_mem(Bus bus, const std::string& device_name, mem_node* new_mem_node) const {
    device_node *device;
    switch (bus) {
        case rs485:
            device = this->RS485_Bus.device;
            break;
        case lora:
            device = this->Lora_Bus.device;
            break;
    }
    while (device != nullptr) {
        if (device->name == device_name) {
            mem_node *mem = device->mem;
            while (mem != nullptr) {
                if (mem->name == new_mem_node->name) {
                    return same_name;
                }
                if (mem->next == nullptr) {
                    break;
                }
                mem = mem->next;
            }
            if (mem == nullptr) {
                device->mem = new_mem_node;
            } else {
                mem->next = new_mem_node;
            }
            device->num++;
            return success;
        }
        device = device->next;
    }
    return no_node;
}

Device_Node_Def::error_code Device_Node_Def::del_mem(Bus bus, const std::string& device_name, const std::string& mem_name) const {
    device_node *device;
    switch (bus) {
        case rs485:
            device = this->RS485_Bus.device;
            break;
        case lora:
            device = this->Lora_Bus.device;
            break;
    }

    while (device != nullptr) {
        if (device->name == device_name) {
            mem_node **mem = &device->mem;
            while (*mem != nullptr) {
                if ((*mem)->name == mem_name) {
                    mem_node *next_node = (*mem)->next;
                    delete *mem;
                    *mem = next_node;
                    device->num--;
                    return success;
                }
                mem = &(*mem)->next;
            }
            return no_mem;
        }
        device = device->next;
    }
    return no_node;
}

Device_Node_Def::error_code Device_Node_Def::del_all_mem(Bus bus, const std::string& device_name) const {
    device_node *device;
    switch (bus) {
        case rs485:
            device = this->RS485_Bus.device;
            break;
        case lora:
            device = this->Lora_Bus.device;
            break;
    }
    while (device != nullptr) {
        if (device->name == device_name) {
            mem_node *mem = device->mem;
            while (mem != nullptr) {
                mem_node *next_node = mem->next;
                delete mem;
                mem = next_node;
            }
            device->mem = nullptr;
            device->num = 0;
            return success;
        }
        device = device->next;
    }
    return no_node;
}

void Device_Node_Def::del_all_mem(device_node *device) {
    mem_node *mem = device->mem;
    while (mem != nullptr) {
        mem_node *next_node = mem->next;
        delete mem;
        mem = next_node;
    }
    device->mem = nullptr;
    device->num = 0;
}

void Device_Node_Def::clean_empty_nodes(Bus bus) {
    device_node **device;
    uint16_t *bus_num;
    switch (bus) {
        case rs485:
            device = &this->RS485_Bus.device;
            bus_num = &this->RS485_Bus.num;
            break;
        case lora:
            device = &this->Lora_Bus.device;
            bus_num = &this->Lora_Bus.num;
            break;
        default:
            return;
    }

    while (*device != nullptr) {
        if ((*device)->mem == nullptr) {
            device_node *next_node = (*device)->next;
            delete *device;
            *device = next_node;
            (*bus_num)--;
        } else {
            device = &(*device)->next;
        }
    }
}

uint16_t Device_Node_Def::get_Device_Num(Device_Node_Def::Bus bus) const {
    switch (bus) {
        case rs485:
            return this->RS485_Bus.num;
        case lora:
            return this->Lora_Bus.num;
    }
    return 0;
}

uint16_t Device_Node_Def::get_Mem_Num(Device_Node_Def::Bus bus, const std::string &device_name) const{
    device_node *device;
    switch (bus) {
        case rs485:
            device = this->RS485_Bus.device;
            break;
        case lora:
            device = this->Lora_Bus.device;
            break;
    }
    while (device != nullptr) {
        if (device->name == device_name) {
            return device->num;
        }
        device = device->next;
    }
    return 0;
}

std::string Device_Node_Def::display_bus(Device_Node_Def::Bus bus) const {
    device_node *device;
    std::string result = "bus:";
    switch (bus) {
        case rs485:
            device = this->RS485_Bus.device;
            result +="RS485\r\n";
            break;
        case lora:
            device = this->Lora_Bus.device;
            result +="Lora\r\n";
            break;
    }
    result += "├num：" + std::to_string(this->get_Device_Num(bus)) + "\r\n";
    while (device != nullptr) {
        std::string indent = (device->next == nullptr ? "   " : "│ ");
        result += (device->next == nullptr ? "└" : "├");
        result += "device："+ device->name + "\r\n";
        result += indent + "├id：" + std::to_string(device->id) + "\r\n";
        result += indent +(device->num==0 ? "└" : "├")+ "num：" + std::to_string(device->num) + "\r\n";
        mem_node *mem = device->mem;
        while (mem != nullptr) {
            std::string mem_indent = indent + (mem->next == nullptr ? "   " : "│ ");
            result += indent + (mem->next == nullptr ? "└" : "├") + "node："+ mem->name + "\r\n";
            result += mem_indent + "├add：" + std::to_string(mem->add) + "\r\n";
            result += mem_indent + "├type：" + display_tpye(mem->type) + "\r\n";
            result += mem_indent + "└compute：" + std::to_string(mem->compute) + "\r\n";
            mem = mem->next;
        }
        device = device->next;
    }
    return result;
}

std::string Device_Node_Def::display_tpye(Device_Node_Def::data_type type) {
    switch (type) {
        case float_type:
            return "float";
        case int_type:
            return "int";
        case uint_type:
            return "uint";
        case bool_type:
            return "bool";
    }
    return "error_else";
}

std::string Device_Node_Def::display_error_code(Device_Node_Def::error_code code) {
    switch (code) {
        case success:
            return "success !!\r\n";
        case no_node:
            return "error:no_node\r\n";
        case no_mem:
            return "error:no_mem\r\n";
        case same_name:
            return "error:same_name\r\n";
        case error_else:
            return "error:error_else\r\n";
    }
    return "error_else";
}

std::string Device_Node_Def::Command(const std::string &COM) {
    std::string ret;
//    ret += "<< " ;
//    ret += COM ;
//    ret += ">>" ;
    std::istringstream iss(COM);
    std::string command;
    iss >> command;
    if (command == "add_node") {
        std::string name;
        std::string bus_str;
        std::string ID;
        bool isValid = true;
        iss >> bus_str >> name >> ID;

        uint8_t id = 0;
//        try {
//            id = std::stoi(ID);
//        } catch (const std::invalid_argument& e) {
//            ret+= "Invalid number format: ";
//            ret+= e.what();
//            ret+= "\r\n";
//            isValid = false;
//        } catch (const std::out_of_range& e) {
//            ret+= "Number out of range: ";
//            ret+= e.what();
//            ret+= "\r\n";
//            isValid = false;
//        }

        std::istringstream issid(ID);
        int num;
        if (!(issid >> num) || num < 0 || num > 255) {
            isValid = false;
            ret+= "Error: Invalid number format\r\n";
        }
        id = num;

        if(isValid){
            if (bus_str == "RS485") {
                ret +=display_error_code(
                        this->new_node(Device_Node_Def::Bus::rs485,name,id));
            } else if (bus_str == "lora") {
                ret +=display_error_code(
                        this->new_node(Device_Node_Def::Bus::lora,name,id));
            } else {
                ret += "Invalid bus type: ";
                ret += bus_str;
                ret += "\r\n";
            }
        }
    } else if(command == "del_node"){
        std::string name;
        std::string bus_str;
        iss >> bus_str >> name;
        if (bus_str == "RS485") {
            ret +=display_error_code(
                    this->del_node(Device_Node_Def::Bus::rs485,name));
        } else if (bus_str == "lora") {
            ret +=display_error_code(
                    this->del_node(Device_Node_Def::Bus::lora,name));
        } else {
            ret += "Invalid bus type: ";
            ret += bus_str;
            ret += "\r\n";
        }
    }else if(command == "add_mem"){
        std::string name;
        std::string bus_str;
        std::string dev_str;
        std::string mem_add;
        std::string mem_type;
        std::string mem_compute;
        iss >> bus_str >> dev_str >> name >> mem_add >> mem_type >> mem_compute;
        bool isValid = true;
        uint16_t add = 0;
        float compute = 0.0f;

        std::istringstream issm(mem_add);
        int num;
        if (!(issm >> num) || num < 0 || num > 65535) {
            isValid = false;
            ret+= "Error: Invalid number format\r\n";
        }
        add = num;

        std::transform(mem_type.begin(), mem_type.end(), mem_type.begin(), ::tolower);

        data_type type;
        if (mem_type == "float") {
            type = float_type;
        } else if (mem_type == "int") {
            type = int_type;
        } else if (mem_type == "uint") {
            type = uint_type;
        } else if (mem_type == "bool") {
            type = bool_type;
        } else {
            ret += "Invalid data type: ";
            ret += mem_type;
            ret += "\r\n";
            isValid = false;
        }

        std::istringstream isst(mem_compute);
        float comp;
        if (!(isst >> comp)) {
            isValid = false;
            ret+= "Error: Invalid number format\r\n";
        }
        compute = comp;

        if(isValid){
            if (bus_str == "RS485") {
                mem_node *mem = new_mem(name, add, type,compute);
                ret +=display_error_code(
                        this->add_mem(Device_Node_Def::Bus::rs485, dev_str, mem));
            } else if (bus_str == "lora") {
                mem_node *mem = new_mem(name, add, (data_type)type,compute);
                ret +=display_error_code(
                        this->add_mem(Device_Node_Def::Bus::lora, dev_str, mem));
            } else {
                ret += "Invalid bus type: ";
                ret += bus_str;
                ret += "\r\n";
            }
        }
    }else if(command == "del_mem"){
        std::string name;
        std::string bus_str;
        std::string dev_str;
        iss >> bus_str >> dev_str >> name;
        if (bus_str == "RS485") {
            ret +=display_error_code(
                    this->del_mem(Device_Node_Def::Bus::rs485,dev_str,name));
        } else if (bus_str == "lora") {
            ret +=display_error_code(
                    this->del_mem(Device_Node_Def::Bus::lora,dev_str, name));
        } else {
            ret += "Invalid bus type: ";
            ret += bus_str;
            ret += "\r\n";
        }
    }
    else if(command == "tree") {
        std::string bus_str;
        iss  >> bus_str;
        if (bus_str == "RS485") {
            ret +=this->display_bus(Device_Node_Def::Bus::rs485);
//            ret += Device_Node_Def::outputNodes(Device_Node_Def::Bus::RS485);
        } else if (bus_str == "lora") {
            ret +=this->display_bus(Device_Node_Def::Bus::lora);
//            ret += Device_Node_Def::outputNodes(Device_Node_Def::Bus::lora);
        } else {
            ret += "Invalid bus type: ";
            ret += bus_str;
            ret +="\r\n";
        }
    }
    else {
        ret += "Invalid command: ";
        ret += command;
        ret += "\r\n";
    }
    return ret;
}

std::string Device_Node_Def::outputNodes(Device_Node_Def::Bus bus) const {
    device_node *device;
    std::string name;
    switch (bus) {
        case rs485:
            device = this->RS485_Bus.device;
            name = "RS485";
            break;
        case lora:
            device = this->Lora_Bus.device;
            name = "lora";
            break;
    }
    std::stringstream ss;
    while(device != nullptr) {
        ss << "add_node " << name << " " << device->name << " " << (int)device->id << "\n";
        mem_node* mem = device->mem;
        while(mem != nullptr) {
            ss << "add_mem " << name << " " << device->name << " " << mem->name << " " << mem->add << " float " << mem->compute << "\n";
            mem = mem->next;
        }
        device = device->next;
    }
    std::string str = ss.str();
    return str;
}

std::string Device_Node_Def::inputNodes(const std::string &data, _USART_ *Debug) {
    // 计算总行数
    int totalLines = std::count(data.begin(), data.end(), '\n');

    std::stringstream ss(data);
    std::string line;
    int currentLine = 0;
    int failCount = 0;
    try {
        while(std::getline(ss, line)) {
            // 显示进度和命令内容
            std::string progress = "(" + std::to_string(++currentLine) + "/" + std::to_string(totalLines) + ") " + line;
            if(Debug != nullptr) {
                Debug->print(progress + "\r\n");  // 假设_USART_类有一个名为print的方法
            }

            std::string result = Command(line);  // 假设你已经实现了这个函数并返回结果
            if(result.find("success") != std::string::npos) {
                if(Debug != nullptr) {
                    Debug->print((string )"OK!\r\n");
                }
            } else {
                failCount++;
                if(Debug != nullptr) {
                    Debug->print(" Failed: " + result + "\r\n");  // 如果失败，添加失败信息
                }
            }
        }
    } catch (const std::exception& e) {
        // 处理异常，例如添加错误信息到返回字符串
        if(Debug != nullptr) {
            Debug->print((std::string)"An error occurred: " + e.what() + "\r\n");  // 打印错误信息
        }
    }
    return "一共：" + std::to_string(totalLines) + ",失败：" + std::to_string(failCount)+ "\r\n";
}

