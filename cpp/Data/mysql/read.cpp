#include <stdlib.h>
#include <iostream>

#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

using namespace std;

int main(void)
{
    try {
        sql::Driver *driver;
        sql::Connection *conn;
        sql::Statement *stmt;
        sql::ResultSet *res;

        driver = get_driver_instance(); // Create a connection
        conn = driver->connect("tcp://127.0.0.1:3306", "user", "passw");
        conn->setSchema("main_storage"); // database

        stmt = conn->createStatement();
        res = stmt->executeQuery("SELECT * FROM users"); // statement
        while (res->next()) {
            cout << res->getInt(1) << ':' << res->getString(2) << ':' << res->getString(3) << endl;
        }

        delete res;
        delete stmt;
        delete conn;

    } catch (sql::SQLException &e) {
        cout << "# ERR: SQLException in " << __FILE__ << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
        cout << "# ERR: " << e.what() << " (MySQL error code: " << e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )" << endl;
    }

    return EXIT_SUCCESS;
}
