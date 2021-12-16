#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>
#include <ctime>
#include <random>

//#include <opencv2/core/core.hpp>
//#include <opencv2/imgproc.hpp>
//#include <opencv2/imgcodecs.hpp>
//#include <opencv2/highgui.hpp>

#include "AVL.hpp"
#include "TwoThree.hpp"
using namespace std;

struct Point {
  int x, y;
};

struct Segment {
  Point left, right;
};

struct Event {
  int x, y;
  bool isLeft;
  int index;
  Event() {
    x = 0;
    y = 0;
    isLeft = false;
    index = 0;
  }
  Event(int x, int y, bool l, int i) : x(x), y(y), isLeft(l), index(i) {}

  bool operator<(const Event &e) const {
    if (y == e.y) return x < e.x;
    return y < e.y;
  }

  bool operator>(const Event &e) const {
    if (y == e.y) return x > e.x;
    return y > e.y;
  }

  bool operator==(const Event &e) const {
    if (x != e.x) return false;
    if (y != e.y) return false;
    if (isLeft != e.isLeft) return false;
    if (index != e.index) return false;
    return true;
  }

  bool operator!=(const Event &e) const {
    if (x == e.x) return false;
    if (y == e.y) return false;
    if (isLeft == e.isLeft) return false;
    if (index == e.index) return false;
    return true;
  }

  Event &operator=(const Event &right) {
    if (this == &right) {
      return *this;
    }
    x = right.x;
    y = right.y;
    isLeft = right.isLeft;
    index = right.index;
    return *this;
  }
};

bool onSegment(Point p, Point q, Point r) {
  if (q.x <= max(p.x, r.x) && q.x >= min(p.x, r.x) && q.y <= max(p.y, r.y) &&
      q.y >= min(p.y, r.y))
    return true;

  return false;
}

int orientation(Point p, Point q, Point r) {
  int val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);

  if (val == 0) return 0;

  return (val > 0) ? 1 : 2;
}

bool doIntersect(Segment s1, Segment s2) {
  Point p1 = s1.left, q1 = s1.right, p2 = s2.left, q2 = s2.right;

  int o1 = orientation(p1, q1, p2);
  int o2 = orientation(p1, q1, q2);
  int o3 = orientation(p2, q2, p1);
  int o4 = orientation(p2, q2, q1);

  if (o1 != o2 && o3 != o4) return true;

  if (o1 == 0 && onSegment(p1, p2, q1)) return true;
  if (o2 == 0 && onSegment(p1, q2, q1)) return true;
  if (o3 == 0 && onSegment(p2, p1, q2)) return true;
  if (o4 == 0 && onSegment(p2, q1, q2)) return true;

  return false;
}

set<Event>::iterator pred(set<Event> &s, set<Event>::iterator it) {
  return it == s.begin() ? s.end() : --it;
}

set<Event>::iterator succ(set<Event> &s, set<Event>::iterator it) {
  return ++it;
}

int hasIntersectionsNaive(Segment arr[], int n) {
  vector<string> s;

  int ans = 0;

  for (int i = 0; i < n - 1; i++) {
    for (int j = i + 1; j < n; j++) {
        if (doIntersect(arr[i], arr[j])) {
          s.push_back(to_string(i + 1) + " " + to_string(j + 1));
            ans++;
        }
    }
  }

  for (auto &pr : s) {
    cout << pr << "\n";
  }
  return ans;
}

int hasIntersectionsAVL(Segment arr[], int n) {
  unordered_map<string, int> mp;
  vector<Event> e;
  for (int i = 0; i < n; ++i) {
    e.push_back(Event(arr[i].left.x, arr[i].left.y, true, i));
    e.push_back(Event(arr[i].right.x, arr[i].right.y, false, i));
  }

  sort(e.begin(), e.end(), [](Event &e1, Event &e2) { return e1.x < e2.x; });

  AVL::node<Event> *s = nullptr;
  int ans = 0;

  for (int i = 0; i < 2 * n; i++) {
    Event curr = e[i];
    int index = curr.index;

    if (curr.isLeft) {
      AVL::node<Event> *next = nullptr, *prev = nullptr;
      AVL::findPreSuc(s, prev, next, curr);

      bool flag = false;
      if (next != nullptr && doIntersect(arr[next->key.index], arr[index])) {
        string s = to_string(next->key.index + 1) + " " + to_string(index + 1);
        if (mp.count(s) == 0) {
          mp[s]++;
          ans++;
        }
      }
      if (prev != nullptr && doIntersect(arr[prev->key.index], arr[index])) {
        string s = to_string(prev->key.index + 1) + " " + to_string(index + 1);
        if (mp.count(s) == 0) {
          mp[s]++;
          ans++;
        }
      }
      if (prev != nullptr && next != nullptr && next->key.index == prev->key.index)
        ans--;

      s = AVL::insert(s, curr);
    } else {
      auto it = AVL::search(
          s, Event(arr[index].left.x, arr[index].left.y, true, index));

      AVL::node<Event> *next = nullptr, *prev = nullptr;
      AVL::findPreSuc(s, prev, next, it->key);

      if (next != nullptr && prev != nullptr) {
        string s =
            to_string(next->key.index + 1) + " " + to_string(prev->key.index + 1);
        string s1 =
            to_string(prev->key.index + 1) + " " + to_string(next->key.index + 1);
        if (mp.count(s) == 0 && mp.count(s1) == 0 &&
            doIntersect(arr[prev->key.index], arr[next->key.index]))
          ans++;
        mp[s]++;
      }

      s = AVL::remove(s, it->key);
    }
  }

  for (auto &pr : mp) {
    cout << pr.first << "\n";
  }
  return ans;
}

