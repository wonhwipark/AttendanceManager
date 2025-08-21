#include <iostream>
#include <fstream>
#include <string>
#include <map>

using namespace std;

// max
static const int MAX_NUM_OF_ATTEND_LIST = 500;
static const int MAX_NUM_OF_ID          = 100;
static const int MAX_NUM_OF_WEEK        = 100;

// point
static const int INVALID_POINT   = 0; 
static const int BASE_POINT_WEEKDAY   = 1;
static const int BASE_POINT_WEEKEND   = 2;
static const int BASE_POINT_WEDNESDAY = 3;

// Bonus
static const int WED_BONUS_THRESHOLD = 10;
static const int WEEKEND_BONUS_THRESHOLD = 10;
static const int BONUS_POINTS = 10;

// Threshold of Grade
static const int COUNT_THRESHOLD_WEDNESDAY = 10;
static const int COUNT_THRESHOLD_WEEKEND = 10;
static const int POINT_THRESHOLD_GOLD = 50;
static const int POINT_THRESHOLD_SILVER = 30;

enum Grade_e { // Grade
	NORMAL = 0,
	GOLD   = 1,
	SILVER = 2
};

enum Day_e { // Day
	MONDAY    = 0,
	TUESDAY   = 1,
	WEDNESDAY = 2,
	THURSDAY  = 3,
	FRIDAY    = 4,
	SATURDAY  = 5,
	SUNDAY    = 6,
	INVALID_DAY
};

struct AttendanceInfo {
	AttendanceInfo() : id_cnt(0) {
		for (int i = 0; i < MAX_NUM_OF_ID; i++) {
			points[i] = 0;
			grade[i] = NORMAL;
			wedCnt[i] = 0;
			weekendCnt[i] = 0;
			names[i].clear();
			for (int j = 0; j < MAX_NUM_OF_WEEK; j++) {
				dateCnt[i][j] = 0;
			}
		}
	}
	map<string, int> idByName;
	int id_cnt;

	int dateCnt[MAX_NUM_OF_ID + 1][MAX_NUM_OF_WEEK];
	int points[MAX_NUM_OF_ID + 1];
	int grade[MAX_NUM_OF_ID + 1];
	string names[MAX_NUM_OF_ID + 1];

	int wedCnt[MAX_NUM_OF_ID + 1];
	int weekendCnt[MAX_NUM_OF_ID + 1];
};

bool ReadAttendanceList(AttendanceInfo& info);
void UpdatePointAndGrades(AttendanceInfo& info);
void OutputReport(AttendanceInfo& info);
void OutputRemovePlayer(AttendanceInfo& info);

void AssignId(AttendanceInfo& info, std::string& player);
int GetIdFromName(AttendanceInfo& info, const std::string& player);
Day_e GetIndexFromDay(const string& dayString);
int GetBasePoint(const Day_e day);
void UpdateSpecialDays(AttendanceInfo& info, const int nameIdx, const string& dayString);
void UpdateDayAndPoints(AttendanceInfo& info, const int nameIdx, const int dayIdx, const int add_point);

void RunAttendanceSystem() {
	AttendanceInfo info;
	bool isOk = ReadAttendanceList(info);
	if (!isOk) return;

	UpdatePointAndGrades(info);
	OutputReport(info);
	OutputRemovePlayer(info);
}

bool ReadAttendanceList(AttendanceInfo& info)
{
	ifstream fin{ "attendance_weekday_500.txt" }; //500개 데이터 입력
	if (!fin.is_open()) {
		std::cout << "Error opening file!" << endl;
		return false;
	}

	for (int i = 0; i < MAX_NUM_OF_ATTEND_LIST; i++) {
		string player, dayString;
		fin >> player >> dayString;

		AssignId(info, player);
		int nameIdx = GetIdFromName(info, player);
		Day_e dayIdx = GetIndexFromDay(dayString);
		if (dayIdx == INVALID_DAY) return false;

		UpdateSpecialDays(info, nameIdx, dayString);
		UpdateDayAndPoints(info, nameIdx, dayIdx, GetBasePoint(dayIdx));
	}
	return true;
}

