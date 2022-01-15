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

#ifndef AUTOSAVETHREAD_H
#define AUTOSAVETHREAD_H

#include <QThread>
#include <libCAMSEG/campaign.h>

class QTimer;

class AutoSaveThread : public QThread
{
    Q_OBJECT

    public:
        AutoSaveThread(CAMSEG::Campaign* p_campaign);

        void run();

        void restartTimer();

    public slots:
        void autoSave();

signals:
        void showMessage(QString p_message);

    private:
        CAMSEG::Campaign* m_campaign;
        QTimer* tmrAutoSave;
};

#endif // AUTOSAVETHREAD_H
