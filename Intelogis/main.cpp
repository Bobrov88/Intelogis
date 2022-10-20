#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <optional>
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
    while (it != end(_V) - 1) {
        (lineByTwoPoint(begin(vector<Point> {point}), it, next(it)) < 0) ? ++minus : ++plus; // ?????????????????????????????????????????????????????????????????????
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
        Point{2,4},
        Point{4,7},
        Point{4.5,0},
        Point{7,4},
        Point{7,1}
    };

    vector<Point> _s{
        Point{7,-1},
        Point{3,7},
        Point{9,2},
        Point{8,6}
    };
    PolygonBuild(_v);
    PolygonBuild(_s);

    /*for (auto&& el : _v) {
        cout << el.getX() << ";" << el.getY() << endl;
    }
    cout << std::boolalpha << PolygonContainsAPoint(_v, Point{ 0,0 }) << endl;
    cout << std::boolalpha << PolygonContainsAPoint(_v, Point{ 3,7 }) << endl;
    cout << std::boolalpha << PolygonContainsAPoint(_v, Point{ 4,3 }) << endl;
    cout << std::boolalpha << PolygonContainsAPoint(_v, Point{ 5,6 }) << endl;*/


    //std::cout << "Square _S is " << PolygonSquare(_s) << endl;

    auto begin_v{ begin(_v) };
    auto end_v{ end(_v) };
    auto begin_s{ begin(_s) };
    auto end_s{ end(_s) };
    vector<Point> cross_square;
    Point _temp(0, 0);
    while (begin_v != end_v) {
        begin_s = begin(_s);
        while (begin_s != end_s) {
            if (PolygonsSidesCrossing
            (*begin_v,
                ((next(begin_v) == end_v) ? *begin(_v) : *next(begin_v)),
                *begin_s,
                ((next(begin_s) == end_s) ? *begin(_s) : *next(begin_s)),
                _temp)) {
                cout << _temp << endl;
                cross_square.push_back(_temp);
            }
            ++begin_s;
        }
        ++begin_v;
    }
    return 0;
}

