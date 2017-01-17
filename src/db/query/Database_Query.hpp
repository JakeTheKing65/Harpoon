#ifndef DATABASEQUERY_H
#define DATABASEQUERY_H

#include "Database_Query_Base.hpp"

#include "Database_Details_Create.hpp"
#include "Database_Details_Select.hpp"
#include "Database_Details_Insert.hpp"


namespace Query {
    template<class... T>
    details::TmpQueryCreate_CREATE create(T&&... t) {
        details::TmpQueryCreate_CREATE temp(std::forward<T>(t)...);
        return temp;
    }

    template<class... T>
    details::TmpQuerySelect_SELECT select(T&&... t) {
        details::TmpQuerySelect_SELECT temp;
        temp.select(std::forward<T>(t)...);
        return temp;
    }

    template<class... T>
    details::TmpQueryInsert_INSERT insert(T&&... t) {
        details::TmpQueryInsert_INSERT temp;
        return temp;
    }
}

#endif
