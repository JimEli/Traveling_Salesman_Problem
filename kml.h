// Very basic kml file writer class.
#include <fstream>
#include <stack>
#include <string>

class KML
{
    size_t indention{ 0 };
    size_t indentStep{ 2 };
    std::ofstream ofs;
    std::stack<std::string> tagStack;

    std::string removeExt(const std::string& path)
    {
        if (path == "." || path == "..")
            return path;
        size_t pos = path.find_last_of("\\/.");
        if (pos != std::string::npos && path[pos] == '.')
            return path.substr(0, pos);

        return path;
    }

    void incIndent() { indention += indentStep; };
    void decIndent() { indention -= indentStep; };

    void indent() { for (size_t i = indention; i > 0; i -= 2, ofs << "  "); }

public:
    KML() = default;
    ~KML()
    {
        if (!tagStack.empty())
            std::cerr << "KML file error: closing file with open tags.\n";
        ofs << "</kml>\n";
        ofs.close();
    }

    bool open(const std::string fn)
    {
        std::string out = removeExt(fn);
        out.append(".kml");
        ofs.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        try
        {
            ofs.open(out.c_str(), std::ios::out);
            if (!ofs.is_open())
                return false;
        }
        catch (...)
        {
            return false;
        }

        ofs << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<kml>\n";
        return true;
    }

    void end()
    {
        if (!tagStack.empty())
            std::cerr << "KML file error: closing file with open tags.\n";
        ofs << "</kml>\n";
        ofs.close();
    }

    void tagOpen(const char* tag)
    {
        incIndent();
        indent();
        tagStack.push(tag);
        ofs << '<' << tag << ">\n";
    }

    void tagField(const char* field, bool newline = false)
    {
        indent();
        ofs << ' ' << field << ' ';
        if (newline)
            ofs << "\n";
    }

    void tagClose()
    {
        indent();
        decIndent();
        ofs << "</" << tagStack.top() << ">\n";
        tagStack.pop();
    }

    void tagStart(const char* tag)
    {
        incIndent();
        indent();
        tagStack.push(tag);
        ofs << '<' << tag << " ";
    }

    void tagEnd(const char* field) { ofs << field << ">\n"; }

    void tagLine(const std::string id, const std::string field)
    {
        indent();
        ofs << "<" << id << ">" << field << "</" << id << ">\n";
    }

    void write(const std::string str, bool indentLine = false)
    {
        if (indentLine)
            indent();
        ofs << str;
    }
};