void UpdatePointAndGrades(AttendanceInfo& info)
{
	for (int i = 1; i <= info.id_cnt; i++) {
		if (info.dateCnt[i][WEDNESDAY] >= COUNT_THRESHOLD_WEDNESDAY) {
			info.points[i] += BONUS_POINTS;
		}
		if (info.dateCnt[i][SATURDAY] + info.dateCnt[i][SUNDAY] >= COUNT_THRESHOLD_WEEKEND) {
			info.points[i] += BONUS_POINTS;
		}
		if (info.points[i] >= POINT_THRESHOLD_GOLD) {
			info.grade[i] = GOLD;
		}
		else if (info.points[i] >= POINT_THRESHOLD_SILVER) {
			info.grade[i] = SILVER;
		}
		else {
			info.grade[i] = NORMAL;
		}
	}
}

void OutputReport(AttendanceInfo& info)
{
	if (info.id_cnt >= MAX_NUM_OF_ID) return;
	for (int i = 1; i <= info.id_cnt; i++) {
		std::cout << "NAME : " << info.names[i] << ", ";
		std::cout << "POINT : " << info.points[i] << ", ";
		std::cout << "GRADE : ";

		if (info.grade[i] == GOLD) {
			std::cout << "GOLD" << "\n";
		}
		else if (info.grade[i] == SILVER) {
			std::cout << "SILVER" << "\n";
		}
		else {
			std::cout << "NORMAL" << "\n";
		}
	}
}

void OutputRemovePlayer(AttendanceInfo& info)
{
	std::cout << "\n";
	std::cout << "Removed player\n";
	std::cout << "==============\n";
	for (int i = 1; i <= info.id_cnt; i++) {

		if (info.grade[i] != 1 && info.grade[i] != 2 && info.wedCnt[i] == 0 && info.weekendCnt[i] == 0) {
			std::cout << info.names[i] << "\n";
		}
	}
}

void AssignId(AttendanceInfo& info, std::string& player)
{
	if (info.idByName.count(player) == 0) {
		info.idByName.insert({ player, ++info.id_cnt });
		info.names[info.id_cnt] = player;
	}
}

int GetIdFromName(AttendanceInfo& info, const std::string& player)
{
	return info.idByName[player];
}

int GetBasePoint(const Day_e day)
{
	if (day == MONDAY)    return BASE_POINT_WEEKDAY;
	if (day == TUESDAY)   return BASE_POINT_WEEKDAY;
	if (day == WEDNESDAY) return BASE_POINT_WEDNESDAY;
	if (day == THURSDAY)  return BASE_POINT_WEEKDAY;
	if (day == FRIDAY)    return BASE_POINT_WEEKDAY;
	if (day == SATURDAY)  return BASE_POINT_WEEKEND;
	if (day == SUNDAY)    return BASE_POINT_WEEKEND;
	return INVALID_POINT;
}

Day_e GetIndexFromDay(const string& dayString) 
{
	if (dayString == "monday")    return MONDAY;
	if (dayString == "tuesday")   return TUESDAY;
	if (dayString == "wednesday") return WEDNESDAY;
	if (dayString == "thursday")  return THURSDAY;
	if (dayString == "friday")    return FRIDAY;
	if (dayString == "saturday")  return SATURDAY;
	if (dayString == "sunday")    return SUNDAY;
	return INVALID_DAY;
}

void UpdateSpecialDays(AttendanceInfo& info, const int nameIdx, const string& dayString)
{
	if (dayString == "wednesday") {
		info.wedCnt[nameIdx]++;
	}
	if (dayString == "saturday") {
		info.weekendCnt[nameIdx]++;
	}
	if (dayString == "sunday") {
		info.weekendCnt[nameIdx]++;
	}
}

void UpdateDayAndPoints(AttendanceInfo& info, const int nameIdx, const int dayIdx, const int add_point)
{
	info.dateCnt[nameIdx][dayIdx] += 1;
	info.points[nameIdx] += add_point;
}

int main() {
	RunAttendanceSystem();
	return 0;
}