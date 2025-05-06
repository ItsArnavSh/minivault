#include "stringreader.h"
#include <cassert>
#include <iostream>

void testStringReader() {
    std::string text = "hello";
    StringReader reader(text);

    // Initial position should be 0
    assert(reader.getPosition() == 0);
    assert(reader.hasMore());

    // Read characters one by one
    assert(reader.readNextChar() == 'h');
    assert(reader.getPosition() == 1);

    assert(reader.readNextChar() == 'e');
    assert(reader.getPosition() == 2);

    // Go back one character and re-read
    reader.goBack();
    assert(reader.getPosition() == 1);
    assert(reader.readNextChar() == 'e');

    // Go forward two steps
    reader.goForward(2);
    assert(reader.getPosition() == 4);
    assert(reader.readNextChar() == 'o');
    assert(reader.hasMore() == false);

    // Try to go beyond end and read
    reader.goForward();
    assert(reader.hasMore() == false);

    // Go back to beginning and read again
    reader.goBack(5);
    assert(reader.getPosition() == 0);
    assert(reader.readNextChar() == 'h');

    std::cout << "All StringReader tests passed!\n";
}
