#ifndef TINYC_CODEGEN_SYMTABLE_H_
#define TINYC_CODEGEN_SYMTABLE_H_

#include <map>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>

#include "../parser/type/type.h"

namespace tinyc {

class SymbolTable {
public:
    SymbolTable() : outer_(nullptr), table_() {}

    SymbolTable(SymbolTable* outer) : outer_(outer), table_() {}

    void add(const std::string& name, const std::shared_ptr<Type>& type) {
        table_.insert({name, type});
    }

    void del(const std::string& name) { table_.erase(name); }

    const std::shared_ptr<Type>& get(const std::string& name) {
        try {
            return table_.at(name);
        } catch (std::out_of_range e) {
            if (outer_ != nullptr) {
                return outer_->get(name);
            }
            throw std::out_of_range("no such symbol exist: " + name);
        }
    }

private:
    SymbolTable* outer_;
    std::map<std::string, std::shared_ptr<Type>> table_;
};

}  // namespace tinyc

#endif  // TINYC_CODEGEN_SYMTABLE_H_
