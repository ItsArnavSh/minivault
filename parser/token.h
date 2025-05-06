    #ifndef TOKEN_H
    #define TOKEN_H
    #include <string>
    typedef enum TokenType {
        // Keywords
        CREATE,
        TABLE,
        SELECT,
        INSERT,
        INTO,
        VALUES,
        WHERE,
        FROM,
        ORDER,
        BY,
        ASC,
        DESC,
        COLUMN_DEF,
        // Symbols
        OPEN_PAREN,     // (
        CLOSE_PAREN,    // )
        COMMA,          // ,
        SEMICOLON,      // ;

        // Operators
        EQUAL,          // =
        EQUALEQUAL,
        NOT_EQUAL,      // !=
        LESS_THAN,      // <
        GREATER_THAN,   // >
        LESS_EQUAL,     // <=
        GREATER_EQUAL,  // >=
        BANGEQUAL,
        BANG,

        PLUS,           // +
        MINUS,          // -
        STAR,           // *
        SLASH,          // /
        PERCENT,        // %

        AND,
        OR,
        NOT,

        // Identifiers
        IDENTIFIER,     // table_name, column_name, etc.

        // Literals
        NUMBER_LITERAL,
        STRING_LITERAL,

        //Attr
        BLOB,
        TXT,
        NUM
    } TokenType;


    class Token {
    public:
        TokenType type;
        std::string value;

        // Constructors for different token types
        Token(){}
        Token(TokenType type);           // For operators
        Token(std::string literal);      // For identifiers (sets identifierKey)
        Token(int number);               // For numbers (sets number)
        void printToken();
    };
    #endif
