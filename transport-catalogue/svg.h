#pragma once

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <sstream>
#include <variant>
#include <vector>

namespace svg
{

    struct StringColor
    {
        StringColor() = default;

        StringColor(std::string color_)
            : color(color_)
        {
        }

        std::string color;
    };

    struct RGB
    {
        RGB() = default;

        RGB(int red_, int green_, int blue_)
            : red(red_), green(green_), blue(blue_)
        {
        }

        int red = 0;
        int green = 0;
        int blue = 0;
    };

    struct RGBA
    {
        RGBA() = default;

        RGBA(int red_, int green_, int blue_, double opacity_)
            : red(red_), green(green_), blue(blue_), opacity(opacity_)
        {
        }

        int red = 0;
        int green = 0;
        int blue = 0;
        double opacity = 1.0;
    };

    using Color = std::variant<StringColor, RGB, RGBA>;

    struct ColorPrinter
    {

        void operator()(RGB rgb)
        {
            using namespace std::literals;

            out << "rgb("sv;
            out << std::to_string(rgb.red) << ","sv;
            out << std::to_string(rgb.green) << ","sv;
            out << std::to_string(rgb.blue) << ")"sv;
        }

        void operator()(RGBA rgba)
        {
            using namespace std::literals;

            out << "rgba("sv;
            out << std::to_string(rgba.red) << ","sv;
            out << std::to_string(rgba.green) << ","sv;
            out << std::to_string(rgba.blue) << ","sv;
            out << rgba.opacity << ")"sv;
        }

        void operator()(StringColor color)
        {
            out << color.color;
        }

        std::ostream &out;
    };

    inline std::ostream &operator<<(std::ostream &out, Color color)
    {
        using namespace std::literals;
        std::visit(ColorPrinter{out}, color);
        return out;
    }
    struct Point
    {
        Point() = default;
        Point(double x_, double y_)
            : x(x_), y(y_)
        {
        }
        double x = 0;
        double y = 0;
    };

    /*
     * Вспомогательная структура, хранящая контекст для вывода SVG-документа с отступами.
     * Хранит ссылку на поток вывода, текущее значение и шаг отступа при выводе элемента
     */
    struct RenderContext
    {
        RenderContext(std::ostream &out_)
            : out(out_)
        {
        }

        RenderContext(std::ostream &out_, int indent_step_, int indent_ = 0)
            : out(out_), indent_step(indent_step_), indent(indent_)
        {
        }

        RenderContext Indented() const
        {
            return {out, indent_step, indent + indent_step};
        }

        void RenderIndent() const
        {
            for (int i = 0; i < indent; ++i)
            {
                out.put(' ');
            }
        }

        std::ostream &out;
        int indent_step = 0;
        int indent = 0;
    };

    enum class StrokeLineCap
    {
        BUTT,
        ROUND,
        SQUARE,
    };

    enum class StrokeLineJoin
    {
        ARCS,
        BEVEL,
        MITER,
        MITER_CLIP,
        ROUND,
    };

    inline std::ostream &operator<<(std::ostream &out, StrokeLineCap line_cap)
    {
        if (line_cap == StrokeLineCap::BUTT)
        {
            out << "butt";
        }
        if (line_cap == StrokeLineCap::ROUND)
        {
            out << "round";
        }
        if (line_cap == StrokeLineCap::SQUARE)
        {
            out << "square";
        }
        return out;
    }

    inline std::ostream &operator<<(std::ostream &out, StrokeLineJoin line_join)
    {
        if (line_join == StrokeLineJoin::ARCS)
        {
            out << "arcs";
        }
        if (line_join == StrokeLineJoin::BEVEL)
        {
            out << "bevel";
        }
        if (line_join == StrokeLineJoin::MITER)
        {
            out << "miter";
        }
        if (line_join == StrokeLineJoin::MITER_CLIP)
        {
            out << "miter-clip";
        }
        if (line_join == StrokeLineJoin::ROUND)
        {
            out << "round";
        }
        return out;
    }

    /*
     * Абстрактный базовый класс Object служит для унифицированного хранения
     * конкретных тегов SVG-документа
     * Реализует паттерн "Шаблонный метод" для вывода содержимого тега
     */
    class Object
    {
    public:
        void Render(const RenderContext &context) const;

        virtual ~Object() = default;

    private:
        virtual void RenderObject(const RenderContext &context) const = 0;
    };

    class ObjectContainer
    {
    public:
        template <typename SomeObject>
        void Add(SomeObject object)
        {
            AddPtr(std::make_unique<SomeObject>(std::move(object)));
        }

        virtual void AddPtr(std::unique_ptr<Object> &&obj) = 0;
    };

    // Интерфейс Drawable задаёт объекты, которые можно нарисовать с помощью Graphics
    class Drawable
    {
    public:
        virtual void Draw(ObjectContainer &obj) const = 0;

        virtual ~Drawable() = default;
    };

