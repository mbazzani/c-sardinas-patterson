#include <algorithm>
#include <bitset>
#include <cassert>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <iterator>
#include <numeric>
#include <optional>
#include <ranges>
#include <set>
#include <thread>
#include <utility>
#include <vector>

using namespace std;

struct CodeWord {
    uint32_t len;
    uint32_t word;
};

bool operator==(const CodeWord &lhs, const CodeWord &rhs) {
    return (lhs.len == rhs.len) && (lhs.word == rhs.word);
}

bool operator!=(const CodeWord &lhs, const CodeWord &rhs) {
    return !(lhs == rhs);
}

bool operator<(const CodeWord &lhs, const CodeWord &rhs) {
    return (lhs.word == rhs.word) ? (lhs.len < rhs.len) : (lhs.word < rhs.word);
}

bool operator<=(const CodeWord &lhs, const CodeWord &rhs) {
    return (lhs.word == rhs.word) ? (lhs.len <= rhs.len)
                                  : (lhs.word <= rhs.word);
}

bool operator>(const CodeWord &lhs, const CodeWord &rhs) {
    return (lhs.word == rhs.word) ? (lhs.len > rhs.len) : (lhs.word > rhs.word);
}
bool operator>=(const CodeWord &lhs, const CodeWord &rhs) {
    return (lhs.word == rhs.word) ? (lhs.len >= rhs.len)
                                  : (lhs.word >= rhs.word);
}

std::ostream &operator<<(std::ostream &os, const CodeWord &cw) {
    os << "CodeWord { len = " << cw.len << ", word = " << bitset<32>(cw.word)
       << " }";
    return os;
}

CodeWord codeword_from_binary(const uint32_t len, const uint32_t word) {
    uint32_t shift = 32 - len;
    return CodeWord{len, word << shift};
}

uint8_t composition(const CodeWord codeword) {
    return __builtin_popcount(codeword.word);
}

bool has_prefix(const CodeWord codeword, const CodeWord prefix) {
    return __builtin_clz(codeword.word ^ prefix.word) >= prefix.len;
}

// Assumes prefix.len <= self.len
// Does not check whether prefix is truly prefix
CodeWord strip_prefix(const CodeWord codeword, const CodeWord prefix) {
    return CodeWord{
        codeword.len - prefix.len,
        codeword.word << prefix.len,
    };
}

bool is_empty(const CodeWord c) { return c.len == 0; }

void test_codeword_format() {
    CodeWord cw1 = codeword_from_binary(2, 0b11);
    CodeWord cw2 = codeword_from_binary(3, 0b111);
    assert(has_prefix(cw2, cw1));
    assert(strip_prefix(cw2, cw1) == codeword_from_binary(1, 0b1));

    cw1 = codeword_from_binary(7, 0b1001011);
    cw2 = codeword_from_binary(4, 0b1001);
    assert(has_prefix(cw1, cw2));
    assert(!has_prefix(cw2, cw1));
    assert(strip_prefix(cw1, cw2) == codeword_from_binary(3, 0b011));
}

vector<CodeWord> language_quotient(const vector<CodeWord> &N,
                                   const vector<CodeWord> &D) {
    vector<CodeWord> quotient = {};
    for (CodeWord word : D) {
        for (CodeWord prefix : N) {
            if (prefix.len > word.len)
                break;
            if (has_prefix(word, prefix))
                quotient.push_back(strip_prefix(word, prefix));
        }
    }
    return quotient;
}

// Assume N, D are sorted
vector<CodeWord> symmetric_language_quotient(const vector<CodeWord> &N,
                                             const vector<CodeWord> &D) {
    vector<CodeWord> out{};
    for (CodeWord w1 : D)
        for (CodeWord w2 : N) {
            if (has_prefix(w1, w2))
                out.push_back(strip_prefix(w1, w2));
            if (has_prefix(w1, w2))
                out.push_back(strip_prefix(w1, w2));
        }
    return out;
}

vector<CodeWord> first_set(const vector<CodeWord> &C) {
    vector<CodeWord> first = language_quotient(C, C);
    first.erase(remove_if(first.begin(), first.end(),
                          [](CodeWord c) { return c.len == 0; }),
                first.end());
    return first;
}

bool codeword_in_vec(CodeWord c, vector<CodeWord> &codewords) {
    return find(codewords.begin(), codewords.end(), c) != codewords.end();
}

