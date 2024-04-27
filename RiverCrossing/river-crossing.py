
# https://puzzling.stackexchange.com/questions/86220/farmer-needs-to-get-his-word-across-the-river/86228#86228

import random
import sys
import time

status_update_next_ = time.time()


def status_update(msg):
    global status_update_next_
    current = time.time()
    print '%s (%.2f seconds)\r' % (msg, current - status_update_next_),
    status_update_next_ = current


def set_includes(haystack, needle):
    j = 0
    hn = len(haystack)
    for c in needle:
        while j != hn and haystack[j] < c:
            j += 1
        if j == hn:
            return False
        if haystack[j] > c:
            return False
        j += 1
    return True


assert set_includes('abcdef', 'af')
assert set_includes('abcdef', 'bce')
assert set_includes('abcdef', 'abcdef')
assert set_includes('aaaaa', 'a')
assert set_includes('aaaaa', 'aa')
assert set_includes('aaaaax', 'ax')
assert set_includes('abbbcxx', 'abc')


def set_difference(haystack, needle):
    result = ''
    j = 0
    hn = len(haystack)
    for c in needle:
        while j != hn and haystack[j] < c:
            result += haystack[j]
            j += 1
        if j == hn:
            assert False
        if haystack[j] > c:
            assert False
        j += 1
    result += haystack[j:]
    return result


assert set_difference('abcdef', 'af') == 'bcde'
assert set_difference('abcdef', 'bce') == 'adf'
assert set_difference('abcdef', 'abcdef') == ''
assert set_difference('aaaaa', 'a') == 'aaaa'
assert set_difference('aaaaa', 'aa') == 'aaa'
assert set_difference('aaaaax', 'ax') == 'aaaa'
assert set_difference('abbbcxx', 'abc') == 'bbxx'


def rack_of(word):
    return ''.join(sorted(word))


def rack_can_make_word(haystack, needle):
    return set_includes(haystack, sorted(needle))


assert rack_can_make_word('ehllo', 'hello')
assert rack_can_make_word('dehillop', 'hello')
assert not rack_can_make_word('dehilop', 'hello')


def rack_can_make_pair(haystack, v, w):
    if len(v) + len(w) != len(haystack):
        return False
    return set_includes(haystack, sorted(v + w))


assert rack_can_make_pair('dehillop', 'hello', 'dip')
assert not rack_can_make_pair('dehillop', 'hello', 'di')
assert not rack_can_make_pair('dehillop', 'hello', 'dil')
assert not rack_can_make_pair('dehillop', 'hello', 'dilp')


def are_adjacent(ab, cd):
    plans, painter = ab
    planers, paint = cd
    if len(plans) == len(planers):
        return False
    if len(plans) > len(planers):
        return are_adjacent(cd, ab)
    chunklen = len(planers) - len(plans)
    assert chunklen == len(painter) - len(paint)
    for i in range(0, len(painter)):
        chunk = painter[i:i+chunklen]
        if painter[:i] + painter[i+chunklen:] != paint:
            continue
        assert len(chunk) == chunklen
        for j in range(0, len(plans)+1):
            if plans[:j] + chunk + plans[j:] == planers:
                return True
    return False


assert are_adjacent(('plans', 'painter'), ('planers', 'paint'))
assert are_adjacent(('plan', 'painters'), ('planers', 'paint'))
assert are_adjacent(('plan', 'painters'), ('planter', 'pains'))
assert are_adjacent(('per', 'plantains'), ('planter', 'pains'))
assert are_adjacent(('per', 'plantains'), ('pertain', 'plans'))


def unwind_route(finish, reachable_from):
    route = []
    while finish is not None:
        route = [finish] + route
        finish = reachable_from[finish]
    return route


