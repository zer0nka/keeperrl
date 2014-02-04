#include "stdafx.h"

#include "util.h"

using namespace std;

void RandomGen::init(int seed) {
  generator.seed(seed);
}

int RandomGen::getRandom(int max) {
  return getRandom(0, max);
}

int RandomGen::getRandom(int min, int max) {
  CHECK(max > min);
  return uniform_int_distribution<int>(min, max - 1)(generator);
}

void RandomGen::makeShuffle(string id, int min, int max) {
  shuffleMap.erase(id);
  ShuffleInfo info;
  info.minRange = min;
  info.maxRange = max;
  for (int i : Range(min, max))
    info.numbers.push_back(i);
  random_shuffle(info.numbers.begin(), info.numbers.end(), [](int a) { return Random.getRandom(a);});
  shuffleMap.insert({id, std::move(info)});
}

int RandomGen::getRandom(const string& shuffleId, int min, int max) {
  if (!shuffleMap.count(shuffleId))
    makeShuffle(shuffleId, min, max);
  else {
    ShuffleInfo& info = shuffleMap.at(shuffleId);
    if (info.minRange != min || info.maxRange != max || info.numbers.empty())
      makeShuffle(shuffleId, min, max);
  }
  ShuffleInfo& info = shuffleMap.at(shuffleId);
  int ret = info.numbers.back();
  info.numbers.pop_back();
  return ret;
}

int RandomGen::getRandom(const vector<double>& weights, double r) {
  double sum = 0;
  for (double elem : weights)
    sum += elem;
  if (r == -1)
    r = Random.getDouble(0, sum);
  sum = 0;
  for (int i : All(weights)) {
    sum += weights[i];
    if (sum >= r)
      return i;
  }
  return weights.size() - 1;
}

bool RandomGen::roll(int chance) {
  return getRandom(chance) == 0;
}

double RandomGen::getDouble() {
  return defaultDist(generator);
}

double RandomGen::getDouble(double a, double b) {
  return uniform_real_distribution<double>(a, b)(generator);
}

RandomGen Random;

template string convertToString<int>(const int&);
template string convertToString<size_t>(const size_t&);
template string convertToString<char>(const char&);
template string convertToString<double>(const double&);
//template string convertToString<Vec2>(const Vec2&);

template int convertFromString<int>(const string&);
template char convertFromString<char>(const string&);
template double convertFromString<double>(const string&);

template <class T>
string convertToString(const T& t){
  std::stringstream ss;
  ss << t;
  return ss.str();
}

template <>
string convertToString(const Vec2& t){
  std::stringstream ss;
  ss << "(" << t.x << "," << t.y << ")";
  return ss.str();
}

template <class T>
T convertFromString(const string& s){
  std::stringstream ss(s);
  T t;
  ss >> t;
  CHECK(ss) << "Error parsing " << s << " to " << typeid(T).name();
  return t;
}

void trim(string& s) {
  CHECK(s.size() > 0);
  while (isspace(s[0])) 
    s.erase(s.begin());
  while (isspace(*s.rbegin()))
    s.erase(s.size() - 1);
}

string toUpper(const string& s) {
  string ret(s);
  transform(ret.begin(), ret.end(), ret.begin(), ::toupper);
  return ret;
}

string toLower(const string& s) {
  string ret(s);
  transform(ret.begin(), ret.end(), ret.begin(), ::tolower);
  return ret;
}

vector<string> split(const string& s, char delim) {
  int begin = 0;
  vector<string> ret;
  for (int i : Range(s.size() + 1))
    if (i == s.size() || s[i] == delim) {
      string tmp = s.substr(begin, i - begin);
      if (!tmp.empty())
        ret.push_back(tmp);
      begin = i + 1;
    }
  return ret;
}

template<>
bool contains(const string& s, const string& p) {
  return s.find(p) != string::npos;
}


Vec2::Vec2(int _x, int _y) : x(_x), y(_y) {
}

Vec2 Vec2::mult(const Vec2& v) const {
  return Vec2(x * v.x, y * v.y);
}

Vec2 Vec2::div(const Vec2& v) const {
  return Vec2(x / v.x, y / v.y);
}

int Vec2::dotProduct(Vec2 a, Vec2 b) {
  return a.x * b.x + a.y * b.y;
}

vector<Vec2> Vec2::box(int radius, bool shuffle) {
  if (radius == 0)
    return {*this};
  vector<Vec2> v;
  for (int k = -radius; k < radius; ++k)
    v.push_back(*this + Vec2(k, -radius));
  for (int k = -radius; k < radius; ++k)
    v.push_back(*this + Vec2(radius, k));
  for (int k = -radius; k < radius; ++k)
    v.push_back(*this + Vec2(-k, radius));
  for (int k = -radius; k < radius; ++k)
    v.push_back(*this + Vec2(-radius, -k));
  if (shuffle)
    random_shuffle(v.begin(), v.end(), [](int a) { return Random.getRandom(a);});
  return v;
}

