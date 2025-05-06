#ifndef STRINGREADER_H
#define STRINGREADER_H

#include <string>

class StringReader {
public:

    // Constructor
    StringReader(const std::string& input);

    // Reads the next character and moves forward
    char readNextChar();

    // Moves back by 'steps' characters
    void goBack(int steps = 1);

    // Moves forward by 'steps' characters
    void goForward(int steps = 1);

    // Returns true if there are more characters to read
    bool hasMore() const;

    // Gets the current index
    size_t getPosition() const;

private:
    const std::string& source;
    int index = 0;
};

#endif // STRINGREADER_H
