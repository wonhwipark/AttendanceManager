#if defined(_DEBUG)
#include "attendance.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <vector>
#include <string>

using ::testing::Test;

class FixtureAttendance: public Test {

protected:
    AttendanceInfo info;
    AttendanceEngine engine;

    FixtureAttendance() : info(), engine(info) {}
};

struct CapturingReporter : public InterfaceRemoveReporter {
    std::vector<std::string> removed;
    void OnRemoved(const std::string& name) override {
        removed.push_back(name);
    }
};

TEST_F(FixtureAttendance, TestGetBasePoint) {
    EXPECT_EQ(GetBasePoint(MONDAY), 1);
    EXPECT_EQ(GetBasePoint(TUESDAY), 1);
    EXPECT_EQ(GetBasePoint(WEDNESDAY), 3);
    EXPECT_EQ(GetBasePoint(SATURDAY), 2);
    EXPECT_EQ(GetBasePoint(SUNDAY), 2);
    EXPECT_EQ(GetBasePoint(INVALID_DAY), 0);
}

TEST_F(FixtureAttendance, TestGetIndexFromDay) {
    EXPECT_EQ(GetIndexFromDay("monday"), MONDAY);
    EXPECT_EQ(GetIndexFromDay("friday"), FRIDAY);
    EXPECT_EQ(GetIndexFromDay("sunday"), SUNDAY);
    EXPECT_EQ(GetIndexFromDay("Mon"), INVALID_DAY);
}

TEST_F(FixtureAttendance, TestCalcBonus) {
    EXPECT_EQ(CalcBonus(10, 9), 10);
    EXPECT_EQ(CalcBonus(9, 10), 10);
    EXPECT_EQ(CalcBonus(10, 10), 20);
}

TEST_F(FixtureAttendance, TestCalcGrade) {
    EXPECT_EQ(CalcGrade(29), NORMAL);
    EXPECT_EQ(CalcGrade(30), SILVER);
    EXPECT_EQ(CalcGrade(50), GOLD);
}

TEST_F(FixtureAttendance, TestAssignAndUpdatePoints) {
    engine.AssignId("alice");
    engine.AssignId("bob");

    int idA = engine.GetIdFromName("alice");
    int idB = engine.GetIdFromName("bob");
    ASSERT_EQ(idA, 1);
    ASSERT_EQ(idB, 2);

    engine.UpdateSpecialDays(idA, WEDNESDAY);
    engine.UpdateDayAndPoints(idA, WEDNESDAY);
    engine.UpdateDayAndPoints(idA, MONDAY);

    engine.UpdateSpecialDays(idB, SATURDAY);
    engine.UpdateDayAndPoints(idB, SATURDAY);

    engine.UpdatePointAndGrades();

    EXPECT_EQ(info.points[idA], 4 + 0 /*bonus*/);
    EXPECT_EQ(info.grade[idA], NORMAL);

    EXPECT_EQ(info.points[idB], 2 /*base*/ + 0 /*bonus*/);
    EXPECT_EQ(info.grade[idB], NORMAL);
}

TEST_F(FixtureAttendance, TestBonusAndGradeApplied) {
    engine.AssignId("c");
    int id = engine.GetIdFromName("c");

    for (int i = 0; i < 10; ++i) {
        engine.UpdateSpecialDays(id, WEDNESDAY);
        engine.UpdateDayAndPoints(id, WEDNESDAY);
    }

    for (int i = 0; i < 10; ++i) engine.UpdateDayAndPoints(id, MONDAY);

    engine.UpdatePointAndGrades(); // +10 bonus (wed) => total 50 -> GOLD
    EXPECT_EQ(info.points[id], 50);
    EXPECT_EQ(info.grade[id], GOLD);
}

TEST_F(FixtureAttendance, TestReportRemoved_UsesCapturingReporter) {
    engine.AssignId("idle");     // never attends -> should be removed
    engine.AssignId("active");   // attends on weekend -> not removed

    int idle = engine.GetIdFromName("idle");
    int active = engine.GetIdFromName("active");

    engine.UpdateSpecialDays(active, SATURDAY);
    engine.UpdateDayAndPoints(active, SATURDAY);
    engine.UpdatePointAndGrades();

    CapturingReporter reporter;
    engine.ReportRemoved(reporter);

    ASSERT_EQ(reporter.removed.size(), 1u);
    EXPECT_EQ(reporter.removed[0], "idle");
}

TEST_F(FixtureAttendance, TestOutputReport)
{
    info.id_cnt = 1;
    info.names[1] = "Alice";
    info.points[1] = 50;
    info.grade[1] = GOLD;

    info.names[2] = "Charlie";
    info.points[2] = 30;
    info.grade[2] = SILVER;
    OutputReport(info);
}

TEST_F(FixtureAttendance, TestOutputRemovePlayer)
{
    info.id_cnt = 1;
    info.names[1] = "Alice";
    info.points[1] = 50;
    info.grade[1] = NORMAL;
    OutputRemovePlayer(info);
}

TEST_F(FixtureAttendance, TestReadAttendanceList)
{
	std::string filename = "attendance_weekday_500.txt";
    bool isOk = ReadAttendanceList(filename, engine, 500);
    EXPECT_EQ(isOk, true);
}

#endif // _DEBUG