vector<Vec2> Vec2::directions8(bool shuffle) {
  return Vec2(0, 0).neighbors8(shuffle);
}

vector<Vec2> Vec2::neighbors8(bool shuffle) const {
  vector<Vec2> res = {Vec2(x, y + 1), Vec2(x + 1, y), Vec2(x, y - 1), Vec2(x - 1, y), Vec2(x + 1, y + 1), Vec2(x + 1, y - 1), Vec2(x - 1, y - 1), Vec2(x - 1, y + 1)};
  if (shuffle)
    random_shuffle(res.begin(), res.end(),[](int a) { return Random.getRandom(a);});
  return res;
}

vector<Vec2> Vec2::directions4(bool shuffle) {
  return Vec2(0, 0).neighbors4(shuffle);
}

vector<Vec2> Vec2::neighbors4(bool shuffle) const {
  vector<Vec2> res = { Vec2(x, y + 1), Vec2(x + 1, y), Vec2(x, y - 1), Vec2(x - 1, y)};
  if (shuffle)
    random_shuffle(res.begin(), res.end(),[](int a) { return Random.getRandom(a);});
  return res;
}

bool Vec2::isCardinal4() const {
  return abs(x) + abs(y) == 1;
}

Dir Vec2::getCardinalDir() const {
  if (x == 0 && y == -1)
    return Dir::N;
  if (x == 1 && y == -1)
    return Dir::NE;
  if (x == 1 && y == 0)
    return Dir::E;
  if (x == 1 && y == 1)
    return Dir::SE;
  if (x == 0 && y == 1)
    return Dir::S;
  if (x == -1 && y == 1)
    return Dir::SW;
  if (x == -1 && y == 0)
    return Dir::W;
  if (x == -1 && y == -1)
    return Dir::NW;
  FAIL << "Not cardinal dir " << *this;
  return Dir::N;
}

vector<Vec2> Vec2::corners() {
  return { Vec2(1, 1), Vec2(1, -1), Vec2(-1, -1), Vec2(-1, 1)};
}

vector<Vec2> Rectangle::getAllSquares() {
  vector<Vec2> ret;
  for (Vec2 v : (*this))
    ret.push_back(v);
  return ret;
}

bool Vec2::inRectangle(int px, int py, int kx, int ky) const {
  return x >= px && x < kx && y >= py && y < ky;
}

bool Vec2::inRectangle(const Rectangle& r) const {
  return x >= r.px && x < r.kx && y >= r.py && y < r.ky;
}

bool Vec2::operator== (const Vec2& v) const {
  return v.x == x && v.y == y;
}

bool Vec2::operator!= (const Vec2& v) const {
  return v.x != x || v.y != y;
}

Vec2& Vec2::operator +=(const Vec2& v) {
  x += v.x;
  y += v.y;
  return *this;
}

Vec2 Vec2::operator + (const Vec2& v) const {
  return Vec2(x + v.x, y + v.y);
}

Vec2 Vec2::operator * (int a) const {
  return Vec2(x * a, y * a);
}

Vec2 Vec2::operator / (int a) const {
  return Vec2(x / a, y / a);
}

Vec2& Vec2::operator -=(const Vec2& v) {
  x -= v.x;
  y -= v.y;
  return *this;
}

Vec2 Vec2::operator - (const Vec2& v) const {
  return Vec2(x - v.x, y - v.y);
}

Vec2 Vec2::operator - () const {
  return Vec2(-x, -y);
}

bool Vec2::operator < (Vec2 v) const {
  return x < v.x || (x == v.x && y < v.y);
}

int Vec2::length8() const {
  return max(abs(x), abs(y));
}

int Vec2::dist8(Vec2 v) const {
  return (v - *this).length8();
}

int Vec2::length4() const {
  return abs(x) + abs(y);
}

double Vec2::lengthD() const {
  return sqrt(x * x + y * y);
}

Vec2 Vec2::shorten() const {
  CHECK(x == 0 || y == 0 || abs(x) == abs(y));
  int d = length8();
  return Vec2(x / d, y / d);
}

static int sgn(int a) {
  if (a == 0)
    return 0;
  if (a < 0)
    return -1;
  else
    return 1;
}

static int sgn(int a, int b) {
  if (abs(a) >= abs(b))
    return sgn(a);
  else
    return 0;
}

pair<Vec2, Vec2> Vec2::approxL1() const {
  return make_pair(Vec2(sgn(x, x), sgn(y,y)), Vec2(sgn(x, y), sgn(y, x)));
}

