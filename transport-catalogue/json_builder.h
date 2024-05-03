#pragma once
#include "json.h"

#include <memory>

namespace json
{
    class BaseContext;
    class DictItemContext;
    class DictKeyContext;
    class ArrayItemContext;
    class Builder;

    class BaseContext
    {
    public:
        BaseContext(Builder &builder);
        Node Build();
        DictKeyContext Key(std::string key);
        Builder &Value(Node::Value value);
        DictItemContext StartDict();
        ArrayItemContext StartArray();
        Builder &EndDict();
        Builder &EndArray();

    private:
        Builder &builder_;
    };

    class DictItemContext : public BaseContext
    {
    public:
        DictItemContext(BaseContext base);
        Node Build() = delete;
        DictItemContext Value(Node::Value value) = delete;
        DictItemContext StartDict() = delete;
        ArrayItemContext StartArray() = delete;
        Builder &EndArray() = delete;
    };

    class DictKeyContext : public BaseContext
    {
    public:
        DictKeyContext(BaseContext base);
        DictItemContext Value(Node::Value value);
        Node Build() = delete;
        DictKeyContext &Key(std::string key) = delete;
        Builder &EndDict() = delete;
        Builder &EndArray() = delete;
    };

    class ArrayItemContext : public BaseContext
    {
    public:
        ArrayItemContext(BaseContext base);
        ArrayItemContext Value(Node::Value value);
        Node Build() = delete;
        Builder &Key(std::string key) = delete;
        Builder &EndDict() = delete;
    };

    class Builder
    {
    public:
        //Builder() = default;

        Node CreateNode(Node::Value value);
        void AddNode(Node node);

        Builder &Value(Node::Value value);

        DictItemContext StartDict();
        DictKeyContext Key(std::string key);
        Builder &EndDict();

        ArrayItemContext StartArray();
        Builder &EndArray();

        Node Build();

        //~Builder() = default;

    private:
        Node root_ = nullptr;
        std::vector<std::unique_ptr<Node>> nodes_stack_;
    };

}