// vector<CodeWord> get_next_set(const vector<CodeWord> &C,
//                               const vector<CodeWord> &S) {
//     vector<CodeWord> quotient_a;
//     language_quotient(C, S, quotient_a);
//     vector<CodeWord> quotient_b;
//     language_quotient(S, C, quotient_b);
//     sort(quotient_a.begin(), quotient_a.end());
//     sort(quotient_b.begin(), quotient_b.end());
//     vector<CodeWord> vec_union{};
//	vec_union.reserve(quotient_a.size() + quotient_b.size());
//     set_union(quotient_a.begin(), quotient_a.end(), quotient_b.begin(),
//               quotient_b.end(), std::back_inserter(vec_union));
//     return vec_union;
// }

vector<CodeWord> get_next_set(const vector<CodeWord> &C,
                              const vector<CodeWord> &S) {
    vector<CodeWord> next_set = symmetric_language_quotient(C, S);
    sort(next_set.begin(), next_set.end());
    vector<CodeWord> vec_union{};
    return vec_union;
}

// Assume both already sorted
bool intersection_not_empty(const vector<CodeWord> &set1,
                            const vector<CodeWord> &set2) {
    if (set1.empty() || set2.empty())
        return false;

    vector<CodeWord>::const_iterator it1 = set1.begin();
    vector<CodeWord>::const_iterator end1 = set1.end();
    vector<CodeWord>::const_iterator it2 = set2.begin();
    vector<CodeWord>::const_iterator end2 = set2.end();

    if (*it1 > *set2.rbegin() || *it2 > *set1.rbegin())
        return false;

    while (it1 != end1 && it2 != end2) {
        if (*it1 == *it2)
            return true;
        if (*it1 < *it2) {
            it1++;
        } else {
            it2++;
        }
    }

    return false;
}

// Given that
bool has_empty_string(vector<CodeWord> &codewords) {
    for (auto c : codewords)
        if (c.len == 0)
            return true;
    return false;
}

bool sets_are_equal(vector<CodeWord> &s1, vector<CodeWord> &s2) {
    if (s1.size() != s2.size())
        return false;
    for (uint i = 0; i < s1.size(); ++i)
        if (s1[i] != s2[i])
            return false;
    return true;
}
bool sardinas_patterson(vector<CodeWord> &codewords) {
    sort(codewords.begin(), codewords.end());
    vector<vector<CodeWord>> S = {first_set(codewords)};
    for (int i = 0;; ++i) {
        if (has_empty_string(S[i]))
            return false;
        // This could be removed
        if (intersection_not_empty(S[i], codewords))
            return false;
        // Note next_set is sorted because set union is sorted
        vector<CodeWord> next_set = get_next_set(codewords, S[i]);
        for (auto &s : S) {
            if (s == next_set) {
                return true;
            }
        }
        S.push_back(next_set);
    }

    assert(false && "Unreachable");
}

void test_sardinas_patterson() {
    vector<CodeWord> codewords = {codeword_from_binary(1, 0b0),
                                  codeword_from_binary(2, 0b01),
                                  codeword_from_binary(2, 0b11)};
    assert(sardinas_patterson(codewords));

    codewords = {codeword_from_binary(1, 0b0), codeword_from_binary(1, 0b1),
                 codeword_from_binary(2, 0b11), codeword_from_binary(2, 0b00)};
    assert(!sardinas_patterson(codewords));

    codewords = {codeword_from_binary(1, 0b0), codeword_from_binary(2, 0b01),
                 codeword_from_binary(3, 0b011),
                 codeword_from_binary(4, 0b0111)};
    assert(sardinas_patterson(codewords));

    codewords = {
        codeword_from_binary(1, 0b1),     codeword_from_binary(2, 0b01),
        codeword_from_binary(3, 0b001),   codeword_from_binary(4, 0b0001),
        codeword_from_binary(5, 0b00001),
    };
    assert(sardinas_patterson(codewords));
}

bool composition_almost_same(uint32_t a, uint32_t b, uint32_t threshold) {
    return abs(__builtin_popcount(a) - __builtin_popcount(b)) <= threshold;
}

// Assumes sorted largest length first
uint32_t count_longest_leaves(const vector<CodeWord> &code) {
    uint32_t n = 0;
    uint32_t starting_len = code[n].len;
    while (code[n].len == starting_len)
        n++;
    return n;
}

