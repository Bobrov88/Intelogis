#include <iostream>  // including libraries
#include <vector>
#include <algorithm>
#include <fstream>
#include <iterator>
#include <string>
#include <list>
#include <conio.h>
#include <numeric>
#include <math.h>
constexpr auto ANGLETOLENGTH = 66.3; // average koeff of converting from angles of latitude to km
using namespace std;

class Point {  // Point class
    double x;
    double y;
    bool isUsedWhileBuildingPolygon;
public:
    explicit Point(double _x, double _y) : x{ _x },
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
    void setUnused() { isUsedWhileBuildingPolygon = false; }
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

    bool operator==(const Point& other) {
        return this->x == other.x &&
            this->y == other.y;
    }
    ~Point() {};
};

string findDepotID(string& s) {     // reading depotID from file
    size_t index{ 0 };
    size_t step{ 0 };
    while (step < 6) {
        index = s.find('\t', index);
        ++index;
        ++step;
    }
    return s.substr(index, s.find('\t', index) - index);
}

double distanceToPrevPoint(string& s) { // reading distance from file
    size_t index{ 0 };
    size_t step{ 0 };
    while (step < 10) {
        index = s.find('\t', index);
        ++index;
        ++step;
    }
    return std::stod(s.substr(index, s.find('\t', index) - index));

}

string isIndNull(string& s) {       // reading data defining whether an object is the Base or not
    size_t index{ 0 };
    size_t step{ 0 };
    while (step < 7) {
        index = s.find('\t', index);
        ++index;
        ++step;
    }
    return s.substr(index, s.find('\t', index) - index);
}

