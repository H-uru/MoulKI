#include <QMap>
#include <QFile>
#include <QString>
#include <QStringList>
#include "pfConsoleParser.h"

enum ParseState { COMMAND, ARGUMENT, STRING, COMMENT };

pfConsoleParser::pfConsoleParser(QFile& file) {
    ParseState state = COMMAND;
    QString cmd;
    QString arg;
    QStringList* args = new QStringList();
    while(!file.atEnd()) {
        char c = *file.read(1).data();
        if(c == '\r')
            continue;
        switch(state) {
            case COMMAND:
                // reads the first "word" on the line
                switch(c) {
                    case ' ':
                        if(cmd.length() > 0)
                            state = ARGUMENT;
                        break;
                    case '#':
                        state = COMMENT;
                        break;
                    case '\n':
                        if(cmd.length() > 0) {
                            data.insert(cmd, args);
                            cmd.clear();
                            args = new QStringList();
                        }
                        break;
                    default:
                        cmd.append(c);
                        break;
                }
                break;
            case ARGUMENT:
                // reads space separated "words"
                switch(c) {
                    case '\n':
                    case '#':
                    case '"':
                    case ' ':
                        if(arg.length() > 0) {
                            args->append(arg);
                            arg.clear();
                        }
                        if(c == '\n') {
                            state = COMMAND;
                            data.insert(cmd, args);
                            cmd.clear();
                            args = new QStringList();
                        }else if(c == '#') {
                            state = COMMENT;
                            data.insert(cmd, args);
                            cmd.clear();
                            args = new QStringList();
                        }else if(c == '"') {
                            state = STRING;
                        }
                        break;
                    default:
                        arg.append(c);
                        break;
                }
                break;
            case STRING:
                // reads until a closing " or the end of the line
                // for now I won't bother with escape sequences
                if(c == '"' || c == '\n') {
                    args->append(arg);
                    arg.clear();
                    state = ARGUMENT;
                }else{
                    arg.append(c);
                }
            case COMMENT:
                // ignores all characters until a line break
                if(c == '\n')
                    state = COMMAND;
                break;
        }
    }
    // when we hit the end of the file, clean up any final state
    if(arg.length() > 0)
        args->append(arg);
    if(cmd.length() > 0)
        data.insert(cmd, args);
    else
        delete args;
}

const QStringList pfConsoleParser::operator [](const QString& key) {
    return *data[key];
}

const QStringList pfConsoleParser::keys() {
    return data.keys();
}