// Assume vector is sorted with longest length first
bool could_construct_prefix_code(const vector<CodeWord> &code) {
    vector<uint8_t> compositions{};
    uint32_t max_len = code[0].len;
    for (auto w : code) {
        if (w.len != max_len)
            break;
        compositions.push_back(composition(w));
    }
    sort(compositions.begin(), compositions.end(),
         [](uint8_t x, uint8_t y) { return x > y; });

    auto ptr = compositions.begin();
    uint8_t leaves_with_prev_composition = 0;
    while (ptr < compositions.end()) {
        uint8_t current_composition = *ptr;
        uint8_t leaves_with_current_composition = 0;
        while (*ptr == current_composition) {
            leaves_with_current_composition++;
            ptr++;
        }
        if (leaves_with_current_composition < leaves_with_prev_composition)
            return false;
        leaves_with_prev_composition = leaves_with_current_composition;
    }
    return true;
}

bool has_duplicate_leaves(vector<CodeWord> &code) {
    for (auto c1 : code) {
        for (auto c2 : code) {
            if (c2.len != c1.len)
                break;
            if (c2 == c1)
                return true;
        }
    }
    return false;
}

// Assumes vector is sorted largest length first
bool test_alternate_codes(vector<CodeWord> &prefix_code) {
    uint32_t num_longest_leaves = count_longest_leaves(prefix_code);
    sort(prefix_code.begin(), prefix_code.end(),
         [](CodeWord x, CodeWord y) { return x.len > y.len; });
    int max_len = prefix_code[0].len;

    vector<CodeWord> UD_code = prefix_code;

    for (uint32_t i = 0; i < pow(2, max_len); i += 1) {
        CodeWord new_codeword = codeword_from_binary(max_len, i);
        if (could_construct_prefix_code(UD_code))
            continue;
        if (has_duplicate_leaves(UD_code))
            continue;
        UD_code[0] = new_codeword;
        if (sardinas_patterson(UD_code)) {
            cout << "Counterexample found" << endl;
            for (auto c : UD_code) {
                cout << c << endl;
            }
            return true;
        }
    }
    return false;
}

set<multiset<uint32_t>>
next_topologies_from_previous(const multiset<uint> topology) {
    assert(topology.find(0) == topology.end()); // 0 not valid depth
    set<multiset<uint32_t>> possible_topologies{};
    uint32_t previous_depth = 0;
    for (uint32_t depth : topology) {
        if (depth != previous_depth) { // works b/c multiset is sorted
            multiset<uint32_t> new_topology{topology};
            new_topology.erase(new_topology.find(depth));
            new_topology.insert(depth + 1);
            new_topology.insert(depth + 1);
            possible_topologies.insert(new_topology);
        }
        previous_depth = depth;
    }
    return possible_topologies;
}

set<multiset<uint32_t>>
next_set_of_topologies(const set<multiset<uint>> previous_topology_set) {
    set<multiset<uint32_t>> next_topology_set{};
    for (auto topology : previous_topology_set) {
        set<multiset<uint32_t>> next_topologies =
            next_topologies_from_previous(topology);
        next_topology_set.insert(next_topologies.begin(),
                                 next_topologies.end());
    }
    return next_topology_set;
}

set<multiset<uint32_t>> possible_tree_topologies(const int num_leaves) {
    assert(num_leaves > 2);
    multiset<int> possible_leaf_lengths{};
    vector<set<multiset<uint32_t>>> all_tree_topologies;
    int num_elements = num_leaves;
    int max_num_topologies_per_element = 2 ^ num_leaves;
    int space_per_topology = num_leaves * sizeof(uint32_t);
    all_tree_topologies.reserve(num_elements * max_num_topologies_per_element *
                                space_per_topology);
    set<multiset<uint32_t>> three_leaf_topologies{multiset<uint>{1, 2, 2}};
    all_tree_topologies.push_back(three_leaf_topologies);
    for (int i = 1; i <= (num_leaves - 3); ++i) {
        all_tree_topologies.push_back(
            next_set_of_topologies(all_tree_topologies[i - 1]));
    }
    return *prev(all_tree_topologies.end());
}

vector<vector<uint32_t>> convert_multiset_to_vec(set<multiset<uint32_t>> set) {
    vector<vector<uint32_t>> converted{};
    for (auto multiset : set) {
        vector<uint32_t> vec{multiset.begin(), multiset.end()};
        converted.push_back(vec);
    }
    return converted;
}

