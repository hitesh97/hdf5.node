#pragma once
#include <v8.h>
#include <uv.h>
#include <node.h>

#include <iostream>
#include <cstring>
#include <memory>

#include "hdf5.h"
#include "H5LTpublic.h"
#include "ArrayBuffer.hpp"

namespace NodeHDF5 {

    class Uint32Array : public node::ObjectWrap {
    private:
        unsigned long long length;
        std::unique_ptr<unsigned int[]> buffer;
        Local<ArrayBuffer> arrayBuffer;
            static v8::Persistent<v8::FunctionTemplate> Constructor;
            static v8::Handle<v8::Value> New (const v8::Arguments& args){

        if (args.Length() == 1 && args[0]->IsUint32()) {
                
//        std::cout<<"New  "<<args[0]->ToUint32()->IntegerValue()<<std::endl;
                Uint32Array* array = new Uint32Array(args[0]->ToUint32()->IntegerValue());
                array->Wrap(args.This());
        }
        else if(args.Length() == 1 && args[0]->IsObject())
        {
         // unwrap array buffer object
        ArrayBuffer* buf = ObjectWrap::Unwrap<ArrayBuffer>(args[0]->ToObject());
               Uint32Array* array = new Uint32Array(buf->length/4);
                array->Wrap(args.This());
            
        }
//    std::cout<<"old Uint32Array New "<<std::endl;
                return args.This();
            }
    public:
    Uint32Array(unsigned long long length): length (length), buffer(new unsigned int[length]) {
    }
    static void Initialize () {
        
        // instantiate constructor template
        Local<FunctionTemplate> t = FunctionTemplate::New(New);
        
        // set properties
        t->InstanceTemplate()->Set(String::NewSymbol("BYTES_PER_ELEMENT"), v8::Uint32::New(4));
        t->SetClassName(String::New("Uint32Array"));
        t->InstanceTemplate()->SetInternalFieldCount(1);
//	t->InstanceTemplate()->SetAccessor(String::New("learning_momentum"), GetLearningMomentum, SetLearningMomentum);
        // member method prototypes
        Local<Function> getFunc=v8::FunctionTemplate::New(Uint32Array::get)->GetFunction();
        getFunc->SetName(v8::String::NewSymbol("get"));
        t->InstanceTemplate()->Set(v8::String::NewSymbol("get"), getFunc);
        Local<Function> setFunc=v8::FunctionTemplate::New(Uint32Array::set)->GetFunction();
        setFunc->SetName(v8::String::NewSymbol("set"));
        t->InstanceTemplate()->Set(v8::String::NewSymbol("set"), setFunc);
        Local<Function> sliceFunc=v8::FunctionTemplate::New(Uint32Array::slice)->GetFunction();
        sliceFunc->SetName(v8::String::NewSymbol("slice"));
        t->InstanceTemplate()->Set(v8::String::NewSymbol("slice"), sliceFunc);
        Local<Function> subarrayFunc=v8::FunctionTemplate::New(Uint32Array::subarray)->GetFunction();
        subarrayFunc->SetName(v8::String::NewSymbol("subarray"));
        t->InstanceTemplate()->Set(v8::String::NewSymbol("subarray"), subarrayFunc);
//        NODE_SET_METHOD(t->InstanceTemplate(), "get", get);
//        NODE_SET_METHOD(t->InstanceTemplate(), "set", get);
//        NODE_SET_METHOD(t->InstanceTemplate(), "slice", slice);
//        NODE_SET_METHOD(t->InstanceTemplate(), "subarray", subarray);
//        NODE_SET_PROTOTYPE_METHOD(t, "length", length);
        
        // initialize constructor reference
//        std::cout<<"Uint32Array Constructor  "<<std::endl;
        Constructor=Persistent<FunctionTemplate>::New(t);
//        std::cout<<"create  "<<std::endl;
    Constructor->SetClassName(String::NewSymbol("Uint32Array"));
//    Constructor->InstanceTemplate()->Set(String::NewSymbol("BYTES_PER_ELEMENT"), v8::Uint32::New(4));
//        NODE_SET_METHOD(Constructor->InstanceTemplate(), "get", get);
//        NODE_SET_PROTOTYPE_METHOD(Constructor, "set", get);
//        NODE_SET_PROTOTYPE_METHOD(Constructor, "slice", slice);
//        NODE_SET_PROTOTYPE_METHOD(Constructor, "subarray", subarray);
//
//    // object has one internal filed ( the C++ object)
    Constructor->InstanceTemplate()->SetInternalFieldCount(1);
//        std::cout<<"created  "<<std::endl;
        
    }
    
