#ifndef HELLOWORLD_H
#define HELLOWORLD_H

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/variant/utility_functions.hpp>



using namespace godot;

class HelloWorld : public Node
{
    GDCLASS(HelloWorld, Node);

protected:
    static void _bind_methods();

public:
    HelloWorld();
    ~HelloWorld();

    void hello_message();
};

#endif