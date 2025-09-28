#ifndef DATABAG_H
#define DATABAG_H

#include <cstddef>
#include <string>
#include <string_view>
#include <unordered_map>
#include <variant>
#include <vector>

class DataBag {
public:
    using ColInt    = std::vector<int>;
    using ColReal   = std::vector<double>;
    using ColText   = std::vector<std::string>;
    using ColBool   = std::vector<bool>;
    using Column    = std::variant<ColInt, ColReal, ColText, ColBool>;

    std::size_t rowsCount() const noexcept { return rows_; }
    std::size_t columnsCount() const noexcept { return columns_.size(); }
    bool empty() const noexcept { return rows_ == 0 && columns_.empty(); }

    // Agregar columna nueva (reemplaza si existe). Valida tamaño.
    template <class TVec>
    void addColumn(std::string name, TVec data);

    // Acceso tipado (lanza std::bad_variant_access si el tipo no coincide)
    template <class TVec>
    const TVec& col(std::string const& name) const;

    bool has(std::string const& name) const { return columns_.count(name) > 0; }

    DataBag selectColumns(std::vector<std::string> const& names) const;
    DataBag selectRows(std::vector<std::size_t> const& idxs) const;

    // Serialización solo del bloque "data": { "rows":..., "columns":{...} }
    std::string toJSON() const;

private:
    std::size_t rows_ = 0;
    std::unordered_map<std::string, Column> columns_;
};

#endif
