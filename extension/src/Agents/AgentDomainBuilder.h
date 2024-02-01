#include "FluidHTN/BaseDomainBuilder.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/script.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/character_body3d.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace FluidHTN;
using namespace godot;

/*
    Agent Domain Builder

    Description : Contains the helper functions to cleanly write domain logic.
                  Encapsulates the Actions, Conditions, Operators, and effects of
                  a task.

*/

class AgentDomainBuilder final : public BaseDomainBuilder
{
public:
    AgentDomainBuilder(StringType n) : BaseDomainBuilder(n) {}

    // TODO Implement Extra HasState, SetState

    //======================================================//
    // Encapsulated HTN Tasks
    //======================================================//
    // TODO Implement Encapsulated HTN Tasks
    void MoveTo()
    {
        UtilityFunctions::print("Got to MoveTo().");
        AddAction("MoveTo");
        UtilityFunctions::print("Added MoveTo Action.");

        AddOperator([](IContext &) -> TaskStatus
                    { 
                        UtilityFunctions::print("Got to Add Operator.");
                        //TODO Implement Godot Function that you call to Execute MoveTo
                        //! Godot Function must return Enum of TaskStatus
                        // You could return an INT of 0 - 2, and convert it in a helper class
                        Ref<Resource> res = ResourceLoader::get_singleton()->load("controller/agent/AgentController.gd", "Script");
                        if (res.is_valid() && res->is_class("Script")) {
                            UtilityFunctions::print("Resource Loaded, is a script.");
                            Ref<Script> script = res;
                            //ERROR : Can't call non-static function 'moveTo' in script
                            Variant ret = script->call("moveTo");
                        } else {
                            // Handle the case where the resource isn't a script or couldn't be loaded
                        }

                        /*
                            Node *node = get_node<CharacterBody3D>(NodePath("AgentController"));
                            CharacterBody3D *node = Object::cast_to<CharacterBody3D>(Node::get_node(NodePath("AgentController")));
                            node->call('moveTo');
                        */
                        
                        
                        return TaskStatus::Success; });

        UtilityFunctions::print("Added MoveTo Operator");
    }
};