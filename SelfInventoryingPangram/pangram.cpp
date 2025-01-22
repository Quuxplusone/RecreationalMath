
#include <array>
#include <cassert>
#include <chrono>
#include <cstdio>
#include <string>

#if USE_TSL
// https://github.com/Tessil/sparse-map
#include <tsl/sparse_set.h>
template<class T, class H> using UnorderedSet = tsl::sparse_set<T, H>;
#else
#include <unordered_set>
template<class T, class H> using UnorderedSet = std::unordered_set<T, H>;
#endif

static constexpr std::string_view numbers[] = {
  "no",
  "one",
  "two",
  "three",
  "four",
  "five",
  "six",
  "seven",
  "eight",
  "nine",
  "ten",
  "eleven",
  "twelve",
  "thirteen",
  "fourteen",
  "fifteen",
  "sixteen",
  "seventeen",
  "eighteen",
  "nineteen",
  "twenty",
  "twentyone",
  "twentytwo",
  "twentythree",
  "twentyfour",
  "twentyfive",
  "twentysix",
  "twentyseven",
  "twentyeight",
  "twentynine",
  "thirty",
  "thirtyone",
  "thirtytwo",
  "thirtythree",
  "thirtyfour",
  "thirtyfive",
  "thirtysix",
  "thirtyseven",
  "thirtyeight",
  "thirtynine",
  "forty",
  "fortyone",
  "fortytwo",
  "fortythree",
  "fortyfour",
  "fortyfive",
  "fortysix",
  "fortyseven",
  "fortyeight",
  "fortynine",
  "fifty",
  "fiftyone",
  "fiftytwo",
  "fiftythree",
  "fiftyfour",
  "fiftyfive",
  "fiftysix",
  "fiftyseven",
  "fiftyeight",
  "fiftynine",
  "sixty",
  "sixtyone",
  "sixtytwo",
  "sixtythree",
  "sixtyfour",
  "sixtyfive",
  "sixtysix",
  "sixtyseven",
  "sixtyeight",
  "sixtynine",
  "seventy",
  "seventyone",
  "seventytwo",
  "seventythree",
  "seventyfour",
  "seventyfive",
  "seventysix",
  "seventyseven",
  "seventyeight",
  "seventynine",
  "eighty",
  "eightyone",
  "eightytwo",
  "eightythree",
  "eightyfour",
  "eightyfive",
  "eightysix",
  "eightyseven",
  "eightyeight",
  "eightynine",
  "ninety",
  "ninetyone",
  "ninetytwo",
  "ninetythree",
  "ninetyfour",
  "ninetyfive",
  "ninetysix",
  "ninetyseven",
  "ninetyeight",
  "ninetynine",
};

using Inventory = std::array<uint8_t, 26>;
struct InventoryHash {
  size_t operator()(const Inventory& inventory) const {
    size_t h = 0;
    for (size_t x : inventory) {
      h = (33u * h) + x;
    }
    return h;
  }
};

Inventory to_inventory(std::string_view sentence) {
  Inventory ret = {};
  for (char ch : sentence) {
    if ('a' <= ch && ch <= 'z') {
      ret[ch - 'a'] += 1;
    }
  }
  return ret;
}

static auto d_from_inventory(const Inventory& inventory) {
  static constexpr int K = 100;
  std::array<std::string, K> d = {};
  for (int i=0; i < 26; ++i) {
    int count = inventory[i];
    assert(count < K);
    d[count] += char('a' + i);
  }
  return d;
}

std::string to_sentence1(const Inventory& inventory) {
  std::string ret;
  for (int i=0; i < 26; ++i) {
    ret += numbers[inventory[i]];
    ret += char('a' + i);
  }
  return ret;
}

std::string to_sentence2(const Inventory& inventory) {
  auto d = d_from_inventory(inventory);
  std::string ret = "";
  for (size_t count=0; count < d.size(); ++count) {
    const auto& chars = d[count];
    if (!chars.empty()) {
      ret += numbers[count];
      ret += chars;
    }
  }
  return ret;
}

std::string to_sentence3(const Inventory& inventory) {
  auto d = d_from_inventory(inventory);
  std::string ret = "";
  for (size_t count=0; count < d.size(); ++count) {
    const auto& chars = d[count];
    if (!chars.empty()) {
      ret += numbers[count];
      if (chars.size() >= 2) {
        ret += "of";
      }
      ret += chars;
    }
  }
  ret += "and";
  return ret;
}

std::string to_sentence4(const Inventory& inventory) {
  auto d = d_from_inventory(inventory);
  std::string ret = "thissentencehasand";
  for (size_t count=0; count < d.size(); ++count) {
    const auto& chars = d[count];
    if (!chars.empty()) {
      ret += numbers[count];
      if (chars.size() >= 2) {
        ret += "ofand";
      }
      ret += chars;
      if (chars.size() == 1 && count >= 2) {
        ret += 's';
      }
    }
  }
  return ret;
}

