#ifndef PARSER_H
#define PARSER_H

#include <QObject>

class Parser
{
public:
    Parser();
    std::vector<std::string> parseSpace(std::string, bool);
};

#endif // PARSER_H
