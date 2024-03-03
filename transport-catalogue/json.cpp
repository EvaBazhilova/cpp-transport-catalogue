#include "json.h"

#include <iostream>

using namespace std;
using namespace literals;

namespace json
{

    struct NodePrinter
    {
        ostream &out;
        void operator()(nullptr_t)
        {
            out << "null";
        }
        void operator()(bool v)
        {
            out << boolalpha << v;
        }
        void operator()(double v)
        {
            out << v;
        }
        void operator()(int v)
        {
            out << v;
        }

        void operator()(const string &v)
        {
            out << "\""sv;
            for (char c : v)
            {
                switch (c)
                {
                case '\\':
                    out << "\\\\"sv;
                    break;
                case '\"':
                    out << "\\\""sv;
                    break;
                case '\n':
                    out << "\\n"sv;
                    break;
                case '\r':
                    out << "\\r"sv;
                    break;
                case '\t':
                    out << "\\t"sv;
                    break;
                default:
                    out << c;
                    break;
                }
            }
            out << "\""sv;
        }
        void operator()(const Array &v)
        {
            out << "["sv << endl;
            for (size_t i = 0; i < v.size(); ++i)
            {
                out << v[i];
                if (i + 1 == v.size())
                {
                    break;
                }
                out << "," << endl;
            }
            out << endl;
            out << "]"sv;
        }
        void operator()(const Dict &v)
        {

            size_t lim = v.size();
            out << "{"sv << endl;
            for (const auto &i : v)
            {
                out << '"' << i.first << "\": " << i.second;
                --lim;
                if (lim == 0)
                    break;
                out << "," << endl;
            }
            out << endl;
            out << "}"sv;
        }
    };

    ostream &operator<<(ostream &os, const Node &node)
    {
        visit(NodePrinter{os}, node.GetValue());
        return os;
    }

    Node LoadNode(istream &input);

    Node LoadNull(istream &input)
    {
        string value;
        getline(input, value, ',');
        auto it = value.find(' ');
        while (it != std::string::npos)
        {
            value.erase(it);
            it = value.find(' ');
        }
        // std::cerr << value << " null" << std::endl;
        if (value == "null")
        {
            return {};
        }
        else
        {
            throw ParsingError("Null parsing error");
        }
    }

    // Считывает содержимое строкового литерала JSON-документа
    // Функцию следует использовать после считывания открывающего символа ":
    Node LoadString(istream &input)
    {
        using namespace literals;

        auto it = istreambuf_iterator<char>(input);
        auto end = istreambuf_iterator<char>();
        string s;
        while (true)
        {
            if (it == end)
            {
                // Поток закончился до того, как встретили закрывающую кавычку?
                throw ParsingError("String parsing error");
            }
            const char ch = *it;
            if (ch == '"')
            {
                // Встретили закрывающую кавычку
                ++it;
                break;
            }
            else if (ch == '\\')
            {
                // Встретили начало escape-последовательности
                ++it;
                if (it == end)
                {
                    // Поток завершился сразу после символа обратной косой черты
                    throw ParsingError("String parsing error");
                }
                const char escaped_char = *(it);
                // Обрабатываем одну из последовательностей: \\, \n, \t, \r, \"
                switch (escaped_char)
                {
                case 'n':
                    s.push_back('\n');
                    break;
                case 't':
                    s.push_back('\t');
                    break;
                case 'r':
                    s.push_back('\r');
                    break;
                case '"':
                    s.push_back('"');
                    break;
                case '\\':
                    s.push_back('\\');
                    break;
                default:
                    // Встретили неизвестную escape-последовательность
                    throw ParsingError("Unrecognized escape sequence \\"s + escaped_char);
                }
            }
            else if (ch == '\n' || ch == '\r')
            {
                // Строковый литерал внутри- JSON не может прерываться символами \r или \n
                throw ParsingError("Unexpected end of line"s);
            }
            else
            {
                // Просто считываем очередной символ и помещаем его в результирующую строку
                s.push_back(ch);
            }
            ++it;
        }
        return Node(move(s));
    }

    Node LoadArray(istream &input)
    {
        Array result;

        if (input.peek() == -1)
            throw ParsingError("");
        char c;
        for (; input >> c && c != ']';)
        {
            if (c != ',')
            {
                input.putback(c);
            }
            result.push_back(LoadNode(input));
        }

        if (c != ']')
        {
            throw ParsingError("Incorrect input"s);
        }
        return Node(move(result));
    }

    Node LoadDict(istream &input)
    {
        Dict result;
        if (input.peek() == -1)
            throw ParsingError("");
        char c;
        for (; input >> c && c != '}';)
        {
            if (c == ' ' || c == '\t' || c == '\n' || c == '\r')
            {
                continue;
            }
            if (c == ',')
            {
                input >> c;
            }
            string key = LoadString(input).AsString();
            input >> c;
            result.insert({move(key), LoadNode(input)});
        }
        if (c != '}')
        {
            throw ParsingError("Incorrect input"s);
        }
        return Node(move(result));
    }

    Node LoadBool(istream &input)
    {
        string value;
        getline(input, value, 'e');

        if (value.find("tru") != std::string::npos)
        {
            return Node(true);
        }
        else if (value.find("fals") != std::string::npos)
        {
            return Node(false);
        }
        else
        {
            throw ParsingError("Bool parsing error");
        }
    }

