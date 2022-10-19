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
Point PolygonsSidesCrossing(const C& point1_1, const C& point1_2, const C& point2_1, const C& point2_2) {



}

int main() {
    vector<Point> _v{
        Point{4,7},
        Point{6,6},
        Point{7,3},
        Point{2,2},
        Point{3,0},
        Point{6,1},
        Point{2,5},
        Point{5,0}
    };

    vector<Point> _s{
        Point{1,1},
        Point{3,3},
        Point{3,1},
        Point{1,3}
    };
    PolygonBuild(_v);
    for (auto&& el : _v) {
        cout << el.getX() << ";" << el.getY() << endl;
    }
    cout << std::boolalpha << PolygonContainsAPoint(_v, Point{ 0,0 }) << endl;
    cout << std::boolalpha << PolygonContainsAPoint(_v, Point{ 3,7 }) << endl;
    cout << std::boolalpha << PolygonContainsAPoint(_v, Point{ 4,3 }) << endl;
    cout << std::boolalpha << PolygonContainsAPoint(_v, Point{ 5,6 }) << endl;


    PolygonBuild(_s);
    for (auto&& el : _s) {
        cout << el.getX() << ";" << el.getY() << endl;
    }
    std::cout << "Square _S is " << PolygonSquare(_s) << endl;
    return 0;
}

