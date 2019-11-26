#ifndef CSIZEUTIL_H
#define CSIZEUTIL_H


class QFont;
class QString;

class CSizeUtil
{
public:
    ~CSizeUtil(){}
    static CSizeUtil &instance();
    int defaultFontHeight();
    int widthWithDefaultFont(const QString & text);
    int widthWithFont(const QString &text, int fontPointSize);
    int fontHeight(int fontPointSize);
    double dpiFactor();

private:
    CSizeUtil() {}
    CSizeUtil(const CSizeUtil&);
    CSizeUtil& operator= (const CSizeUtil&);
};

#endif // CSIZEUTIL_H
