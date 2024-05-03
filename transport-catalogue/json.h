#pragma once

#include <stdexcept>
#include <map>
#include <string>
#include <variant>
#include <vector>

using namespace std::literals;

namespace json
{

    class Node;

    // Сохраните объявления Dict и Array без изменения
    using Dict = std::map<std::string, Node>;
    using Array = std::vector<Node>;

    // Эта ошибка должна выбрасываться при ошибках парсинга JSON
    class ParsingError : public std::runtime_error
    {
    public:
        using runtime_error::runtime_error;
    };

    std::ostream &operator<<(std::ostream &os, const Node &node);

    class Node final
        : private std::variant<std::nullptr_t, Array, Dict, bool, double, int, std::string>
    {
    public:
        /* Реализуйте Node, используя std::variant */
        using variant::variant;
        using Value = variant;

        const variant &GetValue() const;

        bool IsInt() const;
        bool IsDouble() const;
        bool IsPureDouble() const;
        bool IsBool() const;
        bool IsString() const;
        bool IsNull() const;
        bool IsArray() const;
        bool IsMap() const;
        int AsInt() const;
        bool AsBool() const;
        double AsDouble() const;
        const std::string &AsString() const;
        const Array &AsArray() const;
        const Dict &AsMap() const;

        bool operator==(const Node &node) const;
        bool operator!=(const Node &node) const;
    };

    class Document
    {
    public:
        explicit Document(Node root);

        const Node &GetRoot() const;

        bool operator==(const Document &rhs);
        bool operator!=(const Document &rhs);

    private:
        Node root_;
    };

    Node LoadNull(std::istream &input);
    Node LoadString(std::istream &input);
    Node LoadArray(std::istream &input);
    Node LoadDict(std::istream &input);
    Node LoadBool(std::istream &input);
    Node LoadNumber(std::istream &input);
    Node LoadNode(std::istream &input);

    Document Load(std::istream &input);

    void Print(const Document &doc, std::ostream &output);

} // namespace json