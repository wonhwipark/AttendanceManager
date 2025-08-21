#if defined(_DEBUG)
#pragma once
#include <string>
#include <map>

// max
static const int MAX_NUM_OF_ATTEND_LIST = 500;
static const int MAX_NUM_OF_ID          = 100;
static const int MAX_NUM_OF_WEEK        = 100;

enum Grade_e { // Grade
    NORMAL = 0,
    GOLD = 1,
    SILVER = 2
};

enum Day_e { // Day
    MONDAY = 0,
    TUESDAY = 1,
    WEDNESDAY = 2,
    THURSDAY = 3,
    FRIDAY = 4,
    SATURDAY = 5,
    SUNDAY = 6,
    INVALID_DAY
};

struct AttendanceInfo {
    AttendanceInfo();

    std::map<std::string, int> idByName;
    int id_cnt;

    int  dateCnt[MAX_NUM_OF_ID + 1][MAX_NUM_OF_WEEK];
    int  points[MAX_NUM_OF_ID + 1];
    int  grade[MAX_NUM_OF_ID + 1];
    std::string names[MAX_NUM_OF_ID + 1];

    int  wedCnt[MAX_NUM_OF_ID + 1];
    int  weekendCnt[MAX_NUM_OF_ID + 1];

    int  maxIdLimit() const { return MAX_NUM_OF_ID; }
};
int GetBasePoint(Day_e day);

Day_e GetIndexFromDay(const std::string& dayString);
int CalcBonus(int wedCount, int weekendCount);
Grade_e CalcGrade(int totalPoints);
void OutputReport(const AttendanceInfo& info);

struct InterfaceRemoveReporter { // 인터페이스
    virtual ~InterfaceRemoveReporter() {}
    virtual void OnRemoved(const std::string& name) = 0;
};

class AttendanceEngine {
public:
    explicit AttendanceEngine(AttendanceInfo& info);

    void AssignId(const std::string& player);
    int  GetIdFromName(const std::string& player);

    void UpdateSpecialDays(int id, Day_e day);
    void UpdateDayAndPoints(int id, Day_e day);

    void UpdatePointAndGrades();

    void ReportRemoved(InterfaceRemoveReporter& reporter);

private:
    AttendanceInfo& info;
};

bool ReadAttendanceList(std::string filename, AttendanceEngine& engineine, int maxLines = MAX_NUM_OF_ATTEND_LIST);
void OutputReport(const AttendanceInfo& info);
void OutputRemovePlayer(const AttendanceInfo& info);

#endif // _DEBUG