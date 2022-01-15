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

#ifndef CAMSEGOBJECTINFOS_H
#define CAMSEGOBJECTINFOS_H

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QTableWidget>
#include "global.h"
#include <libCAMSEG/campaign.h>

#define MAX_INFOS 8

using namespace CAMSEG;

class CAMSEGObjectInfos : public QWidget
{
    Q_OBJECT

    public:
        CAMSEGObjectInfos(ObjectType p_viewType, int p_objectInternalID, Campaign* p_campaign);

        void reloadInfos();
        void changeID(unsigned int p_objectInternalID);
void clearInfos();
    private:
        QVBoxLayout* m_mainLayout;
        QFormLayout* m_infosLayout;
        QLabel* m_objectName;

        QLabel* m_infos[MAX_INFOS];
        QTableWidget* table;

        void fillInfos();


        ObjectType m_type;
        unsigned int m_objectInternalID;
        Campaign* m_campaign;
};

#endif // CAMSEGOBJECTINFOS_H