    static Handle<v8::Value> NewInstance(const v8::Arguments& args){
        HandleScope scope;
        Local<Value> argv[1] = {
                
                Uint32::New(args[0]->ToUint32()->IntegerValue())
                
        };
        Local<Object> instance = Constructor->GetFunction()->NewInstance(1,argv);
        Uint32Array* pThis = node::ObjectWrap::Unwrap<Uint32Array>(instance);
        instance->SetIndexedPropertiesToExternalArrayData((void * )pThis->buffer.get(), v8::kExternalDoubleArray, (int)pThis->length);
        //pThis->setShape(shape);
        return scope.Close(instance);

    }


    static v8::Handle<v8::Value> get (const v8::Arguments& args) {
        
        HandleScope scope;
        
        if (args.Length() != 1 || !args[0]->IsUint32()) {
            return scope.Close(Undefined());
        }
        // unwrap array
        Uint32Array* array = Uint32Array::Unwrap<Uint32Array>(args.This());
        
        return scope.Close(Number::New(array->buffer[args[0]->ToUint32()->IntegerValue()]));
    }
    
    static v8::Handle<v8::Value> set (const v8::Arguments& args) {
        
        HandleScope scope;
        
        if (args.Length() != 2 || !args[0]->IsUint32() || !args[1]->IsNumber()) {
            return scope.Close(Undefined());
        }
        // unwrap array
        Uint32Array* array = Uint32Array::Unwrap<Uint32Array>(args.This());
        if (array->length>=(unsigned long long)args[0]->ToUint32()->IntegerValue()) {
            return scope.Close(Undefined());
        }
        
        array->buffer[args[0]->ToUint32()->IntegerValue()]=args[0]->ToNumber()->NumberValue();
        
        return scope.Close(Undefined());
    }
    
    static v8::Handle<v8::Value> slice (const v8::Arguments& args) {
        
        HandleScope scope;
        
        if (args.Length() <1 || args.Length() >2 ||!args[0]->IsUint32() || (args.Length() ==2 && !args[1]->IsUint32())) {
            return scope.Close(Undefined());
            
        }
        // unwrap array
        Uint32Array* array = Uint32Array::Unwrap<Uint32Array>(args.This());
        unsigned long begin =args[0]->ToUint32()->IntegerValue();
        unsigned long end=array->length;
        if(args.Length()==2)end=args[1]->ToUint32()->IntegerValue();
        Local<Object> slice=Uint32Array::Instantiate(ArrayBuffer::Instantiate(std::max((unsigned long)0, end-begin-1)));
        Uint32Array* pThis = node::ObjectWrap::Unwrap<Uint32Array>(slice);
        slice->SetIndexedPropertiesToExternalArrayData((void * )pThis->buffer.get(), v8::kExternalUnsignedIntArray, (int)pThis->length);
        std::memcpy(slice->GetIndexedPropertiesExternalArrayData(), (array->buffer.get()+begin), pThis->length);
        return scope.Close(slice);
    }
    
    static v8::Handle<v8::Value> subarray (const v8::Arguments& args) {
        
        HandleScope scope;
        
        // unwrap array
//        Uint32Array* array = Uint32Array::Unwrap<Uint32Array>(args.This());
        
        return scope.Close(Undefined());
    }
    
    static Local<Object> Instantiate (unsigned long long size) {
        
//        HandleScope scope;
        
        // group name and file reference
        Local<Value> argv[1] = {
                
                Uint32::New(size)
                
        };

        Local<Object> instance=Local<FunctionTemplate>::New(Constructor)->GetFunction()->NewInstance(1, argv);
        Uint32Array* pThis = node::ObjectWrap::Unwrap<Uint32Array>(instance);
//        pThis->arrayBuffer.buffer=ArrayBuffer::Instantiate(4*size);
        instance->SetIndexedPropertiesToExternalArrayData((void * )pThis->buffer.get(), v8::kExternalUnsignedIntArray, (int)pThis->length);
        instance->Set(String::NewSymbol("buffer"), ArrayBuffer::Instantiate(4*size));
        // return new array instance
        return instance;
        
    }

    static Local<Object> Instantiate (Local<Object> arrayBuffer) {
        
//        HandleScope scope;
        
        // group name and file reference
        Local<Value> argv[1] = {
                
                arrayBuffer
                
        };

        Local<Object> instance=Local<FunctionTemplate>::New(Constructor)->GetFunction()->NewInstance(1, argv);
        Uint32Array* pThis = node::ObjectWrap::Unwrap<Uint32Array>(instance);
//        pThis->arrayBuffer.buffer=ArrayBuffer::Instantiate(4*size);
        ArrayBuffer* pArrayBuffer = node::ObjectWrap::Unwrap<ArrayBuffer>(arrayBuffer);
        instance->SetIndexedPropertiesToExternalArrayData((void * )pArrayBuffer->buffer.get(), v8::kExternalUnsignedIntArray, (int)pThis->length);
        instance->Set(String::NewSymbol("buffer"), arrayBuffer);
        // return new array instance
        return instance;
        
    }
    
    };
    Persistent<FunctionTemplate> Uint32Array::Constructor;
}