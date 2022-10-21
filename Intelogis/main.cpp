#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <iterator>
#include <string>
#include <list>
#include <numeric>
using namespace std;

class Point {
    double x;
    double y;
    bool isUsedWhileBuildingPolygon;
public:
    Point(double _x, double _y) : x{ _x },
        y{ _y },
        isUsedWhileBuildingPolygon{ 0 }
    {}
    Point(const Point& other) : x{ other.x },
        y{ other.y },
        isUsedWhileBuildingPolygon{ other.isUsedWhileBuildingPolygon }
    {}
    void setX(const double _x) { x = _x; }
    void setY(const double _y) { y = _y; }
    void setUsed() { isUsedWhileBuildingPolygon = true; }
    double getX() const { return x; }
    double getY() const { return y; }
    bool getUsed() const { return isUsedWhileBuildingPolygon; }
    Point& operator=(const Point& other) {
        this->x = other.x;
        this->y = other.y;
        this->isUsedWhileBuildingPolygon =
            other.isUsedWhileBuildingPolygon;
        return *this;
    }
    ~Point() {};
};

string findDepotID(string& s) {
    size_t index{ 0 };
    size_t step{ 0 };
    while (step < 6) {
        index = s.find('\t', index);
        ++index;
        ++step;
    }
    return s.substr(index, s.find('\t', index) - index);
}

string isIndNull(string& s) {
    size_t index{ 0 };
    size_t step{ 0 };
    while (step < 7) {
        index = s.find('\t', index);
        ++index;
        ++step;
    }
    return s.substr(index, s.find('\t', index) - index);
}

Point readDepotCoord(string& s) {
    Point _temp(0, 0);
    size_t index{ 0 };
    size_t step{ 0 };
    while (step < 22) {
        index = s.find('\t', index);
        ++index;
        ++step;
    }
    _temp.setX(
        std::stod(
        s.substr(index, s.find('\t', index) - index)));
    index = s.find('\t', index);
    _temp.setY(
        std::stod(
            s.substr(index, s.find('\n', index) - index)));
    return _temp;
}

ostream& operator<<(ostream& os, const Point& obj) {
    os << "[" << obj.getX() << ";" << obj.getY() << "]";
    return os;
}

ostream& operator<<(ostream& os, const vector<Point>& _v) {
    for (auto&& el : _v) std::cout << el << endl;
    return os;
    }

template <typename C>
double lineByTwoPoint(const C& X, const C& X1Y1, const C& X2Y2) {
    return X->getX() * 
        (X2Y2->getY() - X1Y1->getY()) +
        X->getY() *
        (X1Y1->getX() - X2Y2->getX()) -
        X1Y1->getX() * X2Y2->getY() +
        X2Y2->getX() * X1Y1->getY();
}

template <typename C>
void PolygonBuild(C& _V) {
    double onOtherSide{ 0 };
    size_t onLeftSide{ 0 };
    size_t onRightSide{ 0 };
    auto _begin{ begin(_V) };
    _begin->setUsed();
    auto _end{ end(_V) };
    auto outlinepoint{ begin(_V)};
    auto it{ begin(_V)};
    while (_begin != _end-2) {
        it = std::next(_begin);
        while (it != _end) {
            outlinepoint = std::begin(_V);
            onLeftSide = 0;
            onRightSide = 0;
            while (outlinepoint != _end) {
                if (outlinepoint != _begin && outlinepoint != it) {
                    onOtherSide = lineByTwoPoint(outlinepoint, _begin, it);
                    if (onOtherSide > 0) ++onLeftSide;
                    if (onOtherSide < 0) ++onRightSide;
                }
                ++outlinepoint;
            }
            if (((!onLeftSide && onRightSide)
                || (onLeftSide && !onRightSide)) 
                && !it->getUsed()) {
                it->setUsed();
                std::swap(*next(_begin), *it);
                break;
            }
            ++it;
        }
        ++_begin;
    }
    _V.erase(find_if(begin(_V), end(_V), [&](auto element) {
        return element.getUsed() == false; }),
        end(_V));
}

template <typename C>
double PolygonSquare(const C& _V) {
    double ps1{ 0 };
    double ps2{ 0 };
    auto n{ _V.size() };
    for (size_t i{ 0 }; i < n - 1; ++i) {
        ps1 += _V[i].getX() * _V[i + 1].getY();
        ps2 += _V[i + 1].getX() * _V[i].getY();
    }
    return 0.5 * abs(
        ps1 - ps2 +
        _V[n - 1].getX() *
        _V[0].getY() -
        _V[0].getX() * _V[n - 1].getY()
    );
}

template <typename C>
bool PolygonContainsAPoint(const C& _V, const Point& point) {
    auto it{ std::begin(_V) };
    short minus{ 0 };
    short plus{ 0 };
    double lineByPointResult{ 0 };
    while (it != end(_V) - 1) {
        lineByPointResult = lineByTwoPoint(begin(vector<Point> {point}), it, next(it));
        if (lineByPointResult < 0) ++minus;
        if (lineByPointResult > 0) ++plus;
        if (lineByPointResult == 0) return false;
        ++it;
    }
    if ((minus && !plus) || (!minus && plus))
        return true;
    else return false;
}