int hasIntersectionsTwoThree(Segment* arr, int n) {
  unordered_map<string, int>
      mp;
  vector<Event> e;
  for (int i = 0; i < n; ++i) {
    e.push_back(Event(arr[i].left.x, arr[i].left.y, true, i));
    e.push_back(Event(arr[i].right.x, arr[i].right.y, false, i));
  }

  sort(e.begin(), e.end(), [](Event &e1, Event &e2) { return e1.x < e2.x; });

  set<Event> s;
  int ans = 0;

  for (int i = 0; i < 2 * n; i++) {
    Event curr = e[i];
    int index = curr.index;

    if (curr.isLeft) {
      auto next = s.lower_bound(curr);
      auto prev = pred(s, next);
      bool flag = false;
      if (next != s.end() && doIntersect(arr[next->index], arr[index])) {
        string s = to_string(next->index + 1) + " " + to_string(index + 1);
        if (mp.count(s) == 0) {
          mp[s]++;
          ans++;
        }
      }
      if (prev != s.end() && doIntersect(arr[prev->index], arr[index])) {
        string s = to_string(prev->index + 1) + " " + to_string(index + 1);
        if (mp.count(s) == 0) {
          mp[s]++;
          ans++;
        }
      }
      if (prev != s.end() && next != s.end() && next->index == prev->index)
        ans--;

      s.insert(curr);
    }

    else {
      auto it =
          s.find(Event(arr[index].left.x, arr[index].left.y, true, index));
      auto next = succ(s, it);
      auto prev = pred(s, it);

      if (next != s.end() && prev != s.end()) {
        string s =
            to_string(next->index + 1) + " " + to_string(prev->index + 1);
        string s1 =
            to_string(prev->index + 1) + " " + to_string(next->index + 1);
        if (mp.count(s) == 0 && mp.count(s1) == 0 &&
            doIntersect(arr[prev->index], arr[next->index]))
          ans++;
        mp[s]++;
      }

      s.erase(it);
    }
  }

  for (auto &pr : mp) {
    cout << pr.first << "\n";
  }
  return ans;
}

int clip(int n, int lower, int upper) {
  return std::max(lower, std::min(n, upper));
}

Segment RandomSegment(int x, int y) {
  std::uniform_real_distribution<double> unif(0, 1);
  std::uniform_real_distribution<double> unif_neg(-1, 1);
  std::random_device rand_dev;
  std::mt19937 rand_engine(rand_dev());

  int new_x = clip(unif(rand_engine) * x, x * 0.05, x * 0.9);
  int new_y = clip(unif(rand_engine) * y, y * 0.05, y * 0.9);

  Segment segment;
  segment.left = Point{new_x, new_y};

  // printf("unif_neg: %f\n", unif_neg(rand_engine));
  new_x = clip(new_x + unif_neg(rand_engine) * x * 0.5, x * 0.1, x * 0.95);
  new_y = clip(new_y + unif_neg(rand_engine) * y * 0.5, y * 0.1, y * 0.95);

  segment.right = Point{new_x, new_y};

  if (segment.left.x > segment.right.x) {
    swap(segment.left.x, segment.right.x);
  }

  return segment;
};

int main() {
  int n = 5;
  int width = 1000, height = 500;

  //cv::RNG rng(12345);

  bool loop = false;
  //bool show = false;

  while (true) {
    Segment *arr = new Segment[n];
    for (int i = 0; i < n; i++) {
      arr[i] = RandomSegment(width, height);
      cout << arr[i].left.x << " " << arr[i].left.y << " " << arr[i].right.x
           << " " << arr[i].right.y << endl;
    }

    //Segment arr[] = {{{10, 50}, {40, 50}},
    //                 {{20, 50}, {100, 10}},
    //                 {{30, 20}, {100, 30}},
    //                 {{60, 40}, {90, 40}},
    //                 {{70, 10}, {80, 10}}};

    cout << "Naive: " << endl;
    cout << hasIntersectionsNaive(arr, n) << endl << endl;
    cout << "AVL: " << endl;
    cout << hasIntersectionsAVL(arr, n) << endl << endl;
    cout << "TwoThree: " << endl;
    cout << hasIntersectionsTwoThree(arr, n) << endl << endl;

    //cv::Mat image(height, width, CV_8UC3, cv::Scalar(0, 0, 0));

    //if (!image.data) {
    //  cout << "Could not open or find"
    //       << " the image";

    //  return 0;
    //}

    //for (int i = 0; i < n; i++) {
    //  cv::Point p1(arr[i].left.x, arr[i].left.y),
    //      p2(arr[i].right.x, arr[i].right.y);
    //  int thickness = 2;
    //  cv::Scalar color(rng.uniform(0, 255), rng.uniform(0, 255),
    //                   rng.uniform(0, 255));

    //  cv::line(image, p1, p2, color, thickness, cv::LINE_AA);
    //}

    //if (show) {
    //  cv::imshow("Output", image);
    //  cv::waitKey(0);
    //}
    if (!loop) break;
  }

  return 0;
}