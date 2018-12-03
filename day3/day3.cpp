#include <fstream>
#include <sstream>
#include <string>
#include <regex>
#include <iostream>
#include <optional>
#include <set>

const static unsigned int FABRIC_SIZE = 1000;

struct Rectangle {
    unsigned int id;
    unsigned int x;
    unsigned int y;
    unsigned int width;
    unsigned int height;
};

class Fabric {
public:
    Fabric() {
        for (int i = 0; i < FABRIC_SIZE; i++) {
            std::fill(&_fabric_claims[i][0], &_fabric_claims[i][FABRIC_SIZE], -1);
        }
    }

    void add_claim(const Rectangle& rectangle) {
        bool is_claim_overlapping = false;
        for (unsigned int y = rectangle.y; y < rectangle.y + rectangle.height; y++) {
            for (unsigned int x = rectangle.x; x < rectangle.x + rectangle.width; x++) {
                // Part 1
                _fabric_counter[y][x]++;

                if (_fabric_counter[y][x] == 2) {
                    _overlapping_inches++;
                }


                // Part 2
                if (_fabric_claims[y][x] != -1) {
                    is_claim_overlapping = true;
                    _non_overlapping_claims.erase(_fabric_claims[y][x]);
                }
                _fabric_claims[y][x] = rectangle.id;
            }
        }

        if (!is_claim_overlapping) {
            _non_overlapping_claims.insert(rectangle.id);
        }
    }

    unsigned int count_overlapping_inches() {
        return _overlapping_inches;
    }

    const std::set<unsigned int>& get_non_overlapping_claims() {
        return _non_overlapping_claims;
    }

private:
    // Part 1
    unsigned int _fabric_counter[FABRIC_SIZE][FABRIC_SIZE] = {};
    unsigned int _overlapping_inches = 0;

    // Part 2
    int _fabric_claims[FABRIC_SIZE][FABRIC_SIZE];
    std::set<unsigned int> _non_overlapping_claims;
};

int main(int argc, const char* argv[]) {
    if (argc != 2) {
        return 1;
    }

    std::string filename(argv[1]);
    std::ifstream input_file(filename);

    std::regex regex("#(\\d+) @ (\\d+),(\\d+): (\\d+)x(\\d+)");

    Fabric fabric;

    std::string line;
    while (std::getline(input_file, line)) {

        std::smatch match;
        if (!std::regex_match(line, match, regex)) {
            return 1;
        }
        if (match.size() != 6) {
            return 1;
        }

        Rectangle rectangle;
        rectangle.id = std::stoul(match[1].str());
        rectangle.x = std::stoul(match[2].str());
        rectangle.y = std::stoul(match[3].str());
        rectangle.width = std::stoul(match[4].str());
        rectangle.height = std::stoul(match[5].str());

        fabric.add_claim(rectangle);
    }

    std::cout << fabric.count_overlapping_inches() << "\n";
    auto non_overlapping_claims = fabric.get_non_overlapping_claims();
    if (non_overlapping_claims.size() != 1) {
        std::cout << "More than one non-overlapping claim\n";
        return 1;
    }
    std::cout << *non_overlapping_claims.begin() << "\n";

    return 0;
}
