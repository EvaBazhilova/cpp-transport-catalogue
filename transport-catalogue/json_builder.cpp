#include "json_builder.h"

namespace json
{
    // BaseContext
    BaseContext::BaseContext(Builder &builder)
        : builder_(builder)
    {
    }
    Node BaseContext::Build()
    {
        return builder_.Build();
    }
    DictKeyContext BaseContext::Key(std::string key)
    {
        return builder_.Key(key);
    }
    Builder &BaseContext::Value(Node::Value value)
    {
        return builder_.Value(value);
    }
    DictItemContext BaseContext::StartDict()
    {
        return DictItemContext(builder_.StartDict());
    }
    ArrayItemContext BaseContext::StartArray()
    {
        return ArrayItemContext(builder_.StartArray());
    }
    Builder &BaseContext::EndDict()
    {
        return builder_.EndDict();
    }
    Builder &BaseContext::EndArray()
    {
        return builder_.EndArray();
    }
    // end BaseContext

    // DictItemContext
    DictItemContext::DictItemContext(BaseContext base)
        : BaseContext(std::move(base))
    {
    }
    // end DictItemContext

    // DictKeyContext
    DictKeyContext::DictKeyContext(BaseContext base)
        : BaseContext(std::move(base))
    {
    }
    DictItemContext DictKeyContext::Value(Node::Value value)
    {
        return DictItemContext(BaseContext::Value(std::move(value)));
    }
    // end DictKeyContext

    // ArrayItemContext
    ArrayItemContext::ArrayItemContext(BaseContext base)
        : BaseContext(std::move(base))
    {
    }
    ArrayItemContext ArrayItemContext::Value(Node::Value value)
    {
        return ArrayItemContext(BaseContext::Value(std::move(value)));
    }
    // end ArrayItemContext

    Node Builder::CreateNode(Node::Value value)
    {
        Node node;
        if (std::holds_alternative<int>(value))
        {
            node = Node(std::get<int>(value));
            return node;
        }
        if (std::holds_alternative<double>(value))
        {
            node = Node(std::get<double>(value));
            return node;
        }
        if (std::holds_alternative<bool>(value))
        {
            node = Node(std::get<bool>(value));
            return node;
        }
        if (std::holds_alternative<std::nullptr_t>(value))
        {
            node = Node(std::get<std::nullptr_t>(value));
            return node;
        }
        if (std::holds_alternative<Array>(value))
        {
            node = Node(std::get<Array>(std::move(value)));
            return node;
        }
        if (std::holds_alternative<std::string>(value))
        {
            node = Node(std::get<std::string>(std::move(value)));
            return node;
        }
        if (std::holds_alternative<Dict>(value))
        {
            node = Node(std::get<Dict>(std::move(value)));
            return node;
        }
        return Node();
    }

    void Builder::AddNode(Node node)
    {
        if (nodes_stack_.empty())
        {
            if (root_ != nullptr)
            {
                throw std::logic_error("Add not after creation JSON!");
            }
            root_ = node;
        }
        else
        {
            if (!nodes_stack_.back()->IsArray() && !nodes_stack_.back()->IsString())
            {
                throw std::logic_error("Something wrong");
            }

            if (nodes_stack_.back()->IsArray())
            {
                Array array = nodes_stack_.back()->AsArray();
                array.emplace_back(node);

                nodes_stack_.pop_back();
                nodes_stack_.emplace_back(std::move(std::make_unique<Node>(array)));

                return;
            }

            if (nodes_stack_.back()->IsString())
            {
                std::string str = nodes_stack_.back()->AsString();
                nodes_stack_.pop_back();

                if (nodes_stack_.back()->IsMap())
                {
                    Dict dict = nodes_stack_.back()->AsMap();
                    dict.emplace(std::move(str), node);

                    nodes_stack_.pop_back();
                    nodes_stack_.emplace_back(std::move(std::make_unique<Node>(dict)));
                }

                return;
            }
        }
    }

    Builder &Builder::Value(Node::Value value)
    {
        AddNode(CreateNode(value));
        return *this;
    }

    DictItemContext Builder::StartDict()
    {
        nodes_stack_.emplace_back(std::move(std::make_unique<Node>(Dict())));
        return DictItemContext(*this);
    }

    DictKeyContext Builder::Key(std::string key)
    {
        if (nodes_stack_.empty())
        {
            throw std::logic_error("Dictionary does not exist");
        }
        if (nodes_stack_.back()->IsString())
        {
            throw std::logic_error("Key after key can not exist!");
        }
        if (nodes_stack_.back()->IsMap())
        {
            nodes_stack_.emplace_back(std::move(std::make_unique<Node>(key)));
        }
        return DictKeyContext(*this);
    }

    Builder &Builder::EndDict()
    {
        if (nodes_stack_.empty())
        {
            throw std::logic_error("it is impossible to close a dictionary that is not already open");
        }
        if (!nodes_stack_.back()->IsMap())
        {
            throw std::logic_error("Try to close not a dictionary");
        }
        Node node = *nodes_stack_.back();
        nodes_stack_.pop_back();
        AddNode(node);

        return *this;
    }

    ArrayItemContext Builder::StartArray()
    {
        nodes_stack_.emplace_back(std::move(std::make_unique<Node>(Array())));
        return ArrayItemContext(*this);
    }

    Builder &Builder::EndArray()
    {
        if (nodes_stack_.empty())
        {
            throw std::logic_error("it is impossible to close an array that is not already open");
        }
        if (!(*nodes_stack_.back()).IsArray())
        {
            throw std::logic_error("Try to close not an array");
        }
        Node node = *nodes_stack_.back();
        nodes_stack_.pop_back();
        AddNode(node);

        return *this;
    }

    Node Builder::Build()
    {
        if (root_ == nullptr)
        {
            throw std::logic_error("JSON is empty");
        }
        if (!nodes_stack_.empty())
        {
            throw std::logic_error("Errors in JSON");
        }
        return root_;
    }
}