string Vec2::getBearing() const {
  double ang = atan2(y, x) / 3.14159265359 * 180 / 45;
  if (ang < 0)
    ang += 8;
  if (ang < 0.5 || ang >= 7.5)
    return "east";
  if (ang >= 0.5 && ang < 1.5)
    return "south-east";
  if (ang >= 1.5 && ang < 2.5)
    return "south";
  if (ang >= 2.5 && ang < 3.5)
    return "south-west";
  if (ang >= 3.5 && ang < 4.5)
    return "west";
  if (ang >= 4.5 && ang < 5.5)
    return "north-west";
  if (ang >= 5.5 && ang < 6.5)
    return "north";
  if (ang >= 6.5 && ang < 7.5)
    return "north-east";
  FAIL << ang;
  return "";
}

Rectangle::Rectangle(int _w, int _h) : px(0), py(0), kx(_w), ky(_h), w(_w), h(_h) {
  CHECK(w > 0 && h > 0);
}

Rectangle::Rectangle(Vec2 d) : px(0), py(0), kx(d.x), ky(d.y), w(d.x), h(d.y) {
  CHECK(d.x > 0 && d.y > 0);
}

Rectangle::Rectangle(int px1, int py1, int kx1, int ky1) : px(px1), py(py1), kx(kx1), ky(ky1), w(kx1 - px1),
    h(ky1 - py1) {
  CHECK(kx > px && ky > py);
}

Rectangle::Rectangle(Vec2 p, Vec2 k) : px(p.x), py(p.y), kx(k.x), ky(k.y), w(k.x - p.x), h(k.y - p.y) {
  CHECK(k.x > p.x);
  CHECK(k.y > p.y);
}

Rectangle::Iter::Iter(int x1, int y1, int px1, int py1, int kx1, int ky1) : pos(x1, y1), px(px1), py(py1), kx(kx1), ky(ky1) {}

Vec2 Rectangle::randomVec2() const {
  return Vec2(Random.getRandom(px, kx), Random.getRandom(py, ky));
}

Vec2 Rectangle::middle() const {
  return Vec2((px + kx) / 2, (py + ky) / 2);
}

int Rectangle::getPX() const {
  return px;
}

int Rectangle::getPY() const {
  return py;
}

int Rectangle::getKX() const {
  return kx;
}

int Rectangle::getKY() const {
  return ky;
}

int Rectangle::getW() const {
  return w;
}

int Rectangle::getH() const {
  return h;
}

Vec2 Rectangle::getTopLeft() const {
  return Vec2(px, py);
}

Vec2 Rectangle::getBottomRight() const {
  return Vec2(kx, ky);
}

Vec2 Rectangle::getTopRight() const {
  return Vec2(kx, py);
}

Vec2 Rectangle::getBottomLeft() const {
  return Vec2(px, ky);
}

bool Rectangle::intersects(const Rectangle& other) const {
  return max(px, other.px) < min(kx, other.kx) && max(py, other.py) < min(ky, other.ky);
}

Rectangle Rectangle::intersection(const Rectangle& other) const {
  return Rectangle(max(px, other.px), max(py, other.py), min(kx, other.kx), min(ky, other.ky));
}

Rectangle Rectangle::minusMargin(int margin) const {
  CHECK(px + margin < kx - margin && py + margin < ky - margin) << "Margin too big";
  return Rectangle(px + margin, py + margin, kx - margin, ky - margin);
}

Vec2 Rectangle::Iter::operator* () const {
  return pos;
}
bool Rectangle::Iter::operator != (const Iter& other) const {
  return pos != other.pos;
}

const Rectangle::Iter& Rectangle::Iter::operator++ () {
  ++pos.y;
  if (pos.y >= ky) {
    pos.y = py;
    ++pos.x;
  } else 
    CHECK(pos.x < kx) << "Iterator out of range: " << **this << ", " << Vec2(px, py) << " " << Vec2(kx, ky);
  return *this;
}

Rectangle::Iter Rectangle::begin() const {
  return Iter(px, py, px, py, kx, ky);
}

Rectangle::Iter Rectangle::end() const {
  return Iter(kx, py, px, py, kx, ky);
}

Range::Range(int a, int b) : start(a), finish(b) {
  CHECK(a <= b);
}
Range::Range(int a) : Range(0, a) {}

Range::Iter Range::begin() {
  return Iter(start, start, finish);
}

Range::Iter Range::end() {
  return Iter(finish, start, finish);
}

Range::Iter::Iter(int i, int a, int b) : ind(i), min(a), max(b) {}

int Range::Iter::operator* () const {
  return ind;
}

bool Range::Iter::operator != (const Iter& other) const {
  return other.ind != ind;
}

const Range::Iter& Range::Iter::operator++ () {
  CHECK(++ind <= max);
  return *this;
}

string combine(const vector<string>& adj) {
  string res;
  for (int i : All(adj)) {
    if (i == adj.size() - 1 && i > 0)
      res.append(" and ");
    else if (i > 0)
      res.append(", ");
    res.append(adj[i]);
  }
  return res;
}

