#include "DataBag.h"
#include <stdexcept>
#include <sstream>
#include <type_traits>

// helper: escape básico para strings JSON
static std::string json_escape(std::string const& s) {
    std::string out; out.reserve(s.size() + 8);
    for (unsigned char c : s) {
        switch (c) {
            case '\"': out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\n': out += "\\n";  break;
            case '\t': out += "\\t";  break;
            case '\r': out += "\\r";  break;
            default:
                if (c < 0x20) { // control chars
                    char buf[7];
                    std::snprintf(buf, sizeof(buf), "\\u%04x", c);
                    out += buf;
                } else out += char(c);
        }
    }
    return out;
}

// addColumn (templado)
template <class TVec>
void DataBag::addColumn(std::string name, TVec data) {
    static_assert(
        std::is_same_v<TVec, ColInt>  ||
        std::is_same_v<TVec, ColReal> ||
        std::is_same_v<TVec, ColText> ||
        std::is_same_v<TVec, ColBool>,
        "Tipo de columna no soportado"
    );
    if (rows_ == 0) rows_ = data.size();
    if (data.size() != rows_)
        throw std::invalid_argument("DataBag: tamaño de columna no coincide con rows");

    columns_[std::move(name)] = Column(std::move(data));
}

// col<T>
template <class TVec>
const TVec& DataBag::col(std::string const& name) const {
    auto it = columns_.find(name);
    if (it == columns_.end()) throw std::out_of_range("DataBag: columna inexistente");
    return std::get<TVec>(it->second);
}

// selectColumns
DataBag DataBag::selectColumns(std::vector<std::string> const& names) const {
    DataBag out;
    out.rows_ = rows_;
    for (auto const& n : names) {
        auto it = columns_.find(n);
        if (it == columns_.end()) throw std::out_of_range("selectColumns: columna no existe");
        out.columns_.emplace(n, it->second); // copia barata (variant y vectors)
    }
    return out;
}

// selectRows
DataBag DataBag::selectRows(std::vector<std::size_t> const& idxs) const {
    DataBag out;
    out.rows_ = idxs.size();
    for (auto const& [name, col] : columns_) {
        auto build = [&](auto const& vec) {
            using TVec = std::decay_t<decltype(vec)>;
            TVec filtered; filtered.reserve(idxs.size());
            for (auto i : idxs) {
                if (i >= vec.size()) throw std::out_of_range("selectRows: índice fuera de rango");
                filtered.push_back(vec[i]);
            }
            out.columns_.emplace(name, std::move(filtered));
        };
        std::visit(build, col);
    }
    return out;
}

// toJSON de solo "data"
std::string DataBag::toJSON() const {
    std::ostringstream os;
    os << "{\"rows\":" << rows_ << ",\"columns\":{";
    bool firstCol = true;
    for (auto const& [name, col] : columns_) {
        if (!firstCol) os << ',';
        firstCol = false;
        os << '\"' << json_escape(name) << "\":";
        std::visit([&](auto const& vec){
            // imprime arreglo JSON según tipo
            os << '[';
            for (std::size_t i = 0; i < vec.size(); ++i) {
                if (i) os << ',';
                using T = typename std::decay_t<decltype(vec)>::value_type;
                if constexpr (std::is_same_v<T, std::string>) {
                    os << '\"' << json_escape(vec[i]) << '\"';
                } else if constexpr (std::is_same_v<T, bool>) {
                    os << (vec[i] ? "true" : "false");
                } else {
                    os << vec[i];
                }
            }
            os << ']';
        }, col);
    }
    os << "}}";
    return os.str();
}

// Explicitar templates que usamos (evita linker errors)
template void DataBag::addColumn<DataBag::ColInt>(std::string, DataBag::ColInt);
template void DataBag::addColumn<DataBag::ColReal>(std::string, DataBag::ColReal);
template void DataBag::addColumn<DataBag::ColText>(std::string, DataBag::ColText);
template void DataBag::addColumn<DataBag::ColBool>(std::string, DataBag::ColBool);

template const DataBag::ColInt&  DataBag::col<DataBag::ColInt>(std::string const&) const;
template const DataBag::ColReal& DataBag::col<DataBag::ColReal>(std::string const&) const;
template const DataBag::ColText& DataBag::col<DataBag::ColText>(std::string const&) const;
template const DataBag::ColBool& DataBag::col<DataBag::ColBool>(std::string const&) const;