vector<CodeWord> prefix_code_from_len_profile(vector<uint32_t> len_profile) {
    vector<CodeWord> prefix_code{};
    uint32_t previous_len = 0;
    uint32_t current_codeword = 0;
    for (uint32_t i = 0; i < len_profile.size(); ++i) {
        uint32_t len = len_profile[i];
        current_codeword <<= (len - previous_len);
        prefix_code.push_back(codeword_from_binary(len, current_codeword));
        previous_len = len;
        current_codeword++;
    }
    return prefix_code;
}

void test_prefix_code_generation() {
    vector<vector<uint32_t>> len_profiles =
        convert_multiset_to_vec(possible_tree_topologies(6));
    for (auto l : len_profiles) {
        auto prefix_code = prefix_code_from_len_profile(l);
        assert(sardinas_patterson(prefix_code));
    }
}

void print_code(vector<CodeWord> code) {
    cout << endl;
    for (auto c : code) {
        cout << c << ", ";
    }
    cout << endl;
}

void print_len_profile(vector<uint32_t> len_profile) {
    cout << endl;
    for (auto l : len_profile) {
        cout << l << ", ";
    }
    cout << endl;
}

void bench(uint max_length) {
    vector<CodeWord> codewords = {};
    for (uint i = 1; i <= max_length; ++i) {
        codewords.push_back(codeword_from_binary(i, 1 << (i - 1)));
    }
    sort(codewords.begin(), codewords.end(),
         [](CodeWord x, CodeWord y) { return x.len > y.len; });
    auto t1 = chrono::high_resolution_clock::now();
    cout << test_alternate_codes(codewords) << endl;
    auto t2 = chrono::high_resolution_clock::now();
    cout << "Bench took "
         << chrono::duration_cast<chrono::milliseconds>(t2 - t1).count()
         << " milliseconds\n";
}

vector<CodeWord> random_prefix_code(uint32_t num_splits) {
    assert(num_splits < 32);
    vector<CodeWord> code{codeword_from_binary(1, 0b0),
                          codeword_from_binary(1, 0b1)};
    code.reserve(num_splits + 3);
    for (uint32_t i = 0; i < num_splits; ++i) {
        uint idx = rand() % code.size();
        CodeWord codeword = code[idx];
        code.erase(code.begin() + idx);
        CodeWord left{codeword.len + 1, codeword.word};
        CodeWord right{codeword.len + 1,
                       codeword.word |
                           ((uint32_t)1 << (32 - codeword.len - 1))};
        code.push_back(left);
        code.push_back(right);
    }
    sort(code.begin(), code.end(),
         [](CodeWord x, CodeWord y) { return x.len > y.len; });
    return code;
}

bool test_multiple_codes(uint32_t n, uint32_t num_splits) {
    for (uint32_t i = 0; i < n; ++i) {
		//TODO: Reverse here instead of sort?
        vector<CodeWord> code = random_prefix_code(num_splits);
        if (test_alternate_codes(code))
            return true;
    }
    return false;
}

bool exists_prefix_code_with_same_compositions(vector<CodeWord>& code) {
     vector<pair<uint8_t, uint8_t>> compositions;
	 compositions.reserve(code.size());
	 for (auto word: code) {
		uint8_t num_ones = composition(word);
		compositions.push_back(make_pair(num_ones, word.len-num_ones));
	 }
}

int main() {
    srand(static_cast<unsigned>(time(0)));
    test_codeword_format();
    test_sardinas_patterson();
    test_prefix_code_generation();
    // bench(max_len);

    auto t1 = chrono::high_resolution_clock::now();

    uint32_t num_codes_to_test = 100000000;
    uint32_t num_splits = 24;
    uint8_t num_threads = 8;
    vector<thread> threads{};
    for (uint i = 0; i < num_threads; ++i) {
        threads.push_back(
            thread(test_multiple_codes, num_codes_to_test / 8, num_splits));
    }
    for (auto &thread : threads) {
        thread.join();
    }

    auto t2 = chrono::high_resolution_clock::now();
    cout << "Tested " << num_codes_to_test << " codes in "
         << chrono::duration_cast<chrono::milliseconds>(t2 - t1).count()
         << " milliseconds\n";

    return 0;
}

//    vector<vector<CodeWord>> prefix_codes{};
//    vector<vector<uint32_t>> len_profiles =
//        convert_multiset_to_vec(possible_tree_topologies(max_len + 1));
//    for (auto len_profile : len_profiles) {
//        prefix_codes.push_back(prefix_code_from_len_profile(len_profile));
//    }
