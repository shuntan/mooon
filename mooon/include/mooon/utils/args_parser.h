/**
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Writed by yijian on 2015/7/18, eyjian@gmail/eyjian@qq.com
 */
#ifndef MOOON_UTILS_ARGS_PARSER_H
#define MOOON_UTILS_ARGS_PARSER_H
#include <mooon/utils/any2string.h>
#include <mooon/utils/string_utils.h>
#include <map>

// 使用示例：
//
// #include <mooon/utils/args_parser.h>
//
// STRING_ARG_DEFINE(ip, "127.0.0.1", "listen IP address");
// INTEGER_ARG_DEFINE(uint16_t, port, 2015, 1000, 5000, "listen port");
//
// int main(int argc, char* argv[])
// {
//     std::string errmsg;
//     if (!mooon::utils::parse_arguments(argc, argv, &errmsg))
//     {
//         fprintf(stderr, "%s\n", errmsg.c_str());
//         exit(1);
//     }
//
//     printf("ip: %s\n", mooon::argument::ip->c_value());
//     printf("port: %u\n", mooon::argument::port->value());
//     return 0;
// }

// 应当总是在main()函数所在文件中调用STRING_ARG_DEFINE()和INTEGER_ARG_DEFINE()，
// 如果其它文件中也需要访问，则应当使用STRING_ARG_DECLARE()和INTEGER_ARG_DECLARE()。

