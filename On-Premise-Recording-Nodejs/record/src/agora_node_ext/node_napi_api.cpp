/*
* Copyright (c) 2017 Agora.io
* All rights reserved.
* Proprietry and Confidential -- Agora.io
*/

/*
*  Created by Wang Yongli, 2017
*/

#include "node_napi_api.h"
#include "node_uid.h"

int napi_get_value_string_utf8_(const Local<Value> &str, char *buffer, uint32_t len)
{
    if (!str->IsString())
        return 0;
    if (!buffer)
    {
        return str.As<String>()->Utf8Length();
    }
    else
    {
        int copied = str.As<String>()->WriteUtf8(buffer, len - 1, nullptr, String::REPLACE_INVALID_UTF8 | String::NO_NULL_TERMINATION);
        buffer[copied] = '\0';
        return copied;
    }
}

napi_status napi_get_value_uint32_(const Local<Value> &value, uint32_t &result)
{
    if (!value->IsUint32())
        return napi_invalid_arg;
    result = value->Uint32Value();
    return napi_ok;
}

napi_status napi_get_value_bool_(const Local<Value> &value, bool &result)
{
    if (!value->IsBoolean())
        return napi_invalid_arg;
    result = value->BooleanValue();
    return napi_ok;
}

napi_status napi_get_value_int32_(const Local<Value> &value, int32_t &result)
{
    if (!value->IsInt32())
        return napi_invalid_arg;
    result = value->Int32Value();
    return napi_ok;
}

napi_status napi_get_value_double_(const Local<Value> &value, double &result)
{
    if (!value->IsNumber())
        return napi_invalid_arg;

    result = value->NumberValue();
    return napi_ok;
}

napi_status napi_get_value_int64_(const Local<Value> &value, int64_t &result)
{
    int32_t tmp;
    napi_status status = napi_get_value_int32_(value, tmp);
    result = tmp;
    return status;
}

napi_status napi_get_value_nodestring_(const Local<Value> &str, NodeString &nodechar)
{
    napi_status status = napi_ok;
    do
    {
        int len = napi_get_value_string_utf8_(str, nullptr, 0);
        if (len == 0)
        {
            break;
        }
        char *outstr = NodeString::alloc_buf(len + 1);
        if (!outstr)
        {
            status = napi_generic_failure;
            break;
        }
        len = napi_get_value_string_utf8_(str, outstr, len + 1);

        if (status != napi_ok)
        {
            break;
        }
        nodechar.setBuf(outstr);
    } while (false);
    return status;
}

Local<Value> napi_create_uint32_(Isolate *isolate, const uint32_t &value)
{
    return v8::Number::New(isolate, value);
}

Local<Value> napi_create_bool_(Isolate *isolate, const bool &value)
{
    return v8::Boolean::New(isolate, value);
}

Local<Value> napi_create_string_(Isolate *isolate, const char *value)
{
    return String::NewFromUtf8(isolate, value ? value : "");
}

Local<Value> napi_create_double_(Isolate *isolate, const double &value)
{
    return v8::Number::New(isolate, value);
}

Local<Value> napi_create_uint64_(Isolate *isolate, const uint64_t &value)
{
    return v8::Number::New(isolate, value);
}

Local<Value> napi_create_int32_(Isolate *isolate, const int32_t &value)
{
    return v8::Int32::New(isolate, value);
}

Local<Value> napi_create_uint16_(Isolate *isolate, const uint16_t &value)
{
    return v8::Uint32::New(isolate, value);
}

Local<Value> napi_create_uid_(Isolate *isolate, const uid_t& uid)
{
    return NodeUid::getNodeValue(isolate, uid);
}

napi_status napi_get_value_int32_object_(Isolate *isolate, Local<Object> &object, const char *str_key, int32_t &result)
{
    Local<Name> key = String::NewFromUtf8(isolate, str_key, NewStringType::kInternalized).ToLocalChecked();
    Local<Value> value = object->Get(isolate->GetCurrentContext(), key).ToLocalChecked();
    napi_status status = napi_get_value_int32_(value, result);
    return status;
}

napi_status napi_get_value_uint32_object_(Isolate *isolate, Local<Object> &object, const char *str_key, uint32_t &result)
{
    Local<Name> key = String::NewFromUtf8(isolate, str_key, NewStringType::kInternalized).ToLocalChecked();
    Local<Value> value = object->Get(isolate->GetCurrentContext(), key).ToLocalChecked();
    napi_status status = napi_get_value_uint32_(value, result);
    return status;
}

napi_status napi_get_value_double_object_(Isolate *isolate, Local<Object> &object, const char *str_key, double &result)
{
    Local<Name> key = String::NewFromUtf8(isolate, str_key, NewStringType::kInternalized).ToLocalChecked();
    Local<Value> value = object->Get(isolate->GetCurrentContext(), key).ToLocalChecked();
    napi_status status = napi_get_value_double_(value, result);
    return status;
}

napi_status napi_get_value_string_object_(Isolate *isolate, Local<Object> &object, const char *str_key, NodeString &result)
{
    Local<Name> key = String::NewFromUtf8(isolate, str_key, NewStringType::kInternalized).ToLocalChecked();
    Local<Value> value = object->Get(isolate->GetCurrentContext(), key).ToLocalChecked();
    napi_status status = napi_get_value_nodestring_(value, result);
    return status;
}

napi_status napi_get_value_array_object_(Isolate *isolate, Local<Object> &object, const char *str_key, Local<Array> &result)
{
    Local<Name> key = String::NewFromUtf8(isolate, str_key, NewStringType::kInternalized).ToLocalChecked();
    Local<Value> value = object->Get(isolate->GetCurrentContext(), key).ToLocalChecked();
    if(!value->IsArray()){
        return napi_invalid_arg;
    }
    result = Local<Array>::Cast(value);
    return napi_ok;
}

