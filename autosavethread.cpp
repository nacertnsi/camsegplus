/*
CAMSEG SCM
Copyright (C) 2008-2010 CAMSEG Technologies

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "autosavethread.h"
#include <QSettings>
#include <QTimer>

using namespace CAMSEG;

AutoSaveThread::AutoSaveThread(Campaign* p_campaign) : m_campaign(p_campaign)
{
    tmrAutoSave = NULL;
}

void AutoSaveThread::run()
{
    restartTimer();
    exec();
}

void AutoSaveThread::restartTimer()
{
    if (tmrAutoSave != NULL)
    {
        tmrAutoSave->stop();
        tmrAutoSave->deleteLater();
        tmrAutoSave = NULL;
    }

    if (m_campaign->getStorageFormat() == Storage_Local)
    {
        QSettings settings;

        if (settings.value("General/AutoSave/State").toBool())
        {
            tmrAutoSave = new QTimer();
            tmrAutoSave->start(settings.value("General/AutoSave/Interval").toInt() * 2000);//60000);
            connect(tmrAutoSave, SIGNAL(timeout()), this, SLOT(autoSave()));
        }
        else
        {
            tmrAutoSave = NULL;
        }
    }
    else
    {
        tmrAutoSave = NULL;
    }
}

void AutoSaveThread::autoSave()
{
    QString message;

    if (m_campaign->getStorageFormat() == Storage_Local && m_campaign->isModified())
    {
        if (m_campaign->saveData())
        {
            message = tr("Campaign saved");
        }
        else
        {
            message = tr("An error occured while saving the campaign : %1").arg(getLiteralFileErrorCode(m_campaign->getLastFileError()));
        }
    }

    emit showMessage(message);
}
