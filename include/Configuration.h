#ifndef COOLPROP_CONFIGURATION
#define COOLPROP_CONFIGURATION

#include "Exceptions.h"
#include "CoolPropTools.h"
#include "rapidjson/rapidjson_include.h"

enum configuration_keys {NORMALIZE_GAS_CONSTANTS, CRITICAL_SPLINES_ENABLED};

namespace CoolProp
{

/// Convert the configuration key to a string in a 1-1 representation.
std::string config_key_to_string(configuration_keys keys);
    
/// A class that contains one entry in configuration
/// Can be cast to yield the output value
class ConfigurationItem
{
    public:
        enum ConfigurationDataTypes {CONFIGURATION_NOT_DEFINED_TYPE = 0, 
                                     CONFIGURATION_BOOL_TYPE, 
                                     CONFIGURATION_DOUBLE_TYPE, 
                                     CONFIGURATION_INTEGER_TYPE, 
                                     CONFIGURATION_STRING_TYPE,
                                     CONFIGURATION_ENDOFLIST_TYPE};
        ConfigurationDataTypes type;
        
        /// Cast to boolean
        operator bool() { check_data_type(CONFIGURATION_BOOL_TYPE);  return v_bool; };
        /// Cast to double
        operator double() { check_data_type(CONFIGURATION_DOUBLE_TYPE);  return v_double; };
        /// Cast to string
        operator std::string() { check_data_type(CONFIGURATION_STRING_TYPE);  return v_string; };
        // Initializer for bool
        ConfigurationItem(configuration_keys key, bool val){
            this->key = key; type = CONFIGURATION_BOOL_TYPE; v_bool = val;
        };
        // Initializer for double
        ConfigurationItem(configuration_keys key, double val){ 
            this->key = key; type = CONFIGURATION_DOUBLE_TYPE; v_double = val;
        };
        // Initializer for string
        ConfigurationItem(configuration_keys key, std::string val){
            this->key = key; type = CONFIGURATION_STRING_TYPE; v_string = val;
        };
        configuration_keys get_key(void){
            return this->key;
        }
        /// Cast to rapidjson::Value
        void add_to_json(rapidjson::Value &val, rapidjson::Document &d){
            std::string name_string = config_key_to_string(key);
            rapidjson::Value name(name_string.c_str(), d.GetAllocator());
            switch (type){
                case CONFIGURATION_BOOL_TYPE:
                {
                    rapidjson::Value v(v_bool);
                    val.AddMember(name, v, d.GetAllocator()); break;
                }
                case CONFIGURATION_INTEGER_TYPE:
                {
                    rapidjson::Value v(v_integer);
                    val.AddMember(name, v, d.GetAllocator()); break;
                }
                case CONFIGURATION_DOUBLE_TYPE:
                {
                    rapidjson::Value v(v_double);
                    val.AddMember(name, v, d.GetAllocator()); break;
                }
                case CONFIGURATION_STRING_TYPE:
                {
                    rapidjson::Value v(v_string.c_str(), d.GetAllocator());
                    val.AddMember(name, v, d.GetAllocator()); break;
                }
                case CONFIGURATION_ENDOFLIST_TYPE:
                case CONFIGURATION_NOT_DEFINED_TYPE:
                    throw ValueError();
            }
        }
        void set_from_json(rapidjson::Value &val){
            switch (type){
                case CONFIGURATION_BOOL_TYPE:{
                    if (!val.IsBool()){throw ValueError(format("Input is not boolean"));}
                    v_bool = val.GetBool(); break;
                }
                case CONFIGURATION_INTEGER_TYPE:{
                    if (!val.IsInt()){throw ValueError(format("Input is not integer"));}
                    v_integer = val.GetInt(); break;
                }
                case CONFIGURATION_DOUBLE_TYPE:{
                    if (!val.IsDouble()){throw ValueError(format("Input is not double"));}
                    v_double = val.GetDouble(); break;
                }
                case CONFIGURATION_STRING_TYPE:{
                    if (!val.IsString()){throw ValueError(format("Input is not string"));}
                    v_string = val.GetString(); break;
                }
                case CONFIGURATION_ENDOFLIST_TYPE:
                case CONFIGURATION_NOT_DEFINED_TYPE:
                    throw ValueError();
            }
        }
         
    protected:
        void check_data_type(ConfigurationDataTypes type){
            if (type != this->type){
                throw ValueError(format("type does not match"));
            }
        };
        double v_double;
        bool v_bool;
        int v_integer;
        std::string v_string;
        configuration_keys key;
};

class Configuration
{
    protected:
        std::map<configuration_keys,ConfigurationItem> items;
        
    public:
        Configuration(){set_defaults();};
        ~Configuration(){};
        
        /// Get an item from the configuration
        ConfigurationItem &get_item(configuration_keys key){
            std::map<configuration_keys,ConfigurationItem>::iterator it;
            // Try to find it
            it = items.find(key);
            // If equal to end, not found
            if (it != items.end()){
                // Found, return it
                return it->second;
            }
            else{
                throw ValueError(format("invalid item"));
            }
        }
        /// Add an item to the configuration
        void add_item(ConfigurationItem item)
        {
            items.insert(std::make_pair<configuration_keys, ConfigurationItem>(item.get_key(), item));
        };
        
        /// Return a reference to all of the items
        std::map<configuration_keys, ConfigurationItem> & get_items(void){return items;};
        
        /// Set the default values in the configuration
        void set_defaults(void)
        {
            add_item(ConfigurationItem(CRITICAL_SPLINES_ENABLED, true));
            add_item(ConfigurationItem(NORMALIZE_GAS_CONSTANTS, true));
        };
};

/// *********************************************************
///                      GETTERS
/// *********************************************************

/// Return the value of a boolean key from the configuration
bool get_config_bool(configuration_keys key);
double get_config_double(configuration_keys key);
rapidjson::Document get_config_as_json();
/// Get values in the configuration based as json data in string format
std::string get_config_as_json_string();

/// *********************************************************
///                      SETTERS
/// *********************************************************

void set_config_bool(configuration_keys key, bool val);
void set_config_double(configuration_keys key, double val);
void set_config_string(configuration_keys key, std::string val);
/// Set values in the configuration based on a json file
void set_config_json(rapidjson::Document & doc);
void set_config_as_json_string(std::string &s);
}

#endif // COOLPROP_CONFIGURATION