template <typename C>
bool PolygonsSidesCrossing(const C& point1_1, const C& point1_2, const C& point2_1, const C& point2_2, Point& _temp) {
    double n{ 0 };
    if (point1_2.getY() - point1_1.getY() != 0) {
        double q = (point1_2.getX() - point1_1.getX()) / (point1_1.getY() - point1_2.getY());
        double sn = point2_1.getX() - point2_2.getX() + (point2_1.getY() - point2_2.getY()) * q;
        if (!sn) return false;
        double fn = point2_1.getX() - point1_1.getX() + (point2_1.getY() - point1_1.getY()) * q;
        n = fn / sn;
    }
    else {
        if (!(point2_1.getY() - point2_2.getY()))
            return false;
        n = (point2_1.getY() - point1_1.getY()) / (point2_1.getY() - point2_2.getY());
    }
    _temp.setX(point2_1.getX() + (point2_2.getX() - point2_1.getX()) * n);
    _temp.setY(point2_1.getY() + (point2_2.getY() - point2_1.getY()) * n);

    if (((_temp.getX() <= point1_1.getX() && point1_2.getX() <= _temp.getX()) ||
         (_temp.getX() <= point1_2.getX() && point1_1.getX() <= _temp.getX())) &&
        ((_temp.getX() <= point2_1.getX() && point2_2.getX() <= _temp.getX()) ||
         (_temp.getX() <= point2_2.getX() && point2_1.getX() <= _temp.getX())) &&
        ((_temp.getY() <= point1_1.getY() && point1_2.getY() <= _temp.getY()) ||
         (_temp.getY() <= point1_2.getY() && point1_1.getY() <= _temp.getY())) &&
        ((_temp.getY() <= point2_1.getY() && point2_2.getY() <= _temp.getY()) ||
         (_temp.getY() <= point2_2.getY() && point2_1.getY() <= _temp.getY())))
        return true;
    else return false;
}

template <typename C>
C PolygonsCrossing(const C& _V1, const C& _V2) {
    vector<Point> crossVector;
    auto begin_v1{ begin(_V1) };
    auto begin_v2{ begin(_V2) };
    Point _temp(0, 0);
    while (begin_v1 != end(_V1)) {
        begin_v2 = begin(_V2);
        while (begin_v2 != end(_V2)) {
            if (PolygonsSidesCrossing
            (*begin_v1,
                ((next(begin_v1) == end(_V1)) ? *begin(_V1) : *next(begin_v1)),
                *begin_v2,
                ((next(begin_v2) == end(_V2)) ? *begin(_V2) : *next(begin_v2)),
                _temp)) {
                crossVector.push_back(_temp);
            }
            ++begin_v2;
        }
        ++begin_v1;
    }
    begin_v2 = begin(_V2);
    while (begin_v2 != end(_V2)) {
        if (PolygonContainsAPoint(_V1, *begin_v2))
            crossVector.push_back(*begin_v2);
        ++begin_v2;
    }
    begin_v1 = begin(_V1);
    while (begin_v1 != end(_V1)) {
        if (PolygonContainsAPoint(_V2, *begin_v1))
            crossVector.push_back(*begin_v1);
        ++begin_v1;
    }
    PolygonBuild(crossVector);
    return crossVector;
}

int main() {

    vector<vector<Point>> BigData;

    ifstream waypoint{ "waypoint.txt" };
    if (!waypoint.is_open()) {
        std::cout << "File \"Waypoint.txt\" not found!" << endl;
        return -1;
    }
    ifstream depot{ "depot.txt" };
    if (!depot.is_open()) {
        std::cout << "File \"Depot.txt\" not found!" << endl;
        return -1;
    }

    string lineFromWaypoint{};
    string lineFromDepot{};
    auto _begin{ depot.tellg() };
    int counter{ 0 };

    getline(waypoint, lineFromWaypoint);
    while (true) {
        getline(waypoint, lineFromWaypoint);
        if (waypoint.eof()) break;
        if (isIndNull(lineFromWaypoint) == "0")
            continue;
        BigData.push_back(vector<Point> {});
        while (!waypoint.eof() && isIndNull(lineFromWaypoint) != "0") {
            lineFromWaypoint = findDepotID(lineFromWaypoint);
            depot.seekg(_begin);
            while (true)
            {
                getline(depot, lineFromDepot);
                if (lineFromDepot.substr(0, lineFromDepot.find('\t', 0)) == lineFromWaypoint) {
                    BigData.back().push_back(readDepotCoord(lineFromDepot));
                std::cout << "line "<<lineFromWaypoint<<" "<<++counter << endl;
                    break;
                }
            }
            getline(waypoint, lineFromWaypoint);
        }
    }

    double BigSquare{};

    for (auto&& areas : BigData) {
        BigSquare += PolygonSquare(areas);
    }

    double CrossSquare{};
    auto it  {begin(BigData)};
    for (; it != end(BigData); ++it) 
        for (auto it2{ next(it) }; it2 != end(BigData); ++it2) 
            CrossSquare+=PolygonSquare(PolygonsCrossing(*it, *it2));

    std::cout << "BigSquare = " << BigSquare << endl;
    std::cout << "CrossSquare = " << CrossSquare << endl;
    std::cout << "Percent is " << CrossSquare / BigSquare * 100;
    waypoint.close();
    depot.close();
    return 0;
}

