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

#ifndef CAMSEGPRODUCTLIST_H
#define CAMSEGPRODUCTLIST_H

#include <QWidget>
#include <libCAMSEG/campaign.h>
#include <libCRL/chart.h>

class CAMSEGProductList : public QWidget
{
    Q_OBJECT

    public:
                    struct CatalogueRow
                    {
                        QString code;
                        QImage image;
                        QString title;
                        QString description;
                        QString price;
                    };

                    enum DeviceType
                    {
                        Device_Widget,
                        Device_Printer,
                        Device_File
                    };

                    enum DisplayType
                    {
                        Catalogue_SimpleList,
                        Catalogue_NoPictures,
                        Catalogue_WithPictures
                    };

                    CAMSEGProductList(CAMSEG::Campaign* p_campaign, QWidget* parent = NULL);

        QSize       minimumSizeHint();
        QSize       sizeHint();

        bool        saveAsPDF(const QString & p_path);
        void        print(QPrinter* p_printer);

        DisplayType displayType() const;
        bool        isGridVisible() const;

        void        setDisplayType(const DisplayType p_type);
        void        setGridVisible(const bool p_value);

    protected:
        void        paintEvent(QPaintEvent *event);

    private:
        void        paintOnDevice(QPaintDevice * p_device, DeviceType p_type = Device_Widget);

        unsigned int        verticalSize;

        QList<CatalogueRow> listRows;
        QString             m_campaignName;

        DisplayType         m_type;
        bool                m_showGrid;

        CAMSEG::Campaign*   m_campaign;
};

#endif // CAMSEGPRODUCTLIST_H
