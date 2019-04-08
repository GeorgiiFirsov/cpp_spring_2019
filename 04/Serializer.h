#pragma once

#include <iostream>

using std::ostream;
using std::istream;

enum class Error
{
    NoError,
    CorruptedArchive
};

class Serializer
{

public:
    
    explicit Serializer(ostream& out) : out_(out) {}

    template <class T>
    Error save(T& object)
    {
        return object.serialize(*this);
    }

    template <class... ArgsT>
    Error operator()(ArgsT&&... args)
    {
        return process(std::forward<ArgsT>(args)...);
    }

private:

    static constexpr char Separator = ' ';
    std::ostream& out_;

    template <class T,  class... ArgsT>
    Error process(T&& value, ArgsT&& ... args)
    {
        if (process(std::forward<T>(value)) == Error::NoError) // вызов спецификации
            return process(std::forward<ArgsT>(args)...); // "сдвиг" аргументов
        else return Error::CorruptedArchive;
    }

    // спецификация для bool
    Error process(bool value)
    {
        auto& errcheck = out_ << (value ? "true" : "false") << Separator;
        return errcheck ? Error::NoError : Error::CorruptedArchive;
    }

    // спецификация для uint64_t
    Error process(uint64_t value)
    {
        auto& errcheck = out_ << value << Separator;
        return errcheck ? Error::NoError : Error::CorruptedArchive;
    }
};

class Deserializer
{

public:

    explicit Deserializer(istream& in) : in_(in) {}

    template <class T>
    Error load(T& object)
    {
        return object.serialize(*this);
    }

    template <class... ArgsT>
    Error operator()(ArgsT&&... args)
    {
        return process(std::forward<ArgsT>(args)...);
    }

private:

    std::istream& in_;

    template <class T,  class... ArgsT>
    Error process(T&& value, ArgsT&& ... args)
    {
        if (process(std::forward<T>(value)) == Error::NoError) // вызов спецификации 
            return process(std::forward<ArgsT>(args)...); // "сдвиг" аргументов
        else return Error::CorruptedArchive;
    }

    // спецификация для bool
    Error process(bool& value)
    {
        std::string text;
        in_ >> text;
        if (text == "true") value = true;
        else if (text == "false") value = false;
        else return Error::CorruptedArchive;
        return Error::NoError;
    }

    // спецификация для uint64_t
    Error process(uint64_t& value)
    {
        std::string value_string;
        in_ >> value_string;
        for (auto symb: value_string)
        {
            if (symb >= '0' && symb <= '9')
                value = value*10  + symb - '0';
            else return Error::CorruptedArchive;
        }
        return Error::NoError;
    }
};
