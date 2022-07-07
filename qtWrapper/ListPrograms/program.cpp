#include "program.h"

#include <Windows.h>
#include <QtWin>
#if QT_VERSION < QT_VERSION_CHECK(5, 12, 0)
#include <QRegExp>
#else
#include <QRegularExpression>
#endif

QSoftware::QSoftware(const Software& software)
{
    // Arch
    switch(software.Architecture)
    {
    case Arch_e::UnKnown:
        this->Arch = ARCH_UNKNOWN;
        break;
    case Arch_e::X86:
        this->Arch = ARCH_X86;
        break;
    case Arch_e::X64:
        this->Arch = ARCH_X64;
        break;
    default:
        this->Arch = ARCH_UNKNOWN;
    }
    // DisplayName
    this->DisplayName = QString::fromStdWString(software.DisplayName);
    // InstallLocation
    this->InstalledLocation = QString::fromStdWString(software.InstallLocation);
    // Version
    this->Version = QString::fromStdWString(software.Version);

    // Create Icon from the Icon String
    // Examine the Icon String.
    QString IconString = QString::fromStdWString(software.Icon);
    int index = 0;

#if QT_VERSION < QT_VERSION_CHECK(5, 12, 0)
    QRegExp match("(.+)\\,(\\d+)");
    if(match.exactMatch(IconString))
#else
    QRegularExpression regex(QRegularExpression::anchoredPattern(QLatin1String("(.+)\\,(\\d+)")));
    QRegularExpressionMatch match = regex.match(IconString);
    if (match.hasMatch())
#endif
    {
        IconString = match.capturedTexts().at(1);
        index = match.capturedTexts().at(2).toInt();
    }
    if(IconString.startsWith("\"")&&IconString.endsWith("\""))
    {
        int size = IconString.size();
        IconString = IconString.left(size - 1).right(size - 2);
    }

    WCHAR out[MAX_PATH];
    GetLongPathNameW(IconString.toStdWString().c_str(), out, MAX_PATH);

    QPixmap SoftwareIcon;
    HICON icon;
    UINT Unused;
    UINT ret = PrivateExtractIconsW(out, index, 48, 48, &icon, &Unused, 1, 0);
    if( ret == 1 )
        SoftwareIcon = QtWin::fromHICON(icon);

    // Icon
    this->Icon = SoftwareIcon;
}


QList<QSoftware*>* QSoftware::SoftwaresOnPC()
{
    QList<QSoftware*>* SoftwareList = new QList<QSoftware*>();
    std::vector<Software> list;
    InstalledPrograms::GetInstalledPrograms(list, false);
    for(std::vector<Software>::iterator it = list.begin(); it != list.end(); it++)
    {
        QSoftware* soft = new QSoftware(*it);
        SoftwareList->append(soft);
    }
    return SoftwareList;
}
