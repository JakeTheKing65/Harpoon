#include "Postgres.hpp"
#include "utils/ModuleProvider.hpp"
#include "event/EventQuit.hpp"
#include "event/EventInit.hpp"
#include "event/EventDatabaseQuery.hpp"
#include "utils/Ini.hpp"

#include <iostream>
#include <soci/soci.h>

using namespace std;


namespace Database {

    PROVIDE_MODULE("database", "postgres", Postgres);


    struct Postgres_Impl {
        EventQueue* appQueue;
        Postgres_Impl(EventQueue* appQueue) : appQueue{appQueue} {};
        bool onEvent(std::shared_ptr<IEvent> event);
        void handleQuery(std::shared_ptr<IEvent> event);

        void query_setupDatabase(const Database::Query& query);
        void query_insert(const Database::Query& query);
        void query_fetch(const Database::Query& query);
        void query_delete(const Database::Query& query);

        shared_ptr<soci::session> sqlSession;
        static const std::map<std::string, std::string> typeMap;
        std::list<std::shared_ptr<IEvent>> heldBackQueries;

        friend Postgres;
    };

    Postgres::Postgres(EventQueue* appQueue)
        : EventLoop{
            {},
            {
                &EventGuard<IDatabaseEvent>
            }
        }
        , impl{make_shared<Postgres_Impl>(appQueue)}
    {
    }

    bool Postgres::onEvent(std::shared_ptr<IEvent> event) {
        return impl->onEvent(event);
    }

    const map<string, string> Postgres_Impl::typeMap {
        {"id", "serial"},
        {"timestamp", "timestamp"},
        {"int", "integer"},
        {"text", "text"},
        {"bool", "boolean"}
    };

    void Postgres_Impl::query_setupDatabase(const Database::Query& query) {
        auto once(sqlSession->once);
        once << "CREATE TABLE IF NOT EXISTS " << query.getTable() << " "
             << "(";
        bool first = true;
        for (auto& op : query.getOperations()) {
            if (op.getOperation() != Database::OperationType::Assign)
                continue;
            if (first) {
                first = false;
            } else {
                once << ", ";
            }
            once << typeMap.at(op.getLeft()) << " " << op.getRight();
        }
        once << ")";
    }

    void Postgres_Impl::query_insert(const Database::Query& query) {
        auto once(sqlSession->once);
        once << "INSERT INTO " << query.getTable() << " "
             << "(";
        bool first = true;
        for (auto& op : query.getOperations()) {
            if (!first) {
                once << ", ";
                first = false;
            }
            once << op.getLeft();
        }
        once << ") VALUES (";
        size_t index = 0;
        for (auto& op : query.getOperations()) {
            if (op.getOperation() != Database::OperationType::Assign)
                continue;
            if (index == 0)
                once << ", ";
            once << ":op" << index;
            ++index;
        }
        once << ")";
        for (auto& op : query.getOperations())
            once, soci::use(op.getRight());
    }

    void Postgres_Impl::query_fetch(const Database::Query& query) {
#warning Postgres QueryType::Fetch stub
        auto once(sqlSession->once);
        once << "SELECT ";
        bool first = true;
        for (auto& column : query.getColumns()) {
            if (first) {
                first = false;
            } else {
                once << ", ";
            }
            once << column;
        }
        once << " FROM " << query.getTable();

        bool where = false, join = false;
        for (auto& op : query.getOperations()) {
            if (op.getOperation() == Database::OperationType::CompareAnd
                || op.getOperation() == Database::OperationType::CompareOr)
                where = true;
            if (op.getOperation() == Database::OperationType::Join)
                join = true;
        }
    }

    void Postgres_Impl::query_delete(const Database::Query& query) {
#warning Postgres QueryType::Delete stub
    }

    void Postgres_Impl::handleQuery(std::shared_ptr<IEvent> event) {
        auto db = event->as<EventDatabaseQuery>();
        for (const auto& query : db->getQueries()) {
            switch (query.getType()) {
            case Database::QueryType::SetupTable:
                query_setupDatabase(query);
                break;
            case Database::QueryType::Fetch:
                query_fetch(query);
                break;
            case Database::QueryType::Insert:
                query_insert(query);
                break;
            case Database::QueryType::Delete:
                query_delete(query);
                break;
            }
        }
    }

    bool Postgres_Impl::onEvent(std::shared_ptr<IEvent> event) {
        UUID eventType = event->getEventUuid();
        if (eventType == EventQuit::uuid) {
            return false;
        } else if (eventType == EventDatabaseQuery::uuid) {
            if (sqlSession) {
                handleQuery(event);
            } else {
                heldBackQueries.push_back(event);
            }
        } else if (eventType == EventInit::uuid) {
            using namespace soci;

            string host,
                port,
                username,
                password,
                database;

            Ini dbIni("config/postgres.ini");
            auto& auth = dbIni.expectCategory("auth");
            dbIni.getEntry(auth, "host", host);
            dbIni.getEntry(auth, "port", port);
            dbIni.getEntry(auth, "username", username);
            dbIni.getEntry(auth, "password", password);
            dbIni.getEntry(auth, "database", database);

#warning check if values are 'evil'
            stringstream login;
            login << "postgresql://";
            if (host.size() > 0)
                login << "host=" << host << " ";
            if (port.size() > 0)
                login << "port=" << port << " ";
            login << "dbname=" << database << " "
                  << "user=" << username << " "
                  << "password=" << password;

            try {
                sqlSession = make_shared<soci::session>(login.str());
            } catch(soci_error& e) {
                cout << "Could not connect to database server. Reason: " << endl << e.what() << endl << endl;
                return false;
            }

            for (auto query : heldBackQueries)
                handleQuery(query);
            heldBackQueries.clear();
        }
        return true;
    }

}
