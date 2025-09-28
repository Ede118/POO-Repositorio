#include "IMessage.h"
#include "Message.h"
#include "AuthMessage.h"
#include "DataBag.h"

#include <cctype>
#include <string>
#include <memory>

using std::string;
using std::string_view;
using std::unique_ptr;
using std::make_unique;

namespace {
inline string trim_copy(string_view sv) {
    size_t i = 0, j = sv.size();
    while (i < j && std::isspace(static_cast<unsigned char>(sv[i]))) ++i;
    while (j > i && std::isspace(static_cast<unsigned char>(sv[j-1]))) --j;
    return string(sv.substr(i, j - i));
}
inline bool contains(string_view hay, string_view needle) {
    return hay.find(needle) != string_view::npos;
}
inline int parse_int_field(string_view s, string_view key, int fb = 0) {
    auto pos = s.find(key);
    if (pos == string_view::npos) return fb;
    pos = s.find(':', pos);
    if (pos == string_view::npos) return fb;
    ++pos;
    while (pos < s.size() && std::isspace(static_cast<unsigned char>(s[pos]))) ++pos;
    int sign = 1;
    if (pos < s.size() && (s[pos] == '-' || s[pos] == '+')) { sign = (s[pos] == '-') ? -1 : 1; ++pos; }
    long v = 0;
    while (pos < s.size() && std::isdigit(static_cast<unsigned char>(s[pos]))) { v = v*10 + (s[pos]-'0'); ++pos; }
    return static_cast<int>(sign * v);
}
inline string parse_string_field(string_view s, string_view key) {
    auto pos = s.find(key);
    if (pos == string_view::npos) return {};
    pos = s.find(':', pos);
    if (pos == string_view::npos) return {};
    pos = s.find('"', pos);
    if (pos == string_view::npos) return {};
    ++pos;
    string out;
    while (pos < s.size() && s[pos] != '"') {
        char c = s[pos++];
        if (c == '\\' && pos < s.size()) {
            char e = s[pos++];
            out.push_back(e == 'n' ? '\n' : e == 't' ? '\t' : e == '"' ? '"' : e == '\\' ? '\\' : e);
        } else out.push_back(c);
    }
    return out;
}
} // namespace

unique_ptr<IMessage> fromJSON(string_view jsonStr) {
    string s = trim_copy(jsonStr);
    int op = parse_int_field(s, "\"op\"");
    bool isAuth = contains(s, "\"type\":\"auth\"") || contains(s, "\"userID\"");
    DataBag data; // por ahora vacío; si después serializás columnas, lo llenás acá

    if (isAuth) {
        string user = parse_string_field(s, "\"userID\"");
        string dev  = parse_string_field(s, "\"deviceID\"");
        return make_unique<AuthMessage>(op, data, std::move(user), std::move(dev));
    } else {
        return make_unique<Message>(op, data);
    }
}