    Node LoadNumber(istream &input)
    {
        string parsed_num;

        // Считывает в parsed_num очередной символ из input
        auto read_char = [&parsed_num, &input]
        {
            parsed_num += static_cast<char>(input.get());
            if (!input)
            {
                throw ParsingError("Failed to read number from stream"s);
            }
        };

        // Считывает одну или более цифр в parsed_num из input
        auto read_digits = [&input, read_char]
        {
            if (!isdigit(input.peek()))
            {
                throw ParsingError("A digit is expected"s);
            }
            while (isdigit(input.peek()))
            {
                read_char();
            }
        };

        if (input.peek() == '-')
        {
            read_char();
        }
        // Парсим целую часть числа
        if (input.peek() == '0')
        {
            read_char();
            // После 0 в JSON не могут идти другие цифры
        }
        else
        {
            read_digits();
        }

        bool is_int = true;
        // Парсим дробную часть числа
        if (input.peek() == '.')
        {
            read_char();
            read_digits();
            is_int = false;
        }

        // Парсим экспоненциальную часть числа
        if (int ch = input.peek(); ch == 'e' || ch == 'E')
        {
            read_char();
            if (ch = input.peek(); ch == '+' || ch == '-')
            {
                read_char();
            }
            read_digits();
            is_int = false;
        }

        try
        {
            if (is_int)
            {
                // Сначала пробуем преобразовать строку в int
                try
                {
                    return Node(stoi(parsed_num));
                }
                catch (...)
                {
                    // В случае неудачи, например, при переполнении,
                    // код ниже попробует преобразовать строку в double
                }
            }
            return Node(stod(parsed_num));
        }
        catch (...)
        {
            throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
        }
    }

    Node LoadNode(istream &input)
    {
        if (!input)
            throw ParsingError(""s);
        char c;
        input >> c;

        if (c == 'n')
        {
            input.putback(c);
            return LoadNull(input);
        }
        else if (c == '"')
        {
            return LoadString(input);
        }
        else if (c == 't' || c == 'f')
        {
            input.putback(c);
            return LoadBool(input);
        }
        else if (c == '[')
        {
            return LoadArray(input);
        }
        else if (c == '{')
        {
            return LoadDict(input);
        }
        else
        {
            input.putback(c);
            return LoadNumber(input);
        }
        throw ParsingError("Incorrect input"s);
    }

    const Node::variant &Node::GetValue() const
    {
        return *this;
    }

    bool Node::IsInt() const
    {
        return holds_alternative<int>(*this);
    }

    bool Node::IsDouble() const
    {
        return (holds_alternative<int>(*this) || holds_alternative<double>(*this));
    }
    bool Node::IsPureDouble() const
    {
        return holds_alternative<double>(*this);
    }
    bool Node::IsBool() const
    {
        return holds_alternative<bool>(*this);
    }
    bool Node::IsString() const
    {
        return holds_alternative<string>(*this);
    }
    bool Node::IsNull() const
    {
        return holds_alternative<std::nullptr_t>(*this);
    }
    bool Node::IsArray() const
    {
        return holds_alternative<Array>(*this);
    }
    bool Node::IsMap() const
    {
        return holds_alternative<Dict>(*this);
    }
    int Node::AsInt() const
    {
        if (IsInt())
        {
            return get<int>(*this);
        }
        else
        {
            throw logic_error("");
        }
    }
    bool Node::AsBool() const
    {
        if (IsBool())
        {
            return get<bool>(*this);
        }
        else
        {
            throw logic_error("");
        }
    }
    double Node::AsDouble() const
    {
        if (IsPureDouble())
        {
            return get<double>(*this);
        }
        else
        {
            if (IsInt())
            {
                return get<int>(*this);
            }
            else
            {
                throw logic_error("");
            }
        }
    }
    const string &Node::AsString() const
    {
        if (IsString())
        {
            return get<string>(*this);
        }
        else
        {
            throw logic_error("");
        }
    }
    const Array &Node::AsArray() const
    {
        if (IsArray())
        {
            return get<Array>(*this);
        }
        else
        {
            throw logic_error("");
        }
    }
    const Dict &Node::AsMap() const
    {
        if (IsMap())
        {
            return get<Dict>(*this);
        }
        else
        {
            throw logic_error("");
        }
    }

    bool Node::operator==(const Node &node) const
    {
        return node.GetValue() == *this;
    }

    bool Node::operator!=(const Node &node) const
    {
        return !(*this == node);
    }

    Document::Document(Node root)
        : root_(move(root))
    {
    }

    const Node &Document::GetRoot() const
    {
        return root_;
    }

    bool Document::operator==(const Document &rhs)
    {
        return this->GetRoot() == rhs.GetRoot();
    }

    bool Document::operator!=(const Document &rhs)
    {
        return !(*this == rhs);
    }

    Document Load(istream &input)
    {
        if (!input)
            throw ParsingError("");
        return Document{LoadNode(input)};
    }

    void Print(const Document &doc, ostream &output)
    {
        visit(NodePrinter{output}, doc.GetRoot().GetValue());

        // Реализуйте функцию самостоятельно
    }

} // namespace json
