#include "svg.h"

#include <utility>

namespace svg
{

    using namespace std::literals;

    void Object::Render(const RenderContext &context) const
    {
        context.RenderIndent();

        // Делегируем вывод тега своим подклассам
        RenderObject(context);

        context.out << std::endl;
    }

    // ---------- Circle ------------------

    Circle &Circle::SetCenter(Point center)
    {
        center_ = center;
        return *this;
    }

    Circle &Circle::SetRadius(double radius)
    {
        radius_ = radius;
        return *this;
    }

    void Circle::RenderObject(const RenderContext &context) const
    {
        auto &out = context.out;
        std::setprecision(20);
        out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
        out << "r=\""sv << radius_ << "\" "sv;
        // Выводим атрибуты, унаследованные от PathProps
        RenderAttrs(context.out);
        out << "/>"sv;
    }

    // ---------- Polyline ------------------

    Polyline &Polyline::AddPoint(Point point)
    {
        points_.push_back(point);
        return *this;
    }

    void Polyline::RenderObject(const RenderContext &context) const
    {
        auto &out = context.out;
        std::setprecision(20);
        out << "<polyline points=\""sv;
        bool first_time = true;
        for (const auto &point : points_)
        {
            if (first_time)
            {
                out << point.x << "," << point.y;
                first_time = false;
            }
            else
            {
                out << " " << point.x << "," << point.y;
            }
        }
        out << "\" "sv;
        // Выводим атрибуты, унаследованные от PathProps
        RenderAttrs(context.out);
        out << "/>"sv;
    }

    // ---------- Text ------------------

    Text &Text::SetPosition(Point pos)
    {
        pos_ = pos;
        return *this;
    };

    Text &Text::SetOffset(Point offset)
    {
        offset_ = offset;
        return *this;
    };

    Text &Text::SetFontSize(uint32_t size)
    {
        size_ = size;
        return *this;
    };

    Text &Text::SetFontFamily(std::string font_family)
    {
        font_family_ = font_family;
        return *this;
    };

    Text &Text::SetFontWeight(std::string font_weight)
    {
        font_weight_ = font_weight;
        return *this;
    };

    Text &Text::SetData(std::string data)
    {
        data_ = data;
        return *this;
    };

    void Text::RenderObject(const RenderContext &context) const
    {
        auto &out = context.out;
        std::setprecision(20);
        //<text x="35" y="20" dx="0" dy="6" font-size="12" font-family="Verdana" font-weight="bold">
        out << "<text"sv
            << " x=\""sv << pos_.x << "\" y=\""sv << pos_.y;
        out << "\" dx=\""sv << offset_.x << "\" dy=\""sv << offset_.y;
        out << "\" font-size=\""sv << size_ << "\""sv;
        if (!font_family_.empty())
        {
            out << " font-family=\""sv << font_family_ << "\""sv;
        }
        if (!font_weight_.empty())
        {
            out << " font-weight=\""sv << font_weight_ << "\""sv;
        }
        // Выводим атрибуты, унаследованные от PathProps
        RenderAttrs(context.out);
        out << ">"sv;
        std::string mod_data = data_;
        // Modifying data according to rules
        auto it = mod_data.find("&");
        while (it != std::string::npos)
        {
            out << it << std::endl;
            mod_data.erase(it, 1);
            mod_data.insert(it, "&amp;");
            it = mod_data.find("&", it + 1);
        }
        it = mod_data.find("\"");
        while (it != std::string::npos)
        {
            out << it << std::endl;
            mod_data.erase(it, 1);
            mod_data.insert(it, "&quot;");
            it = mod_data.find("\"", it + 1);
        }
        it = mod_data.find("\'");
        while (it != std::string::npos)
        {
            out << it << std::endl;
            mod_data.erase(it, 1);
            mod_data.insert(it, "&apos;");
            it = mod_data.find("\'", it + 1);
        }
        it = mod_data.find("<");
        while (it != std::string::npos)
        {
            out << it << std::endl;
            mod_data.erase(it, 1);
            mod_data.insert(it, "&lt;");
            it = mod_data.find("<", it + 1);
        }
        it = mod_data.find(">");
        while (it != std::string::npos)
        {
            out << it << std::endl;
            mod_data.erase(it, 1);
            mod_data.insert(it, "&gt;");
            it = mod_data.find(">", it + 1);
        }
        out << mod_data << "</text>"sv;
    };

    // ---------- Document ------------------

    void Document::AddPtr(std::unique_ptr<Object> &&obj)
    {
        objects_.push_back(std::move(obj));
    };

    void Document::Render(std::ostream &out) const
    {
        out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" << std::endl;
        out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">" << std::endl;
        RenderContext context(out);
        for (auto &object : objects_)
        {
            out << "  ";
            object->Render(context);
        }
        out << "</svg>";
    };

} // namespace svg