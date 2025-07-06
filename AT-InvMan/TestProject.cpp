#include "gtest/gtest.h"
#include "auth.h"
#include "db-conn.h"
#include "InventoryManager.h"
#include "main-functions.h"
#include "pos.h"
#include "salesLogger.h"
#include "session.h"
#include <sqlite3.h>


// Mock or open a test SQLite database connection as needed
sqlite3* openTestDB() {
    sqlite3* db;
    int rc = sqlite3_open(":memory:", &db);  // In-memory DB for testing
    if (rc != SQLITE_OK) {
        return nullptr;
    }
    // You can create test tables here if needed
    return db;
}

TEST(ScheduleTest, ScheduleExistsReturnsFalseForUnknownEmployee) {
    sqlite3* db = openTestDB();
    ASSERT_NE(db, nullptr);

    // No schedules inserted yet, so should return false
    EXPECT_FALSE(scheduleExists(db, 9999));

    sqlite3_close(db);
}

TEST(EmployeeTest, GenerateEmployeeIdLength) {
    sqlite3* db = openTestDB();
    ASSERT_NE(db, nullptr);

    std::string id = generateEmployeeId(db);
    EXPECT_EQ(id.length(), 5);  // Assuming 5-digit ID

    sqlite3_close(db);
}

TEST(AuthTest, IsAdminReturnsTrueForAdminUser) {
    // Set currentUser.position for test
    currentUser.position = "Administrator";
    EXPECT_TRUE(isAdmin());
}

TEST(AuthTest, IsAdminReturnsFalseForNonAdminUser) {
    currentUser.position = "Cashier";
    EXPECT_FALSE(isAdmin());
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