Point readDepotCoord(string& s, size_t _step) { // reading coordinates x and y
    Point _temp(0, 0);
    size_t index{ 0 };
    size_t step{ 0 };
    while (step < _step) {
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

ostream& operator<<(ostream& os, const Point& obj) {        // operator << overloading for printing Point
    os << "[" << obj.getX() << ";" << obj.getY() << "]";
    return os;
}

template <typename T>
ostream& operator<<(ostream& os, const vector<T>& _v) {     // operator << overloading for printing vector of points
    for (auto&& el : _v) std::cout << el << endl;
    return os;
    }

template <typename C>
double lineByTwoPoint(const C& X, const C& X1Y1, const C& X2Y2) { // defining a position of a point in relation to line
    return X->getX() * 
        (X2Y2->getY() - X1Y1->getY()) +
        X->getY() *
        (X1Y1->getX() - X2Y2->getX()) -
        X1Y1->getX() * X2Y2->getY() +
        X2Y2->getX() * X1Y1->getY();
}

template <typename C>
void PolygonBuild(C& _V) {                                  // building konvexes polygon by points
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
double PolygonSquare(const C& _V) {                 // defining square of polygon
    double ps1{ 0 };
    double ps2{ 0 };
    auto n{ _V.size() };
    if (n == 0) return 0;
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
bool PolygonContainsAPoint(const C& _V, const Point& point) {       // defininng whether a point is inside of a polygon
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
bool PolygonsSidesCrossing(const C& point1_1, const C& point1_2, const C& point2_1, const C& point2_2, Point& _temp) {  // defining whether two line crosses or doesn't
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
C PolygonsCrossing(const C& _V1, const C& _V2) {                        // building a polygon by crossing two polygon if it's possible
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
        if (PolygonContainsAPoint(_V1, *begin_v2)) {
            crossVector.push_back(*begin_v2);
            crossVector.back().setUnused();
        }
        ++begin_v2;
    }
    begin_v1 = begin(_V1);
    while (begin_v1 != end(_V1)) {
        if (PolygonContainsAPoint(_V2, *begin_v1)) {
            crossVector.push_back(*begin_v1);
            crossVector.back().setUnused();
        }
        ++begin_v1;
    }
    if (crossVector.size() > 2)
        PolygonBuild(crossVector);
    return crossVector;
}

void findRealPositionForXandY(size_t& lat, string& line) {              // searching position of latitude in files because the latitude has different position in different files
    size_t tabPos{};
    for (char& c : line) {
        if (c == '\t')
            ++tabPos;
        }
        lat = tabPos-1;
}

template <typename T>
double twoPointDistance(const T& A, const T& B) {                       // distance between two points
    return sqrt(pow(A.getX() - B.getX(), 2) +
                pow(A.getY() - B.getY(), 2));
}

template <typename C, typename T>
double ShortestWay(C& _V, T& base) {                                    // defining shortest way
    double distance{ 0 };
    double mindistance{ DBL_MAX };
    double currentdistance{ 0 };
    auto pV{ _V.begin() };
    for (auto&& el : _V) {
        el.setUnused();
        currentdistance = twoPointDistance(el, base);
        if (mindistance > currentdistance) {
            mindistance = currentdistance;
            *pV = el;
        }
    }
    distance += mindistance;
    pV->setUsed();
    swap(*_V.begin(), *pV);
    for (auto it(begin(_V)); it != prev(end(_V)); ++it) {
        mindistance = twoPointDistance(*it, *next(it));
        for (auto it2(next(it)); it2 != end(_V); ++it2) {
            currentdistance = twoPointDistance(*it, *it2);
            if ((mindistance > currentdistance) &&
                (!it2->getUsed())) {
                mindistance = currentdistance;
                pV = it2;
            }
        }
        distance += mindistance;
        pV->setUsed();
        swap(*(next(it)), *pV);
    }
    return distance;
}
 // reading data and building polygons
template <typename It, typename Pos, typename StrFromFile, typename DataPoint, typename DataDouble, typename Latitude>
void ReadingAndBuildingPolygons(It& waypoint, It& depot, Pos& _begin, StrFromFile& lineFromWaypoint, StrFromFile& lineFromDepot, DataPoint& BigData, DataDouble& Distances, Latitude& latitudeitudeLineNumber) {
    while (true) {
        getline(waypoint, lineFromWaypoint);
        if (waypoint.eof()) break;
        if (isIndNull(lineFromWaypoint) == "0")
            continue;
        BigData.push_back(vector<Point> {});
        BigData.back().reserve(100);
        Distances.push_back(0);
        while (!waypoint.eof() && isIndNull(lineFromWaypoint) != "0") {
            Distances.back() += distanceToPrevPoint(lineFromWaypoint);
            lineFromWaypoint = findDepotID(lineFromWaypoint);
            depot.seekg(_begin);
            while (true)
            {
                getline(depot, lineFromDepot);
                if (lineFromDepot.substr(0, lineFromDepot.find('\t', 0)) == lineFromWaypoint) {
                    BigData.back().push_back(readDepotCoord(lineFromDepot, latitudeitudeLineNumber));
                    break;
                }
            }
            getline(waypoint, lineFromWaypoint);
        }
        BigData.back().erase(std::unique(BigData.back().begin(), BigData.back().end()), BigData.back().end());
    }
}

int main() {

    vector<vector<Point>> BigData;      // polygons by points (coverage area of one route)
    vector<double> Distances;           // sum of distances within one route

    ifstream waypoint{ "waypoint.txt" };                    // depot file opening
    if (!waypoint.is_open()) {
        std::cout << "File \"Waypoint.txt\" not found!" << endl;
        return -1;
    }
    ifstream depot{ "depot.txt" };                          // waypoint file opening
    if (!depot.is_open()) {
        std::cout << "File \"Depot.txt\" not found!" << endl;
        return -1;
    }

    string lineFromWaypoint{};                  // string from waypoint.txt
    string lineFromDepot{};                     // strig from depot.txt
    auto _begin{ depot.tellg() };               // fixing beginning of depot.txt file
    size_t latitudeitudeLineNumber{};           // columb number of latitude in depot.txt
    double BigSquare{};                         // common square of all polygons
    double ShortestDistances{ 0 };              // shortest distance
    double RealDistances{ 0 };                  // real distance
    double CrossSquare{};                       // square of polygon ceossing (intersection)

    getline(depot, lineFromDepot);              // reading first line in depot file for defining a colunm number 
    findRealPositionForXandY(latitudeitudeLineNumber,
        lineFromDepot);
    getline(depot, lineFromDepot);              // reading second line in depot file for defining coordinates of base
    Point Base(readDepotCoord(lineFromDepot, latitudeitudeLineNumber));

    getline(waypoint, lineFromWaypoint);        // reading first line in waypoint.txt for ignoring it and passing inputfile iterator to second string

    ReadingAndBuildingPolygons(waypoint, depot, // calling function for reading data and building polygons
        _begin,
        lineFromWaypoint, lineFromDepot,
        BigData, Distances,
        latitudeitudeLineNumber);

    for (auto&& areas : Distances)
        RealDistances += areas;

    for (auto&& areas : BigData) {
        BigSquare += PolygonSquare(areas);
    }

    for (auto it{ begin(BigData) }; it != prev(end(BigData)); ++it)         // calling function for defining square of all polygon intersection
        for (auto it2{ next(it) }; it2 != end(BigData); ++it2) {
            CrossSquare += PolygonSquare(PolygonsCrossing(*it, *it2));
        }
    for (auto&& areas : BigData)
        ShortestDistances += ShortestWay(areas, Base);                      // calling function for defining shortest way

    std::cout << "BigSquare = " << BigSquare << endl;                       // printing to console
    std::cout << "CrossSquare = " << CrossSquare << endl;
    std::cout << "Percent is " << CrossSquare / BigSquare * 100<<endl;
    std::cout << "Shortest distance is " << ShortestDistances * ANGLETOLENGTH << endl;
    std::cout<< "Real distance is " << RealDistances << endl;
    std::cout << "Percent is " << (ShortestDistances*ANGLETOLENGTH) / RealDistances * 100 << endl;

    waypoint.close();                                                       // closing files
    depot.close();
    return 0;
}

