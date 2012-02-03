#ifndef PFCONSOLEPARSER_H
#define PFCONSOLEPARSER_H

class pfConsoleParser
{
public:
    pfConsoleParser(QFile& file);
    const QStringList operator[](const QString& key);
    const QStringList keys();
private:
    QMap<QString, QStringList*> data;
};

#endif // PFCONSOLEPARSER_H
