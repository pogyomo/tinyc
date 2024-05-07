#ifndef TINYC_PREPROCESS_MACRO_H_
#define TINYC_PREPROCESS_MACRO_H_

#include <map>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "../lexer/token.h"

namespace tinyc {

// A macro which can expand to list of token with/without arguments.
class Macro {
public:
    virtual ~Macro() {}
    virtual std::vector<std::shared_ptr<Token>> expand(
        const std::vector<std::vector<std::shared_ptr<Token>>>& args) const = 0;
    virtual std::string debug() const = 0;
};

class ObjectMacro : public Macro {
public:
    ObjectMacro(const std::string& name) : name_(name), body_() {}

    ObjectMacro(const std::string& name,
                const std::vector<std::shared_ptr<Token>>& body)
        : name_(name), body_(body) {}

    std::vector<std::shared_ptr<Token>> expand(
        const std::vector<std::vector<std::shared_ptr<Token>>>& args)
        const override {
        if (!args.empty()) {
            throw std::runtime_error("passing arguments to object macro");
        }
        return body_;
    }

    std::string debug() const override {
        std::stringstream ss;
        ss << name_;
        for (const auto& tk : body_) ss << " " << tk->debug();
        return ss.str();
    }

private:
    const std::string name_;
    const std::vector<std::shared_ptr<Token>> body_;
};

class FunctionMacro : public Macro {
public:
    FunctionMacro(const std::string& name,
                  const std::vector<std::string>& params,
                  const std::vector<std::shared_ptr<Token>>& body)
        : name_(name), params_(params), body_(body) {}

    std::vector<std::shared_ptr<Token>> expand(
        const std::vector<std::vector<std::shared_ptr<Token>>>& args)
        const override {
        if (params_.size() != args.size()) {
            std::stringstream ss;
            ss << "incorrect number of arguments to function macro: ";
            ss << "expected " << params_.size() << ", but got " << args.size();
            throw std::runtime_error(ss.str());
        }

        std::vector<std::shared_ptr<Token>> res;
        for (const auto& tk : body_) {
            if (tk->kind() != TokenKind::Identifier) {
                res.emplace_back(tk);
            }
            bool replaced = false;
            for (int i = 0; i < params_.size(); i++) {
                auto id = std::static_pointer_cast<ValueToken<std::string>>(tk);
                if (id->value() == params_[i]) {
                    replaced = true;
                    res.insert(res.end(), args[i].begin(), args[i].end());
                    break;
                }
            }
            if (!replaced) {
                res.emplace_back(tk);
            }
        }
        return res;
    }

    std::string debug() const override {
        std::stringstream ss;
        ss << name_ << "(";
        if (!params_.empty()) {
            ss << params_[0];
            for (int i = 1; i < params_.size(); i++) ss << ", " << params_[i];
        }
        ss << ")";
        for (const auto& tk : body_) ss << " " << tk->debug();
        return ss.str();
    }

private:
    const std::string name_;
    const std::vector<std::string> params_;
    const std::vector<std::shared_ptr<Token>> body_;
};

class MacroTable {
public:
    MacroTable() : macros_() {}

    void add_macro(const std::string& name, std::shared_ptr<Macro>& macro) {
        macros_.insert({name, macro});
    }

    const std::shared_ptr<Macro>& get_macro(const std::string& name) {
        try {
            return macros_.at(name);
        } catch (std::out_of_range e) {
            std::stringstream ss;
            ss << "no such macro exist: " << name;
            throw std::out_of_range(ss.str());
        }
    }

private:
    std::map<std::string, std::shared_ptr<Macro>> macros_;
};

}  // namespace tinyc

#endif  // TINYC_PREPROCESS_MACRO_H_
