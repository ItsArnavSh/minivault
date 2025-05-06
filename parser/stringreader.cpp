#include "stringreader.h"

StringReader::StringReader(const std::string& input)
    : source(input), index(0) {}

char StringReader::readNextChar() {
    if (index < static_cast<int>(source.size())) {
        return source[index++];
    }
    return '\0'; // Or throw, depending on your needs
}

void StringReader::goBack(int steps) {
    index -= steps;
    if (index < 0) index = 0;
}

void StringReader::goForward(int steps) {
    index += steps;
    if (index > static_cast<int>(source.size())) {
        index = static_cast<int>(source.size());
    }
}

bool StringReader::hasMore() const {
    return index < static_cast<int>(source.size());
}

size_t StringReader::getPosition() const {
    return index;
}