// 注意不用要在其它namespace内调用
// 字符串类型参数定义（供main()函数所在文件中调用）
// param_name 参数名
// default_value 参数的默认值，如果没有通过命令行指定，则default_value为参数值
// help_string 对这个参数的说明
//
// 使用示例（假设参数名为ip）：
// STRING_ARG_DEFINE(ip, "127.0.0.1", "listen IP address");
// mooon::argument::ip->value(); // 返回类型为“const std::string”
// mooon::argument::ip->c_value(); // 返回类型为“const char*”
#define STRING_ARG_DEFINE(param_name, default_value, help_string) \
    namespace mooon { namespace argument \
    { \
        utils::CStringArgument* param_name = \
            new utils::CStringArgument( \
                #param_name, default_value, help_string); \
    }}

// 注意不用要在其它namespace内调用
// 整数类型参数定义（供main()函数所在文件中调用）
//
// 如遇到如下所示的编译错误，
// 这是因为int_type指定了不带长度的类型，如time_t或long等，指定为带长度的类型，如int32_t等即可解决
// no matching function for call to 'mooon::utils::CStringUtils::string2int(const char*, long int&, uint8_t&, bool&)'
//
// int_type 参数的数据类型
// param_name 参数名
// default_value 参数的默认值，如果没有通过命令行指定，则default_value为参数值
// help_string 对这个参数的说明
// min_value 可取的最小值
// max_value 可取的最大值
//
// 使用示例（假设参数名为port）:
// INTEGER_ARG_DEFINE(uint16_t, port, 2015, 1000, 65535, "listen port");
// mooon::argument::port->value(); // 返回类型为“uint16_t”
#define INTEGER_ARG_DEFINE(int_type, param_name, default_value, min_value, max_value, help_string) \
    namespace mooon { namespace argument \
    { \
        utils::CIntArgument<int_type>* param_name = \
            new utils::CIntArgument<int_type>( \
                #param_name, default_value, min_value, max_value, help_string); \
    }}

// 定义双精度类型参数
#define DOUBLE_ARG_DEFINE(param_name, default_value, min_value, max_value, help_string) \
    namespace mooon { namespace argument \
    { \
        utils::CDoubleArgument* param_name = \
            new utils::CDoubleArgument( \
                #param_name, default_value, min_value, max_value, help_string); \
    }}

// 注意不用要在其它namespace内调用
// 整数类型参数声明（供非main()函数所在文件中调用）
#define INTEGER_ARG_DECLARE(int_type, param_name) \
    namespace mooon { namespace argument /** 保证不污染全局空间 */ \
    { \
        extern utils::CIntArgument<int_type>* param_name; \
    }}

 // 声明双精度类型参数
#define DOUBLE_ARG_DECLARE(param_name) \
    namespace mooon { namespace argument /** 保证不污染全局空间 */ \
    { \
        extern utils::CDoubleArgument* param_name; \
    }}

// 注意不用要在其它namespace内调用
// 整数类型参数声明（供非main()函数所在文件中调用）
#define STRING_ARG_DECLARE(param_name) \
    namespace mooon { namespace argument /** 保证不污染全局空间 */ \
    { \
        extern utils::CStringArgument* param_name; \
    }}

////////////////////////////////////////////////////////////////////////////////
UTILS_NAMESPACE_BEGIN

extern std::string g_help_string; // --help的说明
extern std::string g_version_string; // --version的说明

// 解析命令行参数
// 如果解析出错，则返回false，errmsg保存出错原因
bool parse_arguments(int argc, char* argv[], std::string* errmsg);

////////////////////////////////////////////////////////////////////////////////
class CArgumentBase
{
public:
    CArgumentBase(const std::string& name, const std::string& help_string);
    virtual ~CArgumentBase() {}
    virtual bool set_value(const std::string& new_value, std::string* errmsg) = 0;
    virtual std::string usage_string() const { return std::string(""); }

public:
    const std::string& name() const
    {
        return _name;
    }

    const std::string& help_string() const
    {
        return _help_string;
    }

    const char* c_name() const
    {
        return _name.c_str();
    }

    const char* c_help_string() const
    {
        return _help_string.c_str();
    }

private:
    std::string _name;
    std::string _help_string;
};

class CArgumentContainer
{
    SINGLETON_DECLARE(CArgumentContainer)

public:
    void add_argument(CArgumentBase* argument);
    bool set_argument(const std::string& name, const std::string& value, std::string* errmsg);
    std::string usage_string() const;

private:
    std::map<std::string, CArgumentBase*> _argument_table;
};

////////////////////////////////////////////////////////////////////////////////
class CStringArgument: public CArgumentBase
{
public:
    CStringArgument(const std::string& name, const std::string& default_value, const std::string& help_string)
        : CArgumentBase(name, help_string), _default_value(default_value), _value(default_value)
    {
        CArgumentContainer::get_singleton()->add_argument(this);
    }

    const std::string default_value() const
    {
        return _default_value;
    }

    const std::string value() const
    {
        return _value;
    }

    const char* c_default_value() const
    {
        return _default_value.c_str();
    }

    const char* c_value() const
    {
        return _value.c_str();
    }

public:
    virtual bool set_value(const std::string& new_value, std::string* errmsg)
    {
        _value = new_value;
        return true;
    }

    virtual std::string usage_string() const
    {
        std::string prefix;
        if (name().length() < 2)
            prefix = "-";
        else
            prefix = "--";

        return mooon::utils::CStringUtils::format_string(
             "%s%s[%s]: %s", prefix.c_str(), c_name(), c_default_value(), c_help_string());
    }

private:
    std::string _default_value;
    std::string _value;
};

template <typename IntType>
class CIntArgument: public CArgumentBase
{
public:
    CIntArgument(const std::string& name, IntType default_value, IntType min_value, IntType max_value, const std::string& help_string)
        : CArgumentBase(name, help_string),
          _default_value(default_value), _min_value(min_value), _max_value(max_value), _value(default_value)
    {
        CArgumentContainer::get_singleton()->add_argument(this);
    }

    IntType default_value() const
    {
        return _default_value;
    }

    IntType min_value() const
    {
        return _min_value;
    }

    IntType max_value() const
    {
        return _max_value;
    }

    IntType value() const
    {
        return _value;
    }

    const std::string& str_value() const
    {
        return _str_value;
    }

public:
    virtual bool set_value(const std::string& new_value, std::string* errmsg)
    {
        IntType value = 0;
        uint8_t converted_length = 0; // 阻止调用模板类型的string2int，在一些环境这将导致编译错误，原因是long等类型并不没有对应的带长度的类型
        bool ignored_zero = false;

        _str_value = new_value;
        if (!mooon::utils::CStringUtils::string2int(new_value.c_str(), value, converted_length, ignored_zero))
        {
            *errmsg = CStringUtils::format_string("invalid value[%s] of argument[%s]", new_value.c_str(), c_name());
            return false;
        }

        if ((value < _min_value) || (value > _max_value))
        {
            // 对于int8_t或uint8_t需要强制转换一下，否则按字符显示
            if (sizeof(IntType) == sizeof(char))
                *errmsg = any2string("value[", (int)value, "] of argument[", name(), "] not between ", (int)_min_value, " and ", (int)_max_value);
            else
                *errmsg = any2string("value[", value, "] of argument[", name(), "] not between ", _min_value, " and ", _max_value);
            return false;
        }
        else
        {
            _value = value;
            return true;
        }
    }

    virtual std::string usage_string() const
    {
        std::string prefix;
        if (name().length() < 2)
            prefix = "-";
        else
            prefix = "--";

        if (sizeof(IntType) == sizeof(char))
        {
            return mooon::utils::any2string(
                prefix, name(), "[", (int)_default_value, "/", (int)_min_value, ",", (int)_max_value, "]: ", help_string());
        }
        else
        {
            return mooon::utils::any2string(
                prefix, name(), "[", _default_value, "/", _min_value, ",", _max_value, "]: ", help_string());
        }
    }

private:
    std::string _str_value;
    IntType _default_value;
    IntType _min_value;
    IntType _max_value;
    IntType _value;
};

class CDoubleArgument: public CArgumentBase
{
public:
    CDoubleArgument(const std::string& name, double default_value, double min_value, double max_value, const std::string& help_string)
        : CArgumentBase(name, help_string),
          _default_value(default_value), _min_value(min_value), _max_value(max_value), _value(default_value)
    {
        CArgumentContainer::get_singleton()->add_argument(this);
    }

    double default_value() const
    {
        return _default_value;
    }

    double min_value() const
    {
        return _min_value;
    }

    double max_value() const
    {
        return _max_value;
    }

    double value() const
    {
        return _value;
    }

    const std::string& str_value() const
    {
        return _str_value;
    }

public:
    virtual bool set_value(const std::string& new_value, std::string* errmsg)
    {
        double value = 0.0;

        _str_value = new_value;
        if (!mooon::utils::CStringUtils::string2double(new_value.c_str(), value))
        {
            *errmsg = CStringUtils::format_string("invalid value[%s] of argument[%s]", new_value.c_str(), c_name());
            return false;
        }

        if ((value < _min_value) || (value > _max_value))
        {
            *errmsg = any2string("value[", value, "] of argument[", name(), "] not between ", _min_value, " and ", _max_value);
            return false;
        }
        else
        {
            _value = value;
            return true;
        }
    }

    virtual std::string usage_string() const
    {
        std::string prefix;
        if (name().length() < 2)
            prefix = "-";
        else
            prefix = "--";

         return mooon::utils::any2string(
                prefix, name(), "[", _default_value, "/", _min_value, ",", _max_value, "]: ", help_string());
    }

private:
    std::string _str_value;
    double _default_value;
    double _min_value;
    double _max_value;
    double _value;
};

UTILS_NAMESPACE_END
#endif // MOOON_UTILS_ARGS_PARSER_H
