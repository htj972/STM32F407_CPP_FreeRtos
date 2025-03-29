/**
* @Author kokirika
* @Name Modbus
* @Date 2022-09-20
**/

#include "fertilizer.h"
#include "cJSON.h"

string fertilizer::fertilizer_data() const {
    string str;
    str.append("{\"water_mode\":"+to_string(water_mode)+",");
    str.append("\"fertilizer_mode\":"+to_string(fertilizer_mode)+"}");
    return str;
}

void fertilizer::set_water_mode(uint8_t mode, float value) {
    this->water_mode=mode;
    this->water_value=value;
    this->water_flag= true;
}

void fertilizer::set_fertilizer_mode(uint8_t mode, float value) {
    this->fertilizer_mode=mode;
    this->fertilizer_value=value;
    this->fertilizer_flag= true;
}

void fertilizer::set_pial_fertilizer(float pial, float volume) {
    this->pial_fertilizer=pial;
    this->bucket_volume=volume;
    this->bucket_remaining=volume;
    this->bucket_remaining_time=volume/pial;
}

void fertilizer::get_cmd_str(const string& cmd) {
    cJSON *root = cJSON_Parse(cmd.c_str());
    if(root!= nullptr){//  //检查json是否正确cJSON_GetErrorPtr
        cJSON *water_json = cJSON_GetObjectItem(root,"water");
        if(water_json!= nullptr) {
            cJSON *water_mode_js = cJSON_GetObjectItem(water_json, "mode");
            if (water_mode_js != nullptr) {
                cJSON *water_value_js = cJSON_GetObjectItem(water_json, "value");
                if (water_value_js != nullptr) {
                    if (string(water_mode_js->valuestring) == "press")
                        this->set_water_mode(fertilizer::WATER_MODE::press,
                                              float (water_value_js->valuedouble));
                    if (string(water_mode_js->valuestring) == "power")
                        this->set_water_mode(fertilizer::WATER_MODE::power,
                                              float (water_value_js->valuedouble));
                }
                cJSON_Delete(water_value_js);
            }
            cJSON_Delete(water_mode_js);
        }
        cJSON_Delete(water_json);
        cJSON *fertilizer_json = cJSON_GetObjectItem(root,"fertilizer");
        if(fertilizer_json!= nullptr) {
            cJSON *fertilizermode_js = cJSON_GetObjectItem(fertilizer_json, "mode");
            if (fertilizermode_js != nullptr) {
                cJSON *fertilizerva_js = cJSON_GetObjectItem(fertilizer_json, "value");
                if (fertilizerva_js != nullptr) {
                    if (string(fertilizermode_js->valuestring) == "flow")
                        this->set_fertilizer_mode(fertilizer::fertilizer_MODE::flow,
                                                   float (fertilizerva_js->valuedouble));
                    if (string(fertilizermode_js->valuestring) == "ratio")
                        this->set_fertilizer_mode(fertilizer::fertilizer_MODE::ratio,
                                                   float (fertilizerva_js->valuedouble));
                }
                cJSON_Delete(fertilizerva_js);
            }
            cJSON_Delete(fertilizermode_js);
        }
        cJSON_Delete(fertilizer_json);
        cJSON *pial_json = cJSON_GetObjectItem(root,"pial");
        if(pial_json!= nullptr) {
            cJSON *fertilizer_pial = cJSON_GetObjectItem(pial_json,"fertilizer");
            if (fertilizer_pial!= nullptr) {
                cJSON *fertilizer_volume = cJSON_GetObjectItem(pial_json,"volume");
                if (fertilizer_volume!= nullptr) {
                    this->set_pial_fertilizer(float (fertilizer_pial->valuedouble),
                                               float (fertilizer_volume->valuedouble));
                }
                cJSON_Delete(fertilizer_volume);
            }
            cJSON_Delete(fertilizer_pial);
        }
        cJSON_Delete(pial_json);
    }
    cJSON_Delete(root);
}

string fertilizer::set_fertilizer_cmd() const {
    string str;
    str.append("{\"fertilizer\":{");
    if(this->fertilizer_mode==fertilizer::fertilizer_MODE::flow)
        str.append(R"("mode":"flow",)");
    else
        str.append(R"("mode":"ratio",)");
    str.append("\"value\":"+to_string(this->fertilizer_value)+"}}");
    return str;
}

bool fertilizer::get_fertilizer_flag(uint16_t *mode, float *value) {
    bool ret=this->fertilizer_flag;
    if(ret) {
        *mode = this->fertilizer_mode;
        *value = this->fertilizer_value;
        this->fertilizer_flag = false;
    }
    return ret;
}

bool fertilizer::get_water_flag(uint16_t *mode, float *value) {
    bool ret=this->water_flag;
    if(ret) {
        *mode = this->water_mode;
        *value = this->water_value;
        this->water_flag = false;
    }
    return ret;
}
