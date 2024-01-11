// © Copyright 2014-2022, Juan Linietsky, Ariel Manzur and the Godot community (CC-BY 3.0)
#ifndef AI_BLUE_CLASS_H
#define AI_BLUE_CLASS_H

// We don't need windows.h in this plugin but many others do and it throws up on itself all the time
// So best to include it and make sure CI warns us when we use something Microsoft took for their own goals....
#ifdef WIN32
#include <windows.h>
#endif

#include <godot_cpp/classes/ref.hpp>

using namespace godot;

class AIBlue : public RefCounted
{
    GDCLASS(AIBlue, RefCounted);

    int count;

protected:
    static void _bind_methods();

public:
    AIBlue();
    ~AIBlue();

    void add(int p_value);
    void reset();
    int get_total() const;
};

#endif // AIBlue_CLASS_H