    template <typename Owner>
    class PathProps
    {
    public:
        Owner &SetFillColor(Color color)
        {
            fill_color_ = std::move(color);
            return AsOwner();
        }
        Owner &SetStrokeColor(Color color)
        {
            stroke_color_ = std::move(color);
            return AsOwner();
        }
        Owner &SetStrokeWidth(double width)
        {
            stroke_width_ = std::move(width);
            return AsOwner();
        }
        Owner &SetStrokeLineCap(StrokeLineCap line_cap)
        {
            stroke_linecap_ = std::move(line_cap);
            return AsOwner();
        }
        Owner &SetStrokeLineJoin(StrokeLineJoin line_join)
        {
            stroke_linejoin_ = std::move(line_join);
            return AsOwner();
        }

    protected:
        ~PathProps() = default;

        // Метод RenderAttrs выводит в поток общие для всех путей атрибуты fill и stroke
        void RenderAttrs(std::ostream &out) const
        {
            using namespace std::literals;

            if (fill_color_)
            {
                out << " fill=\""sv << *fill_color_ << "\""sv;
            }
            if (stroke_color_)
            {
                out << " stroke=\""sv << *stroke_color_ << "\""sv;
            }
            if (stroke_width_)
            {
                out << " stroke-width=\""sv << *stroke_width_ << "\""sv;
            }
            if (stroke_linecap_)
            {
                out << " stroke-linecap=\""sv << *stroke_linecap_ << "\""sv;
            }
            if (stroke_linejoin_)
            {
                out << " stroke-linejoin=\""sv << *stroke_linejoin_ << "\""sv;
            }
        }

    private:
        Owner &AsOwner()
        {
            // static_cast безопасно преобразует *this к Owner&,
            // если класс Owner — наследник PathProps
            return static_cast<Owner &>(*this);
        }

        std::optional<Color> fill_color_;
        std::optional<Color> stroke_color_;
        std::optional<double> stroke_width_;
        std::optional<StrokeLineCap> stroke_linecap_;
        std::optional<StrokeLineJoin> stroke_linejoin_;
    };

    /*
     * Класс Circle моделирует элемент <circle> для отображения круга
     * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/circle
     */
    class Circle final : public Object, public PathProps<Circle>
    {
    public:
        Circle() = default;
        Circle &SetCenter(Point center);
        Circle &SetRadius(double radius);

    private:
        void RenderObject(const RenderContext &context) const override;

        Point center_ = {0.0, 0.0};
        double radius_ = 1.0;
    };

    /*
     * Класс Polyline моделирует элемент <polyline> для отображения ломаных линий
     * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/polyline
     */
    class Polyline final : public Object, public PathProps<Polyline>
    {
    public:
        Polyline() = default;
        // Добавляет очередную вершину к ломаной линии
        Polyline &AddPoint(Point point);

        /*
         * Прочие методы и данные, необходимые для реализации элемента <polyline>
         */
    private:
        void RenderObject(const RenderContext &context) const override;

        std::vector<Point> points_;
    };

    /*
     * Класс Text моделирует элемент <text> для отображения текста
     * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/text
     */
    class Text final : public Object, public PathProps<Text>
    {
    public:
        Text() = default;

        // Задаёт координаты опорной точки (атрибуты x и y)
        Text &SetPosition(Point pos);

        // Задаёт смещение относительно опорной точки (атрибуты dx, dy)
        Text &SetOffset(Point offset);

        // Задаёт размеры шрифта (атрибут font-size)
        Text &SetFontSize(uint32_t size);

        // Задаёт название шрифта (атрибут font-family)
        Text &SetFontFamily(std::string font_family);

        // Задаёт толщину шрифта (атрибут font-weight)
        Text &SetFontWeight(std::string font_weight);

        // Задаёт текстовое содержимое объекта (отображается внутри тега text)
        Text &SetData(std::string data);

        // Прочие данные и методы, необходимые для реализации элемента <text>
    private:
        void RenderObject(const RenderContext &context) const override;

        Point pos_ = {0.0, 0.0};
        Point offset_ = {0.0, 0.0};
        uint32_t size_ = 1;
        std::string font_family_;
        std::string font_weight_;
        std::string data_ = "";
    };

    class Document : public ObjectContainer
    {
    public:
        Document() = default;
        /*
         Метод Add добавляет в svg-документ любой объект-наследник svg::Object.
         Пример использования:
         Document doc;
         doc.Add(Circle().SetCenter({20, 30}).SetRadius(15));
        */
        // void Add(???);
        template <typename SomeObject>
        void Add(SomeObject object)
        {
            objects_.push_back(std::make_unique<SomeObject>(std::move(object)));
        }
        // Добавляет в svg-документ объект-наследник svg::Object
        void AddPtr(std::unique_ptr<Object> &&obj) override;

        // Выводит в ostream svg-представление документа
        void Render(std::ostream &out) const;

    private:
        std::vector<std::unique_ptr<Object>> objects_;

        // Прочие методы и данные, необходимые для реализации класса Document
    };
} // namespace svg