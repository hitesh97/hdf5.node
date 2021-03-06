#pragma once
#include <v8.h>
#include <uv.h>
#include <node.h>

#include <memory>
#include <iostream>

#include "hdf5.h"
#include "H5IMpublic.h"

namespace NodeHDF5 {

    class H5im {
    public:
static void Initialize (Handle<Object> target) {
        
        // append this function to the target object
        target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "makeImage"), FunctionTemplate::New(v8::Isolate::GetCurrent(), H5im::make_image)->GetFunction());
        target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "readImage"), FunctionTemplate::New(v8::Isolate::GetCurrent(), H5im::read_image)->GetFunction());
        target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "isImage"), FunctionTemplate::New(v8::Isolate::GetCurrent(), H5im::is_image)->GetFunction());
        
    }

static void make_image (const v8::FunctionCallbackInfo<Value>& args)
{

    String::Utf8Value dset_name (args[1]->ToString());
    Local<Uint8Array> buffer =  Local<Uint8Array>::Cast(args[2]);
//    Local<Number> buffer =  Local<Number>::Cast(args[2]);
//    std::cout<<"planes "<<buffer->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "planes"))->ToInt32()->Value()<<std::endl;
    String::Utf8Value interlace (buffer->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "interlace"))->ToString());
    if(buffer->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "planes"))->ToInt32()->Value()==3)
    {
        herr_t err=H5IMmake_image_24bit (args[0]->ToInt32()->Value(), *dset_name,  buffer->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "width"))->ToInt32()->Value(), buffer->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "height"))->ToInt32()->Value(), *interlace, (const unsigned char *)buffer->Buffer()->Externalize().Data());
        if(err<0)
        {
            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to make image 24 bit")));
            args.GetReturnValue().SetUndefined();
            return;
        }
    }
    args.GetReturnValue().SetUndefined();
}

static void read_image (const v8::FunctionCallbackInfo<Value>& args)
{

    String::Utf8Value dset_name (args[1]->ToString());
    hsize_t width;
    hsize_t height;
    hsize_t planes;
    char interlace[255];
    hssize_t npals;
    herr_t err=H5IMget_image_info(args[0]->ToInt32()->Value(), *dset_name, &width, &height, &planes, interlace, &npals);
    if(err<0)
    {
        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to get image info")));
        args.GetReturnValue().SetUndefined();
        return;
    }
//    Local<ArrayBuffer> arrayBuffer=ArrayBuffer::New(v8::Isolate::GetCurrent(), (size_t)(planes*width*height));
    std::unique_ptr<unsigned char[]> contentBuffer(new unsigned char[(size_t)(planes*width*height)]);
    Local<Uint8Array> buffer = Uint8Array::New(ArrayBuffer::New(v8::Isolate::GetCurrent(), (size_t)(planes*width*height)), 0, (size_t)(planes*width*height));
    err=H5IMread_image (args[0]->ToInt32()->Value(), *dset_name, contentBuffer.get() );
    if(err<0)
    {
        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to read image")));
        args.GetReturnValue().SetUndefined();
        return;
    }
    for(size_t index=0;index<(size_t)(planes*width*height);index++)
    {
        buffer->Set(index, Number::New(v8::Isolate::GetCurrent(), contentBuffer[index]));
    }
    buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "width"), Number::New(v8::Isolate::GetCurrent(), width));
    buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "height"), Number::New(v8::Isolate::GetCurrent(), height));
    buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "planes"), Number::New(v8::Isolate::GetCurrent(), planes));
    buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "interlace"), String::NewFromUtf8(v8::Isolate::GetCurrent(), interlace));
    buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "npals"), Number::New(v8::Isolate::GetCurrent(), npals));

    args.GetReturnValue().Set(buffer);
}

static void is_image (const v8::FunctionCallbackInfo<Value>& args)
{

    String::Utf8Value dset_name (args[1]->ToString());
    herr_t err=H5IMis_image ( args[0]->ToInt32()->Value(), *dset_name );
    args.GetReturnValue().Set(err? true:false);
}

    };
}