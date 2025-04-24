#include "Hello_World.h"

using namespace godot;

HelloWorld::HelloWorld()
{
}

HelloWorld::~HelloWorld()
{
}

void HelloWorld::hello_message()
{
    UtilityFunctions::print("Hello, World!");
}

void HelloWorld::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("hello_message"), &HelloWorld::hello_message);
}