def solve_words(xx, yy, words):
    letters_involved = sorted(xx + yy)
    words = [w for w in words if rack_can_make_word(letters_involved, w)]
    pairs = [(v, w) for v in words for w in words if rack_can_make_pair(letters_involved, v, w)]
    adjacencies = {
        p: [q for q in pairs if are_adjacent(p, q)]
        for p in pairs
    }
    start = (xx, yy)
    reachable_from = {
        start: None
    }
    frontier = [start]
    while frontier:
        node = frontier.pop(0)
        for neighbor in adjacencies[node]:
            if neighbor not in reachable_from:
                reachable_from[neighbor] = node
                frontier.append(neighbor)
                l, r = neighbor
                if r == xx:
                    return unwind_route(neighbor, reachable_from)
    return None


def find_interesting_lettersets(words):
    counts = {}
    sample = random.sample(words, 60)
    for xx in words:
        for yy in sample:
            letters = rack_of(xx + yy)
            counts.setdefault(letters, 0)
            counts[letters] += 1
        print xx, '\r',
    result = counts.keys()
    result.sort(key=lambda letters : counts[letters], reverse=True)
    return result


def find_interesting_puzzle(words, letters, words_by_rack):
    firstwords = words[:]
    secondwords = words[:]
    random.shuffle(firstwords)
    random.shuffle(secondwords)
    for xx in firstwords:
        if not rack_can_make_word(letters, xx):
            continue
        remaining_rack = set_difference(letters, rack_of(xx))
        for yy in words_by_rack.get(remaining_rack, []):
            path = solve_words(xx, yy, words)
            if path is None:
                print (xx, yy), 'is undoable'
            else:
                print (xx, yy), 'is DOABLE in', len(path), 'steps'


def find_interesting_puzzle_2(words, letters, words_by_rack):
    nodes = []
    for xx in words:
        if not rack_can_make_word(letters, xx):
            continue
        remaining_rack = set_difference(letters, rack_of(xx))
        for yy in words_by_rack.get(remaining_rack, []):
            node = (xx, yy)
            nodes.append(node)
    status_update('Got wordlist')
    path_lengths = {
        (n1, n2): 1 if are_adjacent(n1, n2) else None
        for n1 in nodes for n2 in nodes if (n1 != n2)
    }
    status_update('Inited path_lengths')
    iterations = 0
    while True:
        new_path_lengths = {}
        for (edge, distance) in path_lengths.iteritems():
            if distance is None: continue
            (n1, n2) = edge
            for (edge2, distance2) in path_lengths.iteritems():
                if distance2 is None: continue
                (n3, n4) = edge2
                if n2 != n3: continue
                if n1 == n4: continue
                sum_distance = distance + distance2
                old_distance = path_lengths[(n1, n4)]
                if old_distance is None or (sum_distance < old_distance):
                    new_path_lengths[(n1, n4)] = sum_distance
        if not new_path_lengths:
            break
        path_lengths.update(new_path_lengths)
        iterations += 1
        status_update('Updated path_lengths (%d)' % iterations)
    puzzles = {}
    status_update('Getting solutions')
    for (edge, distance) in path_lengths.iteritems():
        (n1, n2) = edge
        (start, r) = n1
        (l, finish) = n2
        if (start == finish) and (distance is not None):
            key = (start, r)
            if (key not in puzzles):
                puzzles[key] = (edge, distance)
            else:
                (e, d) = puzzles[key]
                if distance < d:
                    puzzles[key] = (edge, distance)
    status_update('Printing solutions')
    for (edge, distance) in puzzles.itervalues():
        print (edge, distance)

if __name__ == '__main__':
    with open('simple-2of12-words-and-neologisms.txt', 'r') as f:
        words = [w.strip() for w in f.readlines()]

    words_by_rack = {}
    for w in words:
        words_by_rack.setdefault(rack_of(w), []).append(w)

    if sys.argv[1] == 'pairs':
        print find_interesting_lettersets(words)
    elif sys.argv[1] == 'puzzles':
        ls = find_interesting_lettersets(words)
        for letters in ls:
            print 'Trying letters=%s                   ' % letters
            find_interesting_puzzle_2(words, letters, words_by_rack)
    elif len(sys.argv) == 2:
        find_interesting_puzzle_2(words, rack_of(sys.argv[1]), words_by_rack)
    elif len(sys.argv) == 3:
        solution = solve_words(sys.argv[1], sys.argv[2], words)
        print solution
