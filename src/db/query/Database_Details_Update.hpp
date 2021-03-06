#ifndef DATABASE_DETAILS_UPDATE_H
#define DATABASE_DETAILS_UPDATE_H

#include <memory>
#include <utility>
#include <exception>

#include "Database_QueryUpdate_Store.hpp"


namespace details {
    using namespace Query;

    struct TmpQueryUpdate_FILTER {
        std::unique_ptr<QueryUpdate_Store> store;

        explicit inline TmpQueryUpdate_FILTER(std::unique_ptr<QueryUpdate_Store> store)
            : store{std::move(store)}
        { }

        inline operator Query::Update() {
            return std::move(store);
        }

        inline TmpQueryUpdate_FILTER& where(StatementPtr&& filter) {
            if (store->filter)
                throw std::runtime_error("Query already contains filter");
            store->filter = std::move(filter);
            return *this;
        }
    };

    struct TmpQueryUpdate_DATA {
        std::unique_ptr<QueryUpdate_Store> store;

        explicit inline TmpQueryUpdate_DATA(std::unique_ptr<QueryUpdate_Store> store)
            : store{std::move(store)}
        { }

        inline operator Query::Update() {
            return std::move(store);
        }

        TmpQueryUpdate_DATA& data(std::vector<std::string>&& data)
        {
            if (store->data.size() == 0) {
                store->data.swap(data);
            } else {
                store->data.reserve(store->data.size()+std::distance(data.cbegin(), data.cend()));
                store->data.insert(store->data.end(), data.cbegin(), data.cend());
            }
            return *this;
        }

        TmpQueryUpdate_DATA& data(std::vector<std::string>::iterator start,
                                  std::vector<std::string>::iterator end)
        {
            store->data.reserve(store->data.size()+std::distance(start, end));
            store->data.insert(store->data.end(), start, end);
            return *this;
        }

        template<class... T>
        TmpQueryUpdate_FILTER where(T&&... t) {
            auto temp = TmpQueryUpdate_FILTER(std::move(store));
            temp.where(std::forward<T>(t)...);
            return temp;
        }
    };

    struct TmpQueryUpdate_FORMAT {
        std::unique_ptr<QueryUpdate_Store> store;

        explicit inline TmpQueryUpdate_FORMAT(std::unique_ptr<QueryUpdate_Store> store)
            : store{std::move(store)}
        { }

        template<class... T>
        TmpQueryUpdate_DATA data(T&&... t) {
            auto temp = TmpQueryUpdate_DATA(std::move(store));
            temp.data(std::forward<T>(t)...);
            return temp;
        }
    };

    struct TmpQueryUpdate_INTO {
        std::unique_ptr<QueryUpdate_Store> store;

        explicit inline TmpQueryUpdate_INTO(std::unique_ptr<QueryUpdate_Store> store)
            : store{std::move(store)}
        { }

        inline TmpQueryUpdate_FORMAT format(const std::string& column) {
            store->format.emplace_back(column);
            return TmpQueryUpdate_FORMAT(std::move(store));
        }

        template<class... T>
        inline TmpQueryUpdate_FORMAT format(const std::string& column, T&&... t) {
            store->format.emplace_back(column);
            return format(std::forward<T>(t)...);
        }

        inline TmpQueryUpdate_FORMAT into(const std::string& table) {
            if (!store->table.empty())
                throw std::runtime_error("Table for query was already defined");
            store->table = table;
            auto temp = TmpQueryUpdate_FORMAT(std::move(store));
            return temp;
        }
    };

    struct TmpQueryUpdate_UPDATE {
        std::unique_ptr<QueryUpdate_Store> store;

        template<class T>
        TmpQueryUpdate_UPDATE(T&& table)
            : store{cpp11::make_unique<QueryUpdate_Store>()}
        {
            store->table = std::forward<T>(table);
        }

        template<class... T>
        TmpQueryUpdate_FORMAT format(T&&... t) {
            auto temp = TmpQueryUpdate_INTO(std::move(store));
            return temp.format(std::forward<T>(t)...);
        }
    };
}

#endif
