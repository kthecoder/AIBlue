#include "ai_blue.h"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

AIBlue::AIBlue()
{
    count = 0;
}

AIBlue::~AIBlue()
{
}

void AIBlue::add(int p_value)
{
    count += p_value;
}

void AIBlue::reset()
{
    count = 0;
}

int AIBlue::get_total() const
{
    return count;
}

void AIBlue::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("add", "value"), &AIBlue::add, DEFVAL(1));
    ClassDB::bind_method(D_METHOD("reset"), &AIBlue::reset);
    ClassDB::bind_method(D_METHOD("get_total"), &AIBlue::get_total);
}