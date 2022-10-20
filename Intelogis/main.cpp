#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
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

ostream& operator<<(ostream& os, const Point& obj) {
    os << "[" << obj.getX() << ";" << obj.getY() << "]";
    return os;
}

ostream& operator<<(ostream& os, const vector<Point>& _v) {
    for (auto&& el : _v) cout << el << endl;
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
        if (lineByPointResult == 0) return false;// ????????????????begin(vector<Point> {point} ?????????????????????????????????????????
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

    //cout << _temp.getX() << "<" << point1_1.getX() << "&&" << point1_2.getX() << "<" << _temp.getX() << "||" << endl;
    //cout << _temp.getX() << "<" << point1_2.getX() << "&&" << point1_1.getX() << "<" << _temp.getX() << "&&" << endl;
    //cout << _temp.getX() << "<" << point2_1.getX() << "&&" << point2_2.getX() << "<" << _temp.getX() << "||" << endl;
    //cout << _temp.getX() << "<" << point2_2.getX() << "&&" << point2_1.getX() << "<" << _temp.getX() << "&&" << endl;
    //cout << _temp.getY() << "<" << point1_1.getY() << "&&" << point1_2.getY() << "<" << _temp.getY() << "||" << endl;
    //cout << _temp.getY() << "<" << point1_2.getY() << "&&" << point1_1.getY() << "<" << _temp.getY() << "&&" << endl;
    //cout << _temp.getY() << "<" << point2_1.getY() << "&&" << point2_2.getY() << "<" << _temp.getY() << "||" << endl;
    //cout << _temp.getY() << "<" << point2_2.getY() << "&&" << point2_1.getY() << "<" << _temp.getY() << endl;

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

    //double AB{ (point1_2.getX() - point1_1.getX()) / (point1_2.getY() - point1_1.getY()) };
    //double CD{ (point2_2.getX() - point2_1.getX()) / (point2_2.getY() - point2_1.getY()) };
    //double Y{ (CD * point2_1.getY()- point2_1.getX() - AB*point1_1.getY() + point1_1.getX()) / (CD-AB)};
    //double X{ (Y - point1_1.getY()) * AB + point1_1.getX() };
    //if (((X < point1_1.getX() && point1_2.getX() < X) || (X < point1_2.getX() && point1_1.getX() < X)) &&
    //    ((Y < point1_1.getY() && point1_2.getY() < Y) || (Y < point1_2.getY() && point1_1.getY() < Y)) &&
    //    ((X < point2_1.getX() && point2_2.getX() < X) || (X < point2_2.getX() && point2_1.getX() < X)) &&
    //    ((Y < point2_1.getY() && point2_2.getY() < Y) || (Y < point2_2.getY() && point2_1.getY() < Y)))
    //    return Point{ X,Y };
    //else {};

int main() {
    vector<Point> _v{
        Point{2,1},
        Point{1,4},
        Point{3,2},
        Point{4,4},
        Point{4,6},
        Point{5,2},
        Point{6,1},
        Point{7,4}
    };
    PolygonBuild(_v);
    _v.erase(find_if(begin(_v), end(_v), [&](auto element) {
        return element.getUsed() == false; }),
        end(_v));
    cout << _v;

    /*vector<Point> _s{
        Point{7,-1},
        Point{3,7},
        Point{9,2},
        Point{8,6},
        Point{5,2},
    };
    PolygonBuild(_v);
    PolygonBuild(_s);

    cout << PolygonSquare(PolygonsCrossing(_v,_s)) << endl;*/
    return 0;
}

