#pragma once
#include "./STLTypes.h"

using namespace std::string_literals;

namespace FluidHTN
{
    enum class ConsoleColor
    {
        Black,
        Red,
        DarkRed,
        Blue,
        DarkBlue,
        Green,
        DarkGreen,
        White,
        Yellow,
        DarkYellow
    };
    class Debug
    {
    public:
        static StringType DepthToString(int depth)
        {
            StringType s = ""s;
            for (auto i = 0; i < depth; i++)
            {
                s += "\t"s;
            }

            s += "- "s;
            return s;
        }
    };
    struct IBaseDecompositionLogEntry
    {
        StringType Name;
        StringType Description;
        int Depth;
        ConsoleColor Color;
    };

    template <typename T>
    struct IDecompositionLogEntry : public IBaseDecompositionLogEntry
    {
    public:
        SharedPtr<T> _Entry;

        IDecompositionLogEntry(IBaseDecompositionLogEntry logMeta, SharedPtr<T> entry)
            : IBaseDecompositionLogEntry(logMeta), _Entry(entry) {}
    };
} // namespace FluidHTN