std::string to_sentence5(const Inventory& inventory) {
  auto d = d_from_inventory(inventory);
  std::string ret = "icontain";
  int everything_else_count = 0;
  size_t bestsize = 0;
  for (size_t count=0; count < d.size(); ++count) {
    const auto& chars = d[count];
    if (chars.find_first_of("bdjkmpqz") != std::string::npos) {
      // These letters (plus A and C) never appear in a number, so we can't put any
      // of them into the "everything else" bucket because then they won't be mentioned
      // in the pangram at all.
    } else if (chars.size() > bestsize) {
      everything_else_count = count;
      bestsize = chars.size();
    }
  }
  for (size_t count=0; count < d.size(); ++count) {
    if (count == everything_else_count) {
      continue;
    }
    const auto& chars = d[count];
    if (!chars.empty()) {
      ret += numbers[count];
      if (chars.size() >= 2) {
        ret += "ofand";
      }
      ret += chars;
      if (chars.size() == 1 && count >= 2) {
        ret += 's';
      }
    }
  }
  ret += "and";
  ret += numbers[everything_else_count];
  ret += "ofeverythingelse";
  return ret;
}

void advance_odometer(Inventory& inventory) {
  for (int wheel = 26; wheel != 0; --wheel) {
    int neighbor = (wheel != 1) ? inventory[wheel-2] : 29;
    inventory[wheel-1] += 1;
    if (inventory[wheel-1] <= neighbor) return;
    inventory[wheel-1] = 1;
  }
  puts("Odometer ran out! No more solutions.");
  exit(0);
}

int main() {

  {
    // Gilles Esposito-Farese, 1998; Lee Sallows, 2019(?)
    std::string sentence = "one a one b one c one d twenty-eight e seven f five g five h eight i one j one k \
      one l one m eighteen n eighteen o one p one q four r two s ten t four u five v four w one x two y one z";
    Inventory inventory = to_inventory(sentence);
    assert(inventory == to_inventory(to_sentence1(inventory)));
  }
  {
    // Arthur O'Dwyer, 2025
    std::string sentence = "one a,b,c,d,g,h,i,j,k,l,m,p,q,s,v,x,y,z; two e,f,n,r,t,u,w; four o";
    Inventory inventory = to_inventory(sentence);
    assert(inventory == to_inventory(to_sentence2(inventory)));
  }
  {
    // Arthur O'Dwyer, 2025
    std::string sentence = "one of b,c,j,k,l,m,p,q,y,z; two of a,d,g,u,v,w,x; three of h,i,r,s; four of n,t; six f; seven e; and eight o";
    Inventory inventory = to_inventory(sentence);
    assert(inventory == to_inventory(to_sentence3(inventory)));
  }
  {
    // Arthur O'Dwyer, 2025
    std::string sentence = "this sentence has one of b, j, k, l, m, p, q, y, and z; two of c, g, u, and w; three of r, v, and x;"
      "seven of a, f, and i; five h's; six d's; eight o's; ten t's; fourteen s's; sixteen n's; and nineteen e's";
    Inventory inventory = to_inventory(sentence);
    assert(inventory == to_inventory(to_sentence4(inventory)));
  }
  {
    // Arthur O'Dwyer, 2025
    std::string sentence = "this sentence has one of b, j, k, m, p, q, y, and z; two of c, l, u, and x; three of g, r, v, and w;"
      "six of d, f, h, and i; seven a's; eight o's; nine t's; twelve s's; fourteen n's; and eighteen e's";
    Inventory inventory = to_inventory(sentence);
    assert(inventory == to_inventory(to_sentence4(inventory)));
  }
  {
    // Arthur O'Dwyer, 2025
    std::string sentence = "this sentence has one of b, j, k, m, p, q, u, x, and z; two of c, g, l, r, and y; three w's;"
      "five of h, i, and v; seven of d and f; eight of a and o; nine t's; eleven s's; seventeen n's; and twenty e's";
    Inventory inventory = to_inventory(sentence);
    assert(inventory == to_inventory(to_sentence4(inventory)));
  }


#define to_sentence to_sentence5

  auto start_time = std::chrono::steady_clock::now();

  UnorderedSet<Inventory, InventoryHash> seent;
  UnorderedSet<Inventory, InventoryHash> printed;
  size_t bestlen = 9999;
  Inventory odometer = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
  size_t timer = 2'000;
  while (true) {
    advance_odometer(odometer);

    if (--timer == 0) {
      for (int i : odometer) {
        printf(" %2d", i);
      }
      printf("\r");
      timer = 2'000;
    }

    Inventory inventory = odometer;
    std::string sentence = to_sentence(inventory);
    Inventory inventory2 = to_inventory(sentence);
    while (true) {
      auto it = seent.find(inventory2);
      if (it != seent.end()) {
        break;
      }
      seent.insert(inventory2);
      inventory = inventory2;
      sentence = to_sentence(inventory);
      inventory2 = to_inventory(sentence);
    }
    if (inventory2 != inventory) {
      // It's not a solution.
    } else if (printed.find(inventory) != printed.end()) {
      // We already printed this solution.
    } else {
      printf("%8.3f ", std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start_time).count() / 1000.0);
      if (sentence.size() < bestlen) {
        printf("< ");
        bestlen = sentence.size();
      } else {
        printf("+ ");
      }
      printf("%s\n", sentence.c_str());
      printed.insert(inventory);
    }
  }